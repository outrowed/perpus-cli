#pragma once

#include "../account/account_manager.h"
#include "../perpus/book_manager.h"
#include "../perpus/loan_request_manager.h"

struct Session {
    AccountManager accounts;
    BookManager books;
    LoanRequestManager loans;

    Session();
};

bool load_books(Session& session);
