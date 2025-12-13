
#include "book_manager.h"

#include <cctype>
#include <filesystem>
#include <fstream>
#include <random>
#include <sstream>

using std::string;
using std::vector;

namespace {
    bool is_space_char(char c) {
        unsigned char uc = c;
        return  isspace(uc) != 0;
    }

    bool is_digit_char(char c) {
        unsigned char uc = c;
        return  isdigit(uc) != 0;
    }
//baca file jd string
    string read_file_to_string(const string& path) {
         ifstream input(path);
         ostringstream buffer;
        if (input.is_open()) {//baca isi file
            buffer << input.rdbuf();
        }
        return buffer.str();
    }
//ngilangin spais
    string trim_copy(const string& text) {
        size_t start = 0;
        size_t end = text.length();
        while (start < end && is_space_char(text[start])) {
            start++;
        }
        while (end > start && is_space_char(text[end - 1])) {
            end--;
        }
        return text.substr(start, end - start);
    }
//ngambil string dari json
    string extract_json_value(const string& content, const string& key) {
        string needle = "\"" + key + "\"";
        size_t pos = content.find(needle);
        if (pos == string::npos) {
            return "";
        }
        size_t colon = content.find(':', pos);
        if (colon == string::npos) {
            return "";
        }
        size_t firstQuote = content.find('"', colon + 1);
        if (firstQuote == string::npos) {
            return "";
        }
        size_t secondQuote = content.find('"', firstQuote + 1);
        if (secondQuote == string::npos || secondQuote <= firstQuote + 1) {
            return "";
        }
        string value = content.substr(firstQuote + 1, secondQuote - firstQuote - 1);
        return value;
    }
//ngambil int dari json
    int extract_json_int(const string& content, const string& key) {
        string needle = "\"" + key + "\"";
        size_t pos = content.find(needle);
        if (pos == string::npos) {
            return 0;
        }
        size_t colon = content.find(':', pos);
        if (colon == string::npos) {
            return 0;
        }
        size_t start = colon + 1;
        while (start < content.size() && is_space_char(content[start])) {
            start++;
        }
        size_t end = start;
        while (end < content.size() && (is_digit_char(content[end]) || content[end] == '-')) {
            end++;
        }
        string value = content.substr(start, end - start);
         istringstream iss(value);
        int result = 0;
        iss >> result;
        return result;
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
        if (extension != ".json") {
            continue;
        }
        string content = read_file_to_string(entry.path().string()); //baca file
        content = trim_copy(content);
        if (content.empty()) {
            continue;
        }
//Data buku
        Book book;
        book.id = extract_json_value(content, "id");
        book.isbn = extract_json_value(content, "isbn");
        book.title = extract_json_value(content, "title");
        book.author = extract_json_value(content, "author");
        book.publisher = extract_json_value(content, "publisher");
        book.publicationYear = extract_json_int(content, "publicationYear");
        book.stock = extract_json_int(content, "stock");

        if (!book.id.empty()) {
            books.push_back(book);
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
    base /= id + ".json";
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

    output << "{\n";
    output << "  \"ID\": \"" << book.id << "\",\n";
    output << "  \"ISBN\": \"" << book.isbn << "\",\n";
    output << "  \"Judul\": \"" << book.title << "\",\n";
    output << "  \"Pengarang\": \"" << book.author << "\",\n";
    output << "  \"Penerbit\": \"" << book.publisher << "\",\n";
    output << "  \"TahunTerbit\": " << book.publicationYear << ",\n";
    output << "  \"Jumlah\": " << book.stock << "\n";
    output << "}\n";
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
