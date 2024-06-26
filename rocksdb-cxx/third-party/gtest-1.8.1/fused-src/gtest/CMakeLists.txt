add_library(gtest gtest-all.cc)

# Add dependency of gtest on thread library
target_link_libraries(gtest ${CMAKE_THREAD_LIBS_INIT})
target_link_options(gtest PRIVATE ${LINKER_OVERRIDE})
