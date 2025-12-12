#pragma once

#include "../account/account_manager.h"
#include "../perpus/book_manager.h"
#include "../perpus/loan_request_manager.h"

struct Session {
    AccountManager accounts;
    BookManager books;
    LoanRequestManager loans;
    std::string accountsFile;
    std::string loansFile;

    Session();
};

bool load_books(Session& session);
bool load_accounts(Session& session);
bool save_accounts(const Session& session);
bool load_loans(Session& session);
bool save_loans(const Session& session);
