#include "seed.h"

#include "menu_helpers.h"

void seed_sample_data(Session& session) {
    if (!session.accounts.username_exists("admin")) {
        session.accounts.create_account("admin", "admin", Role::Admin);
    }
    if (!session.accounts.username_exists("demo")) {
        session.accounts.create_account("demo", "password", Role::User);
    }

    if (session.books.has_books()) {
        return;
    }

    Book sample1;
    sample1.id = "sample-001";
    sample1.isbn = "ISBN-001";
    sample1.title = "Pemrograman C++ Dasar";
    sample1.author = "Tim Sigma";
    sample1.publisher = "Sigma Press";
    sample1.publicationYear = 2022;
    sample1.stock = 3;

    Book sample2;
    sample2.id = "sample-002";
    sample2.isbn = "ISBN-002";
    sample2.title = "Algoritma Praktis";
    sample2.author = "Tim Sigma";
    sample2.publisher = "Sigma Press";
    sample2.publicationYear = 2021;
    sample2.stock = 2;

    session.books.add_book(sample1);
    session.books.add_book(sample2);

    try {
        const Book& added1 = session.books.get_book_by_isbn(sample1.isbn);
        ensure_pdf_placeholder(session.books, added1);
    } catch (const std::exception&) {}
    try {
        const Book& added2 = session.books.get_book_by_isbn(sample2.isbn);
        ensure_pdf_placeholder(session.books, added2);
    } catch (const std::exception&) {}
}
