#pragma once

#include <memory>

#include "rust/cxx.h"

namespace rocksdb {
    // Create a unique ptr of an empty string
    std::unique_ptr<std::string> make_empty_string();

    // Create a unique ptr of a string
    std::unique_ptr<std::string> make_string(rust::str s);
}
