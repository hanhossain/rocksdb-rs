use regex::RegexBuilder;
use std::collections::HashMap;
use std::path::PathBuf;
use walkdir::WalkDir;

#[derive(Debug)]
struct IncludeMapping {
    include: String,
    path: Option<PathBuf>,
}

impl IncludeMapping {
    fn new(include: String) -> Self {
        Self {
            include,
            path: None,
        }
    }
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
        .map(|p| (p, HashMap::new()))
        .collect::<HashMap<PathBuf, HashMap<String, IncludeMapping>>>();

    let path = paths.keys().next().unwrap().clone();
    println!("{:?}", path);
    let content = std::fs::read_to_string(&path)?;

    let re = RegexBuilder::new("^#include \"(\\S+)\"")
        .multi_line(true)
        .build()?;

    let mut include_mappings = HashMap::new();
    for (_, [include]) in re.captures_iter(&content).map(|c| c.extract()) {
        if !include_mappings.contains_key(include) {
            let path = find_path(&cxx_root, include, &path, &paths);
            let include_mapping = IncludeMapping {
                include: include.to_string(),
                path,
            };
            include_mappings.insert(include.to_string(), include_mapping);
        }
    }

    println!("{:#?}", include_mappings);

    Ok(())
}

fn find_path(
    cxx_root: &PathBuf,
    include: &str,
    current_path: &PathBuf,
    paths: &HashMap<PathBuf, HashMap<String, IncludeMapping>>,
) -> Option<PathBuf> {
    // find relative to cxx_root
    let needle = cxx_root.join(include);
    if paths.contains_key(&needle) {
        return Some(needle);
    }

    // find relative to include folder
    let needle = cxx_root.join("include").join(include);
    if paths.contains_key(&needle) {
        return Some(needle);
    }

    // find relative to current path
    let needle = current_path.join(include);
    if paths.contains_key(&needle) {
        return Some(needle);
    }

    None
}
