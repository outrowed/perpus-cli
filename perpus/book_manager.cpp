
#include "book_manager.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>

using std::string;
using std::vector;

namespace {
// Parse a pipe-separated book record: id|isbn|title|author|publisher|year|stock
    bool parse_book_record(const string& line, Book& out) {
         istringstream iss(line);
        vector<string> parts;
        string part;
        while (getline(iss, part, '|')) {
            parts.push_back(part);
        }

        if (parts.size() < 7) {
            return false;
        }

        out.id = parts[0];
        out.isbn = parts[1];
        out.title = parts[2];
        out.author = parts[3];
        out.publisher = parts[4];

         istringstream yearStream(parts[5]);
        if (!(yearStream >> out.publicationYear)) {
            out.publicationYear = 0;
        }

         istringstream stockStream(parts[6]);
        if (!(stockStream >> out.stock)) {
            out.stock = 0;
        }

        return !out.id.empty();
    }
}
//impleentasi manager buku
BookManager::BookManager() {
    config.detailsDirectory = "details";
    config.pdfDirectory = "ebooks";
}

BookManager::BookManager(const BookStorageConfig& cfg) : config(cfg) {}

const vector<Book>& BookManager::list_books() const {
    return books;
}
//buku ada apa kaga
bool BookManager::has_books() const {
    return !books.empty();
}
//ngambil data buku dari disk
bool BookManager::load_from_disk() {
    books.clear();
    ensure_directories();

     filesystem::path detailsPath(config.detailsDirectory);
    if (! filesystem::exists(detailsPath)) { //folfer ril or fek
        return true;
    }

    for (const auto& entry :  filesystem::directory_iterator(detailsPath)) { 
        if (!entry.is_regular_file()) {
            continue;
        }
        string extension = entry.path().extension().string(); //cek ekstensi filenya
        if (extension != ".csv") {
            continue;
        }
         ifstream input(entry.path());
        if (!input.is_open()) {
            continue;
        }

        string line;
        if (getline(input, line)) {
            Book book;
            if (parse_book_record(line, book)) {
                book.isbn = normalize_isbn(book.isbn);
                book.id = book.isbn;
                try {
                    find_index_by_isbn(book.isbn);
                } catch (const std::exception&) {
                    books.push_back(book);
                }
            }
        }
    }
    return true;
}
//buat nambahin buku
bool BookManager::add_book(const Book& bookInput) {
    Book book = bookInput;
    book.isbn = normalize_isbn(book.isbn);
    book.id = book.isbn;

    try {
        find_index_by_isbn(book.isbn);
        return false;
    } catch (const std::exception&) {}

    books.push_back(book);
    return save_book_to_disk(book);
}
//update buku make id
bool BookManager::update_book_by_id(const string& id, const Book& updated) {
    size_t index = 0;
    string normalizedId = normalize_isbn(id);
    try {
        index = find_index_by_id(normalizedId);
    } catch (const std::exception&) {
        return false;
    }

    string normalizedIsbn = normalize_isbn(updated.isbn);
    try {
        size_t isbnIndex = find_index_by_isbn(normalizedIsbn);
        if (books[isbnIndex].id != id) {
            return false;
        }
    } catch (const std::exception&) {
        // If not found, it's fine to proceed.
    }

    Book copy = updated;
    copy.isbn = normalizedIsbn;
    copy.id = normalizedIsbn;
    books[index] = copy;

     filesystem::path oldDetails = details_path_for_id(normalizedId);
     filesystem::path newDetails = details_path_for_id(copy.id);
    bool saved = save_book_to_disk(copy);
    if (saved && oldDetails != newDetails &&  filesystem::exists(oldDetails)) {
         filesystem::remove(oldDetails);
    }

     filesystem::path oldPdf = pdf_path_internal(normalizedId);
     filesystem::path newPdf = pdf_path_internal(copy.id);
    if (oldPdf != newPdf &&  filesystem::exists(oldPdf)) {
        try {
             filesystem::create_directories(newPdf.parent_path());
             filesystem::rename(oldPdf, newPdf);
        } catch (const std::exception&) {}
    }

    return saved;
}
//hapus buku make id
bool BookManager::remove_book_by_id(const string& id) {
    size_t index = 0;
    string normalizedId = normalize_isbn(id);
    try {
        index = find_index_by_id(normalizedId);
    } catch (const std::exception&) {
        return false;
    }

    books.erase(books.begin() + index);
     filesystem::path detailsFile = details_path_for_id(normalizedId);
    if ( filesystem::exists(detailsFile)) {
         filesystem::remove(detailsFile);
    }
    return true;
}
//nyari buku pake id
const Book& BookManager::get_book_by_id(const string& id) const {
    string normalized = normalize_isbn(id);
    size_t index = find_index_by_id(normalized);
    return books[index];
}
//nyari buku pake isbn
const Book& BookManager::get_book_by_isbn(const string& isbn) const {
    string normalized = normalize_isbn(isbn);
    size_t index = find_index_by_isbn(normalized);
    return books[index];
}
//path detail buku
string BookManager::pdf_path_for_id(const string& id) const {
    string normalized = normalize_isbn(id);
    return pdf_path_internal(normalized);
}//cek buku ad apa kaga di disk
bool BookManager::book_exists_on_disk(const string& id) const {
    string normalized = normalize_isbn(id);
     filesystem::path detailsFile = details_path_for_id(normalized);
    return  filesystem::exists(detailsFile);
}
//naro detail buku
string BookManager::details_path_for_id(const string& id) const {
     filesystem::path base(config.detailsDirectory);
    base /= id + ".csv";
    return base.string();
}
//naro buku pdf
string BookManager::pdf_path_internal(const string& id) const {
     filesystem::path base(config.pdfDirectory);
    base /= id + ".pdf";
    return base.string();
}

string BookManager::normalize_isbn(const string& raw) const {
    if (raw.size() >= 5 && raw.substr(0, 5) == "ISBN-") {
        return raw;
    }
    return "ISBN-" + raw;
}

void BookManager::ensure_directories() const {
     filesystem::create_directories(config.detailsDirectory);
     filesystem::create_directories(config.pdfDirectory);
}

bool BookManager::save_book_to_disk(const Book& book) const {
    ensure_directories();
     filesystem::path path = details_path_for_id(book.id);
     ofstream output(path);
    if (!output.is_open()) {
        return false;
    }

    output << book.id << "|"
           << book.isbn << "|"
           << book.title << "|"
           << book.author << "|"
           << book.publisher << "|"
           << book.publicationYear << "|"
           << book.stock << "\n";
    return true;
}

size_t BookManager::find_index_by_id(const string& id) const {
    for (size_t i = 0; i < books.size(); ++i) {
        if (books[i].id == id) {
            return i;
        }
    }
    throw std::runtime_error("Book not found by id: " + id);
}

size_t BookManager::find_index_by_isbn(const string& isbn) const {
    for (size_t i = 0; i < books.size(); ++i) {
        if (books[i].isbn == isbn) {
            return i;
        }
    }
    throw std::runtime_error("Book not found by isbn: " + isbn);
}
