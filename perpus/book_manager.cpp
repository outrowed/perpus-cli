
#include "book_manager.h"

#include <filesystem>
#include <fstream>
#include <random>
#include <sstream>

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
                books.push_back(book);
            }
        }
    }
    return true;
}
//buat nambahin buku
bool BookManager::add_book(const Book& bookInput) {
    Book book = bookInput;
    if (book.id.empty()) {
        book.id = generate_id();
    }

    if (find_const_by_id(book.id) != nullptr) {
        return false;
    }

    const Book* existingIsbn = get_book_by_isbn(book.isbn);
    if (existingIsbn != nullptr) {
        return false;
    }

    books.push_back(book);
    return save_book_to_disk(book);
}
//update buku make id
bool BookManager::update_book_by_id(const string& id, const Book& updated) {
    Book* existing = find_mutable_by_id(id);
    if (existing == nullptr) {
        return false;
    }
//
    const Book* sameIsbn = get_book_by_isbn(updated.isbn);
    if (sameIsbn != nullptr && sameIsbn->id != id) {
        return false;
    }
//salin data dari updated ke exisiting
    Book copy = updated;
    copy.id = id;
    *existing = copy;
    return save_book_to_disk(copy);
}
//hapus buku make id
bool BookManager::remove_book_by_id(const string& id) {
    for (vector<Book>::iterator it = books.begin(); it != books.end(); ++it) {
        if (it->id == id) {
            books.erase(it);
             filesystem::path detailsFile = details_path_for_id(id);
            if ( filesystem::exists(detailsFile)) {
                 filesystem::remove(detailsFile);
            }
            return true;
        }
    }
    return false;
}
//nyari buku pake id
const Book* BookManager::get_book_by_id(const string& id) const {
    return find_const_by_id(id);
}
//nyari buku pake isbn
const Book* BookManager::get_book_by_isbn(const string& isbn) const {
    return find_const_by_isbn(isbn);
}
//path detail buku
string BookManager::pdf_path_for_id(const string& id) const {
    return pdf_path_internal(id);
}//cek buku ad apa kaga di disk
bool BookManager::book_exists_on_disk(const string& id) const {
     filesystem::path detailsFile = details_path_for_id(id);
    return  filesystem::exists(detailsFile);
}
//buat id buku
string BookManager::generate_id() const {
     random_device rd;
     mt19937 gen(rd());
     uniform_int_distribution<int> dist(0, 15);
    const char* hex = "0123456789abcdef";
    string value = "book-";
    for (int i = 0; i < 12; ++i) {
        int v = dist(gen);
        value.push_back(hex[v]);
    }
    return value;
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

Book* BookManager::find_mutable_by_id(const string& id) {
    for (vector<Book>::iterator it = books.begin(); it != books.end(); ++it) {
        if (it->id == id) {
            return &(*it);
        }
    }
    return nullptr;
}

const Book* BookManager::find_const_by_id(const string& id) const {
    for (vector<Book>::const_iterator it = books.begin(); it != books.end(); ++it) {
        if (it->id == id) {
            return &(*it);
        }
    }
    return nullptr;
}

const Book* BookManager::find_const_by_isbn(const string& isbn) const {
    for (vector<Book>::const_iterator it = books.begin(); it != books.end(); ++it) {
        if (it->isbn == isbn) {
            return &(*it);
        }
    }
    return nullptr;
}
