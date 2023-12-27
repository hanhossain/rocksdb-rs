#[=======================================================================[.md:
** EXPERIMENTAL **: This function is currently still considered experimental
  and is not officially released yet. Feedback and Suggestions are welcome.

ANCHOR: corrosion_add_cxxbridge_custom

```cmake
corrosion_add_cxxbridge_custom(cxx_target
        CRATE <imported_target_name>
        [FILES <file1.rs> <file2.rs>]
)
```

Adds build-rules to create C++ bindings using the [cxx] crate.

### Arguments:
* `cxxtarget`: Name of the C++ library target for the bindings, which corrosion will create.
* **FILES**: Input Rust source file containing #[cxx::bridge].
* **CRATE**: Name of an imported Rust target. Note: Parameter may be renamed before release

#### Currently missing arguments

The following arguments to cxxbridge **currently** have no way to be passed by the user:
- `--cfg`
- `--cxx-impl-annotations`
- `--include`

The created rules approximately do the following:
- Check which version of `cxx` the Rust crate specified by the `CRATE` argument depends on.
- Check if the exact same version of `cxxbridge-cmd` is installed (available in `PATH`)
- If not, create a rule to build the exact same version of `cxxbridge-cmd`.
- Create rules to run `cxxbridge` and generate
  - The `rust/cxx.h` header
  - A header and source file for each of the files specified in `FILES`
- The generated sources (and header include directories) are added to the `cxxtarget` CMake
  library target.

### Limitations

We currently require the `CRATE` argument to be a target imported by Corrosion, however,
Corrosion does not import `rlib` only libraries. As a workaround users can add
`staticlib` to their list of crate kinds. In the future this may be solved more properly,
by either adding an option to also import Rlib targets (without build rules) or by
adding a `MANIFEST_PATH` argument to this function, specifying where the crate is.

### Contributing

Specifically some more realistic test / demo projects and feedback about limitations would be
welcome.

[cxx]: https://github.com/dtolnay/cxx

