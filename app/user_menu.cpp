#include "user_menu.h"

// Implements user flows: browse/read, request/return, and account updates.

#include <filesystem>
#include <iostream>
#include <cstdlib>
#include <exception>

#include "../util/prompt.h"
#include "../util/print.h"
#include "menu_helpers.h"
#include "session.h"

// Open a PDF path with the platform's default application/browser.
static bool open_pdf_with_default_app(const std::string& path) {
    if (!std::filesystem::exists(path)) {
        return false;
    }

#ifdef _WIN32
    std::string command = "start \"\" \"" + path + "\"";
#elif __APPLE__
    std::string command = "open \"" + path + "\"";
#else
    std::string command = "xdg-open \"" + path + "\"";
#endif
    return std::system(command.c_str()) == 0;
}

static void read_book(BookManager& manager) {
    if (!manager.has_books()) {
        println("There are no books to read.");
        return;
    }

    show_book_list(manager);
    std::string id = prompt_required("Enter book ID to read: ");
    try {
        manager.get_book_by_id(id);
    } catch (const std::exception&) {
        println("Book not found.");
        return;
    }

    std::string pdfPath = manager.pdf_path_for_id(id);
    std::cout << "PDF path: " << pdfPath << "\n";
    if (!std::filesystem::exists(pdfPath)) {
        println("PDF file not found. Place it at the path above.");
        return;
    }

    if (open_pdf_with_default_app(pdfPath)) {
        println("Opening PDF with your default application...");
    } else {
        println("Could not open the PDF automatically. Open it manually using the path above.");
    }
}

static void request_loan(Session& session, const std::string& username) {
    LoanRequestManager& loanManager = session.loans;
    BookManager& manager = session.books;
    if (!manager.has_books()) {
        println("There are no books to request.");
        return;
    }

    show_book_list(manager);
    std::string id = prompt_required("Enter book ID to request: ");
    try {
        manager.get_book_by_id(id);
    } catch (const std::exception&) {
        println("Book not found.");
        return;
    }
    if (loanManager.user_has_pending_for_book(username, id)) {
        println("You already have a pending request for this book.");
        return;
    }

    if (loanManager.create_request(username, id)) {
        println("Loan request sent to admin.");
        save_loans(session);
    } else {
        println("Could not create request. Try again.");
    }
}

static void view_requests(const LoanRequestManager& loanManager, const BookManager& manager, const std::string& username) {
    std::vector<LoanRequest> requests = loanManager.requests_for_user(username);
    if (requests.empty()) {
        println("No loan requests yet.");
        return;
    }

    println("\n=== Your Loan Requests ===");
    for (size_t i = 0; i < requests.size(); ++i) {
        const LoanRequest& req = requests[i];
        std::string title = "Unknown book";
        try {
            const Book& book = manager.get_book_by_id(req.bookId);
            title = book.title;
        } catch (const std::exception&) {}
        std::cout << "#" << req.id << " [" << LoanRequestManager::status_label(req.status) << "] "
                  << title << " (" << req.bookId << ") on " << LoanRequestManager::human_date(req.requestedAt) << "\n";
    }
    println("==========================\n");
}

static std::string manage_account(Session& session, const std::string& username) {
    AccountManager& accountManager = session.accounts;
    LoanRequestManager& loanManager = session.loans;
    std::string current = username;
    while (true) {
        println("\n=== Manage Account ===");
        println("1. Change username");
        println("2. Change password");
        println("0. Back");

        std::string choice = prompt_required("Choose: ");
        if (choice == "1") {
            std::string newUsername = prompt_required("New username: ");
            bool updated = accountManager.update_username(current, newUsername);
            if (updated) {
                loanManager.rename_user_requests(current, newUsername);
                current = newUsername;
                println("Username updated.");
                save_accounts(session);
                save_loans(session);
            } else {
                println("Could not update username (it might already be used).");
            }
        } else if (choice == "2") {
            std::string newPassword = prompt_required("New password: ");
            if (accountManager.update_password(current, newPassword)) {
                println("Password updated.");
                save_accounts(session);
            } else {
                println("Could not update password.");
            }
        } else if (choice == "0") {
            return current;
        } else {
            println("Invalid choice.");
        }
    }
}

static void return_book(Session& session, const std::string& username) {
    std::vector<LoanRequest> requests = session.loans.requests_for_user(username);
    bool hasActive = false;
    for (std::size_t i = 0; i < requests.size(); ++i) {
        if (requests[i].status == LoanStatus::Approved) {
            hasActive = true;
            break;
        }
    }
    if (!hasActive) {
        println("No approved loans to return.");
        return;
    }

    println("\n=== Return Book ===");
    for (std::size_t i = 0; i < requests.size(); ++i) {
        const LoanRequest& req = requests[i];
        if (req.status == LoanStatus::Approved) {
            std::string title = "Unknown book";
            try {
                const Book& book = session.books.get_book_by_id(req.bookId);
                title = book.title;
            } catch (const std::exception&) {}
            std::cout << "#" << req.id << " " << title << " (" << req.bookId << ")\n";
        }
    }
    std::string idText = prompt_required("Enter request ID to mark as returned: ");
    int id = 0;
    try {
        id = std::stoi(idText);
    } catch (const std::invalid_argument&) {
        println("Invalid ID.");
        return;
    }

    LoanRequest req;
    try {
        req = session.loans.get_request(id);
    } catch (const std::exception&) {
        println("Request not found for your account.");
        return;
    }
    if (req.username != username) {
        println("Request not found for your account.");
        return;
    }
    if (req.status != LoanStatus::Approved) {
        println("Only approved requests can be returned.");
        return;
    }

    session.loans.set_status(id, LoanStatus::Returned);
    save_loans(session);
    println("Book marked as returned.");
}

void run_user_menu(Session& session, const std::string& usernameInput) {
    std::string username = usernameInput;
    while (true) {
        println("\n=== User Menu ===");
        println("1. Browse library");
        println("2. Read a book (open PDF)");
        println("3. Request to loan a book");
        println("4. Check loan request status");
        println("5. Return a book (mark approved request as returned)");
        println("6. Manage account");
        println("0. Logout");

        std::string choice = prompt_required("Choose: ");
        if (choice == "1") {
            show_book_list(session.books);
        } else if (choice == "2") {
            read_book(session.books);
        } else if (choice == "3") {
            request_loan(session, username);
        } else if (choice == "4") {
            view_requests(session.loans, session.books, username);
        } else if (choice == "5") {
            return_book(session, username);
        } else if (choice == "6") {
            username = manage_account(session, username);
        } else if (choice == "0") {
            return;
        } else {
            println("Invalid choice.");
        }
    }
}
