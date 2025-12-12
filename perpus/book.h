#pragma once

#include <string>

struct Book {
    std::string id;
    std::string isbn;
    std::string title;
    std::string author;
    std::string publisher;
    int publicationYear = 0;
    int stock = 0;
};
