#include <iostream>

#include "./util/prompt.cpp"
#include "./util/print.cpp"
#include "./perpus/book.cpp"
#include "./perpus/book-manager.cpp"

#define add(x,y) ((x)+(y))

int main() {

    Book myBook {
        .title = prompt("Enter book title: "),
        .author = prompt("Enter book author: "),
        .publicationYear = prompt_int("Enter publication year: "),
        .isbn = prompt("Enter book ISBN: ")
    };

    println("Book Details:");
    println("Title: " + myBook.title);
    println("Author: " + myBook.author);
    println("Publication Year: " + std::to_string(myBook.publicationYear));
    println("ISBN: " + myBook.isbn);

    BookManager bookManager = BookManager::BookManagerParam { .booksDirectory = "./books" };

    return 0;
}