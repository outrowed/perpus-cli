#include <iostream>
#include <string>

#pragma once

void println(std::string_view message) {
    std::cout << message << std::endl;
}