#pragma once

#include <string>
#include <vector>

#include "book.h"
using namespace std;
// Configuration for where to store book details and pdf files.
struct BookStorageConfig {
     string detailsDirectory;
     string pdfDirectory;
};

// Handles in-memory book list plus disk persistence to pipe-separated details/<id>.csv and pdf/<id>.pdf paths.
class BookManager {
public:
    BookManager();
    explicit BookManager(const BookStorageConfig& config);

    const  vector<Book>& list_books() const;
    bool has_books() const;

    bool load_from_disk();
    bool add_book(const Book& book);
    bool update_book_by_id(const  string& id, const Book& updated);
    bool remove_book_by_id(const  string& id);

    const Book& get_book_by_id(const  string& id) const;
    const Book& get_book_by_isbn(const  string& isbn) const;

     string pdf_path_for_id(const  string& id) const;
    bool book_exists_on_disk(const  string& id) const;

private:
    BookStorageConfig config;
     vector<Book> books;

     string generate_id() const;
     string details_path_for_id(const  string& id) const;
     string pdf_path_internal(const  string& id) const;
    void ensure_directories() const;
    bool save_book_to_disk(const Book& book) const;

    size_t find_index_by_id(const  string& id) const;
    size_t find_index_by_isbn(const  string& isbn) const;
};
