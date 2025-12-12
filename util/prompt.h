#pragma once

#include <iostream>
#include <limits>
#include <string>

#include "print.h"

inline std::string prompt(const std::string& message) {
    std::string input;
    std::cout << message;
    std::getline(std::cin, input);
    return input;
}

inline std::string prompt_required(const std::string& message) {
    while (true) {
        std::string answer = prompt(message);
        if (!answer.empty()) {
            return answer;
        }
        println("Input is required. Please type something.");
    }
}

inline int prompt_int(const std::string& message) {
    std::string input;
    int value;
    while (true) {
        std::cout << message;
        std::getline(std::cin, input);
        try {
            value = std::stoi(input);
            break;
        } catch (const std::invalid_argument&) {
            std::cout << "Invalid input. Please enter a valid integer." << std::endl;
        }
    }
    return value;
}

inline float prompt_float(const std::string& message) {
    std::string input;
    float value;
    while (true) {
        std::cout << message;
        std::getline(std::cin, input);
        try {
            value = std::stof(input);
            break;
        } catch (const std::invalid_argument&) {
            std::cout << "Invalid input. Please enter a valid number." << std::endl;
        }
    }
    return value;
}
