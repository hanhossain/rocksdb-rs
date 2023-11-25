#include "rocksdb/common_ffi.h"
#include <memory>
#include <string>

namespace rocksdb {
    // Create a unique ptr of an empty string
    std::unique_ptr<std::string> make_string() {
        return std::make_unique<std::string>();
    }
}