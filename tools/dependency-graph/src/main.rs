use regex::RegexBuilder;
use std::collections::{HashMap, HashSet};
use std::path::PathBuf;
use walkdir::WalkDir;

#[derive(Debug)]
struct IncludeMapping {
    include: String,
    path: Option<PathBuf>,
}

fn main() -> anyhow::Result<()> {
    let manifest_dir = env!("CARGO_MANIFEST_DIR");
    let repo_root = std::fs::canonicalize(format!("{manifest_dir}/../.."))?;
    let cxx_root = repo_root.join("rocksdb-cxx");

    let paths = WalkDir::new(&cxx_root)
        .into_iter()
        .filter_map(|e| e.ok())
        .filter(|e| {
            e.path()
                .extension()
                .map_or(false, |e| e == "cc" || e == "c" || e == "h")
        })
        .map(|e| e.into_path())
        .collect::<HashSet<_>>();

    let re = RegexBuilder::new("^#include \"(\\S+)\"")
        .multi_line(true)
        .build()?;

    let mut path_mappings = HashMap::new();

    for path in &paths {
        let content = std::fs::read_to_string(path)?;
        let mut include_mappings = HashMap::new();
        for (_, [include]) in re.captures_iter(&content).map(|c| c.extract()) {
            if !include_mappings.contains_key(include) {
                let path = find_path(&cxx_root, include, path, &paths);
                let include_mapping = IncludeMapping {
                    include: include.to_string(),
                    path,
                };
                include_mappings.insert(include.to_string(), include_mapping);
            }
        }
        path_mappings.insert(path.clone(), include_mappings);
    }

    for (path, mappings) in path_mappings {
        if mappings.values().any(|m| m.path.is_none()) {
            println!("{}: {:#?}", path.display(), mappings);
            break;
        }
    }

    Ok(())
}

fn find_path(
    cxx_root: &PathBuf,
    include: &str,
    current_path: &PathBuf,
    paths: &HashSet<PathBuf>,
) -> Option<PathBuf> {
    // find relative to cxx_root
    let needle = cxx_root.join(include);
    if paths.contains(&needle) {
        return Some(needle);
    }

    // find relative to include folder
    let needle = cxx_root.join("include").join(include);
    if paths.contains(&needle) {
        return Some(needle);
    }

    // find relative to current path
    let needle = current_path
        .parent()
        .expect("file will always have a parent directory")
        .join(include);
    if paths.contains(&needle) {
        return Some(needle);
    }

    None
}
