use build_common::{get_clang_defines, get_clang_flags, get_files};
use clang::{Clang, Index};
use rocksdb_rs as _;

fn main() {
    let clang_context = Clang::new().unwrap();
    let index = Index::new(&clang_context, true, true);

    let mut arguments = Vec::new();

    // use c++
    arguments.push("-xc++".to_string());

    for flag in get_clang_flags() {
        arguments.push(flag.to_string());
    }

    for def in get_clang_defines() {
        arguments.push(format!("-D{}", def));
    }

    let system_paths = clang_sys::support::Clang::find(None, &[])
        .unwrap()
        .cpp_search_paths
        .unwrap();
    for path in system_paths {
        arguments.push(format!("-isystem{}", path.display()));
    }

    // we're currently at tools/dependency-graph
    let manifest_dir = env!("CARGO_MANIFEST_DIR");
    let repo_root = std::fs::canonicalize(format!("{manifest_dir}/../..")).unwrap();

    let include_dirs: Vec<_> = ["rocksdb-cxx/include", "rocksdb-cxx"]
        .into_iter()
        .map(|p| repo_root.join(p))
        .collect();
    for include_dir in &include_dirs {
        arguments.push(format!("-I{}", include_dir.display()));
    }

    // include the generated headers
    let generated_include = env!("ROCKSDB_GENERATED_INCLUDE");
    arguments.push(format!("-I{}", generated_include));

    let files = get_files();
    let count = files.len();

    for (i, file_path) in files.iter().enumerate() {
        println!("parsing {i} of {count} - {file_path}");
        let tu = index
            .parser(&file_path)
            .arguments(&arguments)
            .skip_function_bodies(true)
            .parse()
            .unwrap();

        if tu.get_diagnostics().len() > 0 {
            break;
        }
    }
}
