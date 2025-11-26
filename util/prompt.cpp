#include <iostream>
#include <string>
#include <concepts>
#include <format>

#include "./print.cpp"

#pragma once

std::string_view prompt(std::string_view message) {
    std::string input;
    std::cout << message;
    std::getline(std::cin, input);
    return input;
}

std::string_view prompt_if(
    std::string_view message,
    std::predicate<const std::string_view&> auto check,
    std::string_view false_message = ""
) {
    while (true) {
        auto answer = prompt(message);

        if (check(answer)) return answer;
        else println(std::format(std::string(false_message), answer));
    }
}

std::string_view prompt_required(std::string_view message) {
    while (true) {
        auto answer = prompt(message);

        if (answer != "") return answer;

        println(std::format("{} is an empty string!", answer));
    }
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