use clap::Parser;
use regex::RegexBuilder;
use rocksdb_rs as _;
use std::collections::{HashMap, HashSet};
use std::path::PathBuf;
use walkdir::WalkDir;

// TODO: list files in order of number of dependencies (transitive)
// TODO: show dependency tree of files (root file -> all dependencies -> all dependencies of dependencies)
// TODO: show dependency tree of files (root file -> all dependents -> all dependents of dependents)

const GENERATED_INCLUDE_DIR: &str = env!("ROCKSDB_GENERATED_INCLUDE");

/// This tool is used to interact with the dependency graph of the C++ files.
#[derive(Debug, Parser)]
#[command(version, about, long_about = None)]
enum Commands {
    /// Check for missing include paths
    MissingIncludePaths,
    /// Show files in order of number of dependencies
    Dependencies,
}

fn main() -> anyhow::Result<()> {
    let commands = Commands::parse();

    let manifest_dir = env!("CARGO_MANIFEST_DIR");
    let repo_root = std::fs::canonicalize(format!("{manifest_dir}/../.."))?;
    let cxx_root = repo_root.join("rocksdb-cxx");

    let paths = get_all_paths(&cxx_root)?;

    let re = RegexBuilder::new(r#"^#include "(\S+)""#)
        .multi_line(true)
        .build()?;

    let mut path_mappings = HashMap::new();

    // map every file's include statements
    for path in &paths {
        let content = std::fs::read_to_string(path)?;
        let mut include_mappings = HashSet::new();
        for include in re.captures_iter(&content).map(|c| c[1].to_string()) {
            let include_mapping = IncludeMapping::new(include, path, &cxx_root);
            include_mappings.insert(include_mapping);
        }
        path_mappings.insert(path.clone(), include_mappings);
    }

    match commands {
        Commands::MissingIncludePaths => {
            show_missing_include_paths(&path_mappings);
        }
        Commands::Dependencies => {
            show_order_of_dependencies(&path_mappings);
        }
    }

    Ok(())
}

fn show_order_of_dependencies(path_mappings: &HashMap<PathBuf, HashSet<IncludeMapping>>) {
    println!("Files in order of number of dependencies:");

    let mut files = path_mappings.keys().collect::<Vec<_>>();
    files.sort_by_key(|k| path_mappings.get(*k).unwrap().len());
    for (idx, file) in files.iter().enumerate() {
        let deps = &path_mappings[*file];
        println!("{} - {} deps - {:?}", idx + 1, deps.len(), file);
    }
}

fn show_missing_include_paths(path_mappings: &HashMap<PathBuf, HashSet<IncludeMapping>>) {
    let mut counter = 0;
    println!("Files with missing include paths:");
    for (path, mappings) in path_mappings {
        let mut printed_file = false;

        for include_mapping in mappings.iter().filter(|m| m.path.is_none()) {
            if !printed_file {
                printed_file = true;
                counter += 1;
                println!("{counter} - {:?}", path);
            }
            println!("    {:?}", include_mapping.include);
        }
    }
}

/// Get paths for C++ files from rocksdb-cxx and all header files generated from rocksdb-rs
fn get_all_paths(cxx_root: &PathBuf) -> anyhow::Result<HashSet<PathBuf>> {
    // get all c++ files from rocksdb-cxx
    let mut paths = WalkDir::new(&cxx_root)
        .into_iter()
        .filter_map(|e| e.ok())
        .filter(|e| {
            e.path()
                .extension()
                .map_or(false, |e| e == "cc" || e == "c" || e == "h")
        })
        .map(|e| e.into_path())
        .collect::<HashSet<_>>();

    // get all header files generated from rocksdb-rs
    for path in WalkDir::new(GENERATED_INCLUDE_DIR) {
        let path = path?;
        if path.path().extension().map_or(false, |e| e == "h") {
            paths.insert(path.path().to_path_buf());
        }
    }

    Ok(paths)
}

#[derive(Debug, Eq, PartialEq, Hash)]
struct IncludeMapping {
    include: String,
    path: Option<PathBuf>,
}

impl IncludeMapping {
    /// Map an include statement to a filesystem path
    fn new(include: String, current_path: &PathBuf, cxx_root: &PathBuf) -> IncludeMapping {
        // find relative to cxx_root
        let needle = cxx_root.join(&include);
        if needle.exists() {
            return IncludeMapping {
                include,
                path: Some(needle),
            };
        }

        // find relative to include folder
        let needle = cxx_root.join("include").join(&include);
        if needle.exists() {
            return IncludeMapping {
                include,
                path: Some(needle),
            };
        }

        // find relative to generated include folder
        let needle = PathBuf::from(GENERATED_INCLUDE_DIR).join(&include);
        if needle.exists() {
            return IncludeMapping {
                include,
                path: Some(needle),
            };
        }

        // find relative to current path
        let needle = current_path
            .parent()
            .expect("file will always have a parent directory")
            .join(&include);
        if needle.exists() {
            return IncludeMapping {
                include,
                path: Some(needle),
            };
        }

        IncludeMapping {
            include,
            path: None,
        }
    }
}
