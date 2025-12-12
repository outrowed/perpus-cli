#include "book_manager.h"

#include <cctype>
#include <filesystem>
#include <fstream>
#include <random>
#include <sstream>

using std::string;
using std::vector;

namespace {
    string read_file_to_string(const string& path) {
        std::ifstream input(path);
        std::ostringstream buffer;
        if (input.is_open()) {
            buffer << input.rdbuf();
        }
        return buffer.str();
    }

    string trim_copy(const string& text) {
        size_t start = 0;
        size_t end = text.length();
        while (start < end && std::isspace(static_cast<unsigned char>(text[start]))) {
            start++;
        }
        while (end > start && std::isspace(static_cast<unsigned char>(text[end - 1]))) {
            end--;
        }
        return text.substr(start, end - start);
    }

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
        while (start < content.size() && std::isspace(static_cast<unsigned char>(content[start]))) {
            start++;
        }
        size_t end = start;
        while (end < content.size() && (std::isdigit(static_cast<unsigned char>(content[end])) || content[end] == '-')) {
            end++;
        }
        string value = content.substr(start, end - start);
        std::istringstream iss(value);
        int result = 0;
        iss >> result;
        return result;
    }
}

BookManager::BookManager() {
    config.detailsDirectory = "details";
    config.pdfDirectory = "pdf";
}

BookManager::BookManager(const BookStorageConfig& cfg) : config(cfg) {}

const vector<Book>& BookManager::list_books() const {
    return books;
}

bool BookManager::has_books() const {
    return !books.empty();
}

bool BookManager::load_from_disk() {
    books.clear();
    ensure_directories();

    std::filesystem::path detailsPath(config.detailsDirectory);
    if (!std::filesystem::exists(detailsPath)) {
        return true;
    }

    for (const auto& entry : std::filesystem::directory_iterator(detailsPath)) {
        if (!entry.is_regular_file()) {
            continue;
        }
        string extension = entry.path().extension().string();
        if (extension != ".json") {
            continue;
        }
        string content = read_file_to_string(entry.path().string());
        content = trim_copy(content);
        if (content.empty()) {
            continue;
        }

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

bool BookManager::update_book_by_id(const string& id, const Book& updated) {
    Book* existing = find_mutable_by_id(id);
    if (existing == nullptr) {
        return false;
    }

    const Book* sameIsbn = get_book_by_isbn(updated.isbn);
    if (sameIsbn != nullptr && sameIsbn->id != id) {
        return false;
    }

    Book copy = updated;
    copy.id = id;
    *existing = copy;
    return save_book_to_disk(copy);
}

bool BookManager::remove_book_by_id(const string& id) {
    for (vector<Book>::iterator it = books.begin(); it != books.end(); ++it) {
        if (it->id == id) {
            books.erase(it);
            std::filesystem::path detailsFile = details_path_for_id(id);
            if (std::filesystem::exists(detailsFile)) {
                std::filesystem::remove(detailsFile);
            }
            return true;
        }
    }
    return false;
}

const Book* BookManager::get_book_by_id(const string& id) const {
    return find_const_by_id(id);
}

const Book* BookManager::get_book_by_isbn(const string& isbn) const {
    return find_const_by_isbn(isbn);
}

string BookManager::pdf_path_for_id(const string& id) const {
    return pdf_path_internal(id);
}

bool BookManager::book_exists_on_disk(const string& id) const {
    std::filesystem::path detailsFile = details_path_for_id(id);
    return std::filesystem::exists(detailsFile);
}

string BookManager::generate_id() const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 15);
    const char* hex = "0123456789abcdef";
    string value = "book-";
    for (int i = 0; i < 12; ++i) {
        int v = dist(gen);
        value.push_back(hex[v]);
    }
    return value;
}

string BookManager::details_path_for_id(const string& id) const {
    std::filesystem::path base(config.detailsDirectory);
    base /= id + ".json";
    return base.string();
}

string BookManager::pdf_path_internal(const string& id) const {
    std::filesystem::path base(config.pdfDirectory);
    base /= id + ".pdf";
    return base.string();
}

void BookManager::ensure_directories() const {
    std::filesystem::create_directories(config.detailsDirectory);
    std::filesystem::create_directories(config.pdfDirectory);
}

bool BookManager::save_book_to_disk(const Book& book) const {
    ensure_directories();
    std::filesystem::path path = details_path_for_id(book.id);
    std::ofstream output(path);
    if (!output.is_open()) {
        return false;
    }

    output << "{\n";
    output << "  \"id\": \"" << book.id << "\",\n";
    output << "  \"isbn\": \"" << book.isbn << "\",\n";
    output << "  \"title\": \"" << book.title << "\",\n";
    output << "  \"author\": \"" << book.author << "\",\n";
    output << "  \"publisher\": \"" << book.publisher << "\",\n";
    output << "  \"publicationYear\": " << book.publicationYear << ",\n";
    output << "  \"stock\": " << book.stock << "\n";
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
