#include <string>
#include <map>

#include "./book.cpp"

#pragma once

class BookManager {
    using ISBN = std::string;

private:
    const std::string booksDirectory;

    std::map<ISBN, Book> books;
    std::map<ISBN, std::string> bookPaths;

public:
    struct BookManagerParam
    {
        std::string booksDirectory = "./books";
    };


    BookManager(BookManagerParam param) : booksDirectory(param.booksDirectory) {
        loadBooksFromFile(booksDirectory);
    }

    void addBook();
    void viewBook();
    void updateBook();
    void deleteBook();
    void listBooks();
    void loadBooksFromFile(const std::string& filename);
};