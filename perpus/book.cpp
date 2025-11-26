#include <string>
#include <filesystem>
#include <fstream>

#pragma once

struct Book {
    std::string title;
    std::string author;
    int publicationYear;
    std::string isbn;
};

namespace fs = std::filesystem;

bool bookExists(const fs::path& directory, const std::string& isbn) {
    fs::path bookPath = directory / (isbn + ".txt");
    return fs::exists(bookPath);
}

void saveBook(const fs::path& directory, const Book& book) {
    fs::path bookPath = directory / (book.isbn + ".txt");
    std::ofstream outFile(bookPath);
    if (outFile.is_open()) {
        outFile << "Title: " << book.title << std::endl;
        outFile << "Author: " << book.author << std::endl;
        outFile << "Publication Year: " << book.publicationYear << std::endl;
        outFile << "ISBN: " << book.isbn << std::endl;
        outFile.close();
    }
}

void loadBook(const fs::path& directory, const std::string& isbn, Book& book) {
    fs::path bookPath = directory / (isbn + ".txt");
    std::ifstream inFile(bookPath);
    if (inFile.is_open()) {
        std::getline(inFile, book.title);
        std::getline(inFile, book.author);
        std::string yearLine;
        std::getline(inFile, yearLine);
        book.publicationYear = std::stoi(yearLine.substr(yearLine.find(": ") + 2));
        std::getline(inFile, book.isbn);
        inFile.close();
    }
}