ANCHOR_END: corrosion_add_cxxbridge_custom
#]=======================================================================]
function(corrosion_add_cxxbridge_custom cxx_target)
    set(OPTIONS)
    set(ONE_VALUE_KEYWORDS CRATE)
    set(MULTI_VALUE_KEYWORDS FILES)
    cmake_parse_arguments(PARSE_ARGV 1 _arg "${OPTIONS}" "${ONE_VALUE_KEYWORDS}" "${MULTI_VALUE_KEYWORDS}")

    set(required_keywords CRATE FILES)
    foreach(keyword ${required_keywords})
        if(NOT DEFINED "_arg_${keyword}")
            message(FATAL_ERROR "Missing required parameter `${keyword}`.")
        elseif("${_arg_${keyword}}" STREQUAL "")
            message(FATAL_ERROR "Required parameter `${keyword}` may not be set to an empty string.")
        endif()
    endforeach()

    get_target_property(manifest_path "${_arg_CRATE}" INTERFACE_COR_PACKAGE_MANIFEST_PATH)

    if(NOT EXISTS "${manifest_path}")
        message(FATAL_ERROR "Internal error: No package manifest found at ${manifest_path}")
    endif()

    get_filename_component(manifest_dir ${manifest_path} DIRECTORY)

    execute_process(COMMAND ${CMAKE_COMMAND} -E env
            "CARGO_BUILD_RUSTC=${_CORROSION_RUSTC}"
            ${_CORROSION_CARGO} tree -i cxx --depth=0
            WORKING_DIRECTORY "${manifest_dir}"
            RESULT_VARIABLE cxx_version_result
            OUTPUT_VARIABLE cxx_version_output
    )
    if(NOT "${cxx_version_result}" EQUAL "0")
        message(FATAL_ERROR "Crate ${_arg_CRATE} does not depend on cxx.")
    endif()
    if(cxx_version_output MATCHES "cxx v([0-9]+.[0-9]+.[0-9]+)")
        set(cxx_required_version "${CMAKE_MATCH_1}")
    else()
        message(FATAL_ERROR "Failed to parse cxx version from cargo tree output: `cxx_version_output`")
    endif()

    # First check if a suitable version of cxxbridge is installed
    find_program(INSTALLED_CXXBRIDGE cxxbridge PATHS "$ENV{HOME}/.cargo/bin/")
    mark_as_advanced(INSTALLED_CXXBRIDGE)
    if(INSTALLED_CXXBRIDGE)
        execute_process(COMMAND ${INSTALLED_CXXBRIDGE} --version OUTPUT_VARIABLE cxxbridge_version_output)
        if(cxxbridge_version_output MATCHES "cxxbridge ([0-9]+.[0-9]+.[0-9]+)")
            set(cxxbridge_version "${CMAKE_MATCH_1}")
        else()
            set(cxxbridge_version "")
        endif()
    endif()

    set(cxxbridge "")
    if(cxxbridge_version)
        if(cxxbridge_version VERSION_EQUAL cxx_required_version)
            set(cxxbridge "${INSTALLED_CXXBRIDGE}")
            if(NOT TARGET "cxxbridge_v${cxx_required_version}")
                # Add an empty target.
                add_custom_target("cxxbridge_v${cxx_required_version}"
                )
            endif()
        endif()
    endif()

    # No suitable version of cxxbridge was installed, so use custom target to build correct version.
    if(NOT cxxbridge)
        if(NOT TARGET "cxxbridge_v${cxx_required_version}")
            add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/corrosion/cxxbridge_v${cxx_required_version}/bin/cxxbridge"
                    COMMAND
                    ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/corrosion/cxxbridge_v${cxx_required_version}"
                    COMMAND
                    ${CMAKE_COMMAND} -E env
                    "CARGO_BUILD_RUSTC=${_CORROSION_RUSTC}"
                    ${_CORROSION_CARGO} install
                    cxxbridge-cmd
                    --version "${cxx_required_version}"
                    --root "${CMAKE_BINARY_DIR}/corrosion/cxxbridge_v${cxx_required_version}"
                    --quiet
                    # todo: use --target-dir to potentially reuse artifacts
                    COMMENT "Building cxxbridge (version ${cxx_required_version})"
            )
            add_custom_target("cxxbridge_v${cxx_required_version}"
                    DEPENDS "${CMAKE_BINARY_DIR}/corrosion/cxxbridge_v${cxx_required_version}/bin/cxxbridge"
            )
        endif()
        set(cxxbridge "${CMAKE_BINARY_DIR}/corrosion/cxxbridge_v${cxx_required_version}/bin/cxxbridge")
    endif()


    # The generated folder structure will be of the following form
    #
    #    CMAKE_CURRENT_BINARY_DIR
    #        corrosion_generated
    #            cxxbridge
    #                <cxx_target>
    #                    include
    #                        rocksdb-rs
    #                            src
    #                                <headers>
    #                        rust
    #                            cxx.h
    #                    src
    #                        <sourcefiles>
    #            cbindgen
    #                ...
    #            other
    #                ...

    set(corrosion_generated_dir "${CMAKE_CURRENT_BINARY_DIR}/corrosion_generated")
    set(generated_dir "${corrosion_generated_dir}/cxxbridge/${cxx_target}")
    set(header_placement_dir "${generated_dir}/include/rocksdb-rs/src")
    set(source_placement_dir "${generated_dir}/src")

    add_library(${cxx_target} STATIC)
    target_include_directories(${cxx_target}
            PUBLIC
            $<BUILD_INTERFACE:${generated_dir}/include>
            $<INSTALL_INTERFACE:include>
    )

    # cxx generated code is using c++11 features in headers, so propagate c++11 as minimal requirement
    target_compile_features(${cxx_target} PUBLIC cxx_std_11)

    # Todo: target_link_libraries is only necessary for rust2c projects.
    # It is possible that checking if the rust crate is an executable is a sufficient check,
    # but some more thought may be needed here.
    # Maybe we should also let the user do this, since for c2rust, the user also has to call
    # corrosion_link_libraries() themselves.
    get_target_property(crate_target_type ${_arg_CRATE} TYPE)
    if (NOT crate_target_type STREQUAL "EXECUTABLE")
        target_link_libraries(${cxx_target} PRIVATE ${_arg_CRATE})
    endif()

    file(MAKE_DIRECTORY "${generated_dir}/include/rust")
    add_custom_command(
            OUTPUT "${generated_dir}/include/rust/cxx.h"
            COMMAND
            ${cxxbridge} --header --output "${generated_dir}/include/rust/cxx.h"
            DEPENDS "cxxbridge_v${cxx_required_version}"
            COMMENT "Generating rust/cxx.h header"
    )

    foreach(filepath ${_arg_FILES})
        get_filename_component(filename ${filepath} NAME_WE)
        get_filename_component(directory ${filepath} DIRECTORY)
        set(directory_component "")
        if(directory)
            set(directory_component "${directory}/")
        endif()
        # todo: convert potentially absolute paths to relative paths..
        set(cxx_header ${directory_component}${filename}.rs.h)
        set(cxx_source ${directory_component}${filename}.cpp)

        # todo: not all projects may use the `src` directory.
        set(rust_source_path "${manifest_dir}/src/${filepath}")

        file(MAKE_DIRECTORY "${header_placement_dir}/${directory}" "${source_placement_dir}/${directory}")

        add_custom_command(
                OUTPUT
                "${header_placement_dir}/${cxx_header}"
                "${source_placement_dir}/${cxx_source}"
                COMMAND
                ${cxxbridge} ${rust_source_path} --header --output "${header_placement_dir}/${cxx_header}"
                COMMAND
                ${cxxbridge} ${rust_source_path}
                --output "${source_placement_dir}/${cxx_source}"
                --include "rocksdb-rs/src/${cxx_header}"
                DEPENDS "cxxbridge_v${cxx_required_version}" "${rust_source_path}"
                COMMENT "Generating cxx bindings for crate ${_arg_CRATE}"
        )

        target_sources(${cxx_target}
                PRIVATE
                "${header_placement_dir}/${cxx_header}"
                "${generated_dir}/include/rust/cxx.h"
                "${source_placement_dir}/${cxx_source}"
        )
    endforeach()
endfunction()
