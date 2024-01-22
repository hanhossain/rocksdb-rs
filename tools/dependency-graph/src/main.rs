use build_common::{get_clang_defines, get_clang_flags};
use clang::{Clang, Index};

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

    // verbose
    arguments.push("-v".to_string());

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

    let res = index
        .parser(repo_root.join("rocksdb-cxx/util/common_ffi.cc"))
        .keep_going(true)
        .arguments(&arguments)
        .parse();
    println!("{:?}", res);
}
