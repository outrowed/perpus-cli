#include <iostream>
#include <string>

#pragma once

std::string prompt(const std::string message) {
    std::string input;
    std::cout << message;
    std::getline(std::cin, input);
    return input;
}

int prompt_int(const std::string message) {
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

float prompt_float(const std::string message) {
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