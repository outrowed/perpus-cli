#include "session.h"

#include <filesystem>
#include <fstream>
#include <sstream>

Session::Session() : accounts(), books(), loans(books) {
    accountsFile = "data/accounts.txt";
    loansFile = "data/loans.txt";
}

bool load_books(Session& session) {
    return session.books.load_from_disk();
}

bool load_accounts(Session& session) {
    std::filesystem::path path(session.accountsFile);
    if (!std::filesystem::exists(path)) {
        return true;
    }

    std::ifstream input(path);
    if (!input.is_open()) {
        return false;
    }

    std::vector<Account> loaded;
    std::string line;
    while (std::getline(input, line)) {
        std::istringstream iss(line);
        std::string username;
        std::string hash;
        std::string roleText;
        if (std::getline(iss, username, '|') && std::getline(iss, hash, '|') && std::getline(iss, roleText, '|')) {
            Account account;
            account.username = username;
            account.passwordHash = hash;
            if (roleText == "Admin") {
                account.role = Role::Admin;
            } else {
                account.role = Role::User;
            }
            loaded.push_back(account);
        }
    }

    session.accounts.clear();
    for (std::size_t i = 0; i < loaded.size(); ++i) {
        session.accounts.add_account_raw(loaded[i]);
    }
    return true;
}

bool save_accounts(const Session& session) {
    std::filesystem::create_directories(std::filesystem::path(session.accountsFile).parent_path());
    std::ofstream output(session.accountsFile);
    if (!output.is_open()) {
        return false;
    }

    const std::vector<Account>& accounts = session.accounts.list_accounts();
    for (std::size_t i = 0; i < accounts.size(); ++i) {
        const Account& account = accounts[i];
        std::string roleText = account.role == Role::Admin ? "Admin" : "User";
        output << account.username << "|" << account.passwordHash << "|" << roleText << "\n";
    }
    return true;
}

bool load_loans(Session& session) {
    auto ensure_isbn_prefix = [](const std::string& value) {
        if (value.size() >= 5 && value.rfind("ISBN-", 0) == 0) {
            return value;
        }
        return std::string("ISBN-") + value;
    };

    std::filesystem::path path(session.loansFile);
    if (!std::filesystem::exists(path)) {
        return true;
    }

    std::ifstream input(path);
    if (!input.is_open()) {
        return false;
    }

    session.loans.clear();
    std::string line;
    int maxId = 0;
    while (std::getline(input, line)) {
        std::istringstream iss(line);
        std::string idText;
        std::string username;
        std::string bookId;
        std::string timeText;
        std::string statusText;
        if (std::getline(iss, idText, '|') &&
            std::getline(iss, username, '|') &&
            std::getline(iss, bookId, '|') &&
            std::getline(iss, timeText, '|') &&
            std::getline(iss, statusText, '|')) {
            LoanRequest req;
            req.id = std::stoi(idText);
            req.username = username;
            req.bookId = ensure_isbn_prefix(bookId);
            long long storedSeconds = std::stoll(timeText);
            req.requestedAt = storedSeconds;
            if (statusText == "Approved") {
                req.status = LoanStatus::Approved;
            } else if (statusText == "Rejected") {
                req.status = LoanStatus::Rejected;
            } else if (statusText == "Returned") {
                req.status = LoanStatus::Returned;
            } else {
                req.status = LoanStatus::Pending;
            }
            if (req.id > maxId) {
                maxId = req.id;
            }
            session.loans.add_request_raw(req);
        }
    }
    session.loans.set_next_id(maxId + 1);
    return true;
}

bool save_loans(const Session& session) {
    std::filesystem::create_directories(std::filesystem::path(session.loansFile).parent_path());
    std::ofstream output(session.loansFile);
    if (!output.is_open()) {
        return false;
    }

    const std::vector<LoanRequest>& requests = session.loans.list_requests();
    for (std::size_t i = 0; i < requests.size(); ++i) {
        const LoanRequest& req = requests[i];
        std::string statusText = LoanRequestManager::status_label(req.status);
        long long requestedSeconds = req.requestedAt;
        output << req.id << "|" << req.username << "|" << req.bookId << "|"
               << requestedSeconds << "|" << statusText << "\n";
    }
    return true;
}
