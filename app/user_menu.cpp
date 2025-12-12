#include "user_menu.h"

#include <filesystem>
#include <iostream>

#include "../util/prompt.h"
#include "../util/print.h"
#include "menu_helpers.h"

static void read_book(BookManager& manager) {
    if (!manager.has_books()) {
        println("There are no books to read.");
        return;
    }

    show_book_list(manager);
    std::string id = prompt_required("Enter book ID to read: ");
    const Book* book = manager.get_book_by_id(id);
    if (book == nullptr) {
        println("Book not found.");
        return;
    }

    std::string pdfPath = manager.pdf_path_for_id(id);
    std::cout << "PDF path: " << pdfPath << "\n";
    if (std::filesystem::exists(pdfPath)) {
        println("Open the file in your preferred PDF reader.");
    } else {
        println("PDF file not found. Place it at the path above.");
    }
}

static void request_loan(LoanRequestManager& loanManager, BookManager& manager, const std::string& username) {
    if (!manager.has_books()) {
        println("There are no books to request.");
        return;
    }

    show_book_list(manager);
    std::string id = prompt_required("Enter book ID to request: ");
    const Book* book = manager.get_book_by_id(id);
    if (book == nullptr) {
        println("Book not found.");
        return;
    }
    if (loanManager.user_has_pending_for_book(username, id)) {
        println("You already have a pending request for this book.");
        return;
    }

    if (loanManager.create_request(username, id)) {
        println("Loan request sent to admin.");
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
        const Book* book = manager.get_book_by_id(req.bookId);
        std::string title = "Unknown book";
        if (book != nullptr) {
            title = book->title;
        }
        std::cout << "#" << req.id << " [" << LoanRequestManager::status_label(req.status) << "] "
                  << title << " (" << req.bookId << ") on " << LoanRequestManager::human_date(req.requestedAt) << "\n";
    }
    println("==========================\n");
}

static std::string manage_account(AccountManager& accountManager, LoanRequestManager& loanManager, const std::string& username) {
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
            } else {
                println("Could not update username (it might already be used).");
            }
        } else if (choice == "2") {
            std::string newPassword = prompt_required("New password: ");
            if (accountManager.update_password(current, newPassword)) {
                println("Password updated.");
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

void run_user_menu(Session& session, const std::string& usernameInput) {
    std::string username = usernameInput;
    while (true) {
        println("\n=== User Menu ===");
        println("1. Browse library");
        println("2. Read a book (view PDF path)");
        println("3. Request to loan a book");
        println("4. Check loan request status");
        println("5. Manage account");
        println("0. Logout");

        std::string choice = prompt_required("Choose: ");
        if (choice == "1") {
            show_book_list(session.books);
        } else if (choice == "2") {
            read_book(session.books);
        } else if (choice == "3") {
            request_loan(session.loans, session.books, username);
        } else if (choice == "4") {
            view_requests(session.loans, session.books, username);
        } else if (choice == "5") {
            username = manage_account(session.accounts, session.loans, username);
        } else if (choice == "0") {
            return;
        } else {
            println("Invalid choice.");
        }
    }
}
