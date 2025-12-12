#pragma once

#include <iostream>
#include <string>

inline void println(std::string_view message) {
    std::cout << message << std::endl;
}
