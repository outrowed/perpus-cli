
#include <string>
#include <filesystem>
#include <fstream>

#pragma once
using namespace std;
struct Book {
     string title;
     string author;
    int publicationYear;
     string isbn;
};

/* namespace fs =  filesystem;

bool bookExists(const fs::path& directory, const  string& isbn) {
    fs::path bookPath = directory / (isbn + ".txt");
    return fs::exists(bookPath);
}

void saveBook(const fs::path& directory, const Book& book) {
    fs::path bookPath = directory / (book.isbn + ".txt");
     ofstream outFile(bookPath);
    if (outFile.is_open()) {
        outFile << "Title: " << book.title <<  endl;
        outFile << "Author: " << book.author <<  endl;
        outFile << "Publication Year: " << book.publicationYear <<  endl;
        outFile << "ISBN: " << book.isbn <<  endl;
        outFile.close();
    }
}

void loadBook(const fs::path& directory, const  string& isbn, Book& book) {
    fs::path bookPath = directory / (isbn + ".txt");
     ifstream inFile(bookPath);
    if (inFile.is_open()) {
         getline(inFile, book.title);
         getline(inFile, book.author);
         string yearLine;
         getline(inFile, yearLine);
        book.publicationYear =  stoi(yearLine.substr(yearLine.find(": ") + 2));
         getline(inFile, book.isbn);
        inFile.close();
    }
} */
