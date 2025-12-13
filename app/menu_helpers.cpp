#include "menu_helpers.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

std::string role_to_string(Role role) {
    if (role == Role::Admin) {
        return "Admin";
    }
    return "User";
}

Role parse_role(const std::string& raw) {
    auto to_lower_ascii = [](char c) {
        if (c >= 'A' && c <= 'Z') {
            char lower = 'a' + (c - 'A');
            return lower;
        }
        return c;
    };

    std::string lower;
    lower.reserve(raw.size());
    for (size_t i = 0; i < raw.size(); ++i) {
        lower.push_back(to_lower_ascii(raw[i]));
    }
    if (lower == "admin") {
        return Role::Admin;
    }
    if (lower == "user") {
        return Role::User;
    }
    throw std::runtime_error("Please choose between admin or user.");
}

int prompt_int_with_default(const std::string& message, int currentValue) {
    while (true) {
        std::string input = prompt(message);
        if (input.empty()) {
            return currentValue;
        }
        try {
            return std::stoi(input);
        } catch (const std::invalid_argument&) {
            println("Please enter a valid number.");
        }
    }
}

void show_book_list(const BookManager& manager) {
    const std::vector<Book>& list = manager.list_books();
    if (list.empty()) {
        println("No books available yet.");
        return;
    }

    println("\n=== Library ===");
    for (size_t i = 0; i < list.size(); ++i) {
        const Book& book = list[i];
        std::cout << "- [" << book.id << "] " << book.title << " by " << book.author << "\n";
        std::cout << "  ISBN: " << book.isbn << " | Publisher: " << book.publisher
                  << " | Year: " << book.publicationYear << " | Stock: " << book.stock << "\n";
        std::cout << "  PDF path: " << manager.pdf_path_for_id(book.id) << "\n";
    }
    println("================\n");
}

void print_request_card(const LoanRequest& request, const BookManager& manager) {
    std::string title = "Unknown book";
    try {
        const Book& book = manager.get_book_by_id(request.bookId);
        title = book.title;
    } catch (const std::exception&) {}

    std::cout << "#" << request.id << " [" << LoanRequestManager::status_label(request.status) << "] "
              << title << " (" << request.bookId << ") requested by " << request.username
              << " on " << LoanRequestManager::human_date(request.requestedAt) << "\n";
}

void ensure_pdf_placeholder(const BookManager& manager, const Book& book) {
    std::string pdfPath = manager.pdf_path_for_id(book.id);
    std::filesystem::path path(pdfPath);
    if (std::filesystem::exists(path)) {
        return;
    }
    std::filesystem::create_directories(path.parent_path());
    std::ofstream output(path);
    if (output.is_open()) {
        output << book.title << " (demo PDF placeholder)\n";
    }
}
