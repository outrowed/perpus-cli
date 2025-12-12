#pragma once

#include <string>
#include <vector>

#include "book.h"

struct BookStorageConfig {
    std::string detailsDirectory;
    std::string pdfDirectory;
};

class BookManager {
public:
    BookManager();
    explicit BookManager(const BookStorageConfig& config);

    const std::vector<Book>& list_books() const;
    bool has_books() const;

    bool load_from_disk();
    bool add_book(const Book& book);
    bool update_book_by_id(const std::string& id, const Book& updated);
    bool remove_book_by_id(const std::string& id);

    const Book* get_book_by_id(const std::string& id) const;
    const Book* get_book_by_isbn(const std::string& isbn) const;

    std::string pdf_path_for_id(const std::string& id) const;
    bool book_exists_on_disk(const std::string& id) const;

private:
    BookStorageConfig config;
    std::vector<Book> books;

    std::string generate_id() const;
    std::string details_path_for_id(const std::string& id) const;
    std::string pdf_path_internal(const std::string& id) const;
    void ensure_directories() const;
    bool save_book_to_disk(const Book& book) const;

    Book* find_mutable_by_id(const std::string& id);
    const Book* find_const_by_id(const std::string& id) const;
    const Book* find_const_by_isbn(const std::string& isbn) const;
};
