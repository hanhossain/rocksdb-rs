//
// Created by Han Hossain on 11/25/23.
//

#pragma once

#include <memory>

namespace rocksdb {
    // Create a new string
    std::unique_ptr<std::string> make_string();
}
