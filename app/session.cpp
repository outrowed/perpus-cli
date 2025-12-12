#include "session.h"

Session::Session() : accounts(), books(), loans(books) {}

bool load_books(Session& session) {
    return session.books.load_from_disk();
}
