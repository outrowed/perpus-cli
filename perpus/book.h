#pragma once

#include <string>
using namespace std;

struct Book {
    string id;
    string isbn;
    string title;
    string author;
    string publisher;
    int publicationYear = 0;
    int stock = 0;
};
