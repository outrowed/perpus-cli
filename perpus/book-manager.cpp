//Ini Library
#include <string>
#include <map>
//Ini skrip buku
#include "./book.cpp"

#pragma once

class BookManager {
    using ISBN = std::string;

private:
    const std::string booksDirectory;

    std::map<ISBN, Book> books;
    std::map<ISBN, std::string> bookPaths;

public:
// Tambahkan di bagian public class BookManager:

    // Cek apakah buku ada dan stok > 0
    bool isAvailable(const std::string& isbn) {
        if (books.find(isbn) != books.end()) {
            return books[isbn].stock > 0;
        }
        return false;
    }

    // Kurangi stok (saat dipinjam)
    void decreaseStock(const std::string& isbn) {
        if (books.find(isbn) != books.end() && books[isbn].stock > 0) {
            books[isbn].stock--;
            // saveBook(...); // Jangan lupa simpan perubahan ke file
        }
    }

    // Tambah stok (saat dikembalikan)
    void increaseStock(const std::string& isbn) {
        if (books.find(isbn) != books.end()) {
            books[isbn].stock++;
            // saveBook(...); // Jangan lupa simpan perubahan ke file
        }
    }
    
    // Ambil judul buku untuk display
    std::string getTitle(const std::string& isbn) {
        if (books.find(isbn) != books.end()) {
            return books[isbn].title;
        }
        return "Unknown Book";
    }

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
