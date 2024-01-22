use clang::{Clang, Index};
use std::path::Path;

fn main() {
    let clang_context = Clang::new().unwrap();
    let index = Index::new(&clang_context, true, true);

    let mut arguments = Vec::new();

    // use c++
    arguments.push("-xc++".to_string());
    arguments.push("-std=c++17".to_string());

    // verbose
    arguments.push("-v".to_string());

    let system_paths = clang_sys::support::Clang::find(None, &[])
        .unwrap()
        .cpp_search_paths
        .unwrap();
    for path in system_paths {
        arguments.push(format!("-isystem{}", path.to_str().unwrap()));
    }

    // we're currently at tools/dependency-graph
    let repo_root = Path::new(env!("CARGO_MANIFEST_DIR")).join("../..");

    let include_dirs: Vec<_> = ["rocksdb-cxx/include", "rocksdb-cxx"]
        .into_iter()
        .map(|p| repo_root.join(p))
        .collect();
    for include_dir in &include_dirs {
        arguments.push(format!("-I{}", include_dir.to_str().unwrap()));
    }

    let res = index
        .parser(repo_root.join("rocksdb-cxx/util/common_ffi.cc"))
        .keep_going(true)
        .arguments(&arguments)
        .parse();
    println!("{:?}", res);
}
