#include "admin_menu.h"

// Implements admin flows: book management, request approvals, and account maintenance.

#include <iostream>
#include <optional>

#include "../util/prompt.h"
#include "../util/print.h"
#include "auth.h"
#include "session.h"
#include "menu_helpers.h"

static void admin_add_book(BookManager& manager) {
    println("\n=== Add Book ===");
    Book book;
    book.isbn = prompt_required("ISBN: ");
    const Book* existingIsbn = manager.get_book_by_isbn(book.isbn);
    if (existingIsbn != nullptr) {
        println("A book with that ISBN already exists.");
        return;
    }

    book.title = prompt_required("Title: ");
    book.author = prompt_required("Author: ");
    book.publisher = prompt_required("Publisher: ");
    book.publicationYear = prompt_int("Publication year: ");
    book.stock = prompt_int("Stock count: ");

    bool added = manager.add_book(book);
    if (added) {
        const Book* stored = manager.get_book_by_isbn(book.isbn);
        if (stored != nullptr) {
            ensure_pdf_placeholder(manager, *stored);
            std::cout << "Book added with ID: " << stored->id << "\n";
            std::cout << "Place the PDF at: " << manager.pdf_path_for_id(stored->id) << "\n";
        }
    } else {
        println("Failed to add book.");
    }
}

static void admin_edit_book(BookManager& manager) {
    if (!manager.has_books()) {
        println("No books to edit.");
        return;
    }

    show_book_list(manager);
    std::string id = prompt_required("Enter book ID to edit: ");
    const Book* existing = manager.get_book_by_id(id);
    if (existing == nullptr) {
        println("Book not found.");
        return;
    }

    Book updated = *existing;
    std::string newIsbn = prompt("New ISBN (leave blank to keep current): ");
    if (!newIsbn.empty() && newIsbn != existing->isbn) {
        const Book* isbnCheck = manager.get_book_by_isbn(newIsbn);
        if (isbnCheck != nullptr && isbnCheck->id != id) {
            println("Another book already uses that ISBN.");
            return;
        }
        updated.isbn = newIsbn;
    }

    std::string newTitle = prompt("New title (leave blank to keep current): ");
    if (!newTitle.empty()) {
        updated.title = newTitle;
    }
    std::string newAuthor = prompt("New author (leave blank to keep current): ");
    if (!newAuthor.empty()) {
        updated.author = newAuthor;
    }
    std::string newPublisher = prompt("New publisher (leave blank to keep current): ");
    if (!newPublisher.empty()) {
        updated.publisher = newPublisher;
    }
    updated.publicationYear = prompt_int_with_default("New publication year (leave blank to keep current): ", updated.publicationYear);
    updated.stock = prompt_int_with_default("New stock (leave blank to keep current): ", updated.stock);

    bool updatedOk = manager.update_book_by_id(id, updated);
    if (updatedOk) {
        println("Book updated.");
    } else {
        println("Failed to update book.");
    }
}

static void admin_book_menu(BookManager& manager) {
    while (true) {
        println("\n=== Book Management ===");
        println("1. List books");
        println("2. Add book");
        println("3. Edit book");
        println("0. Back");

        std::string choice = prompt_required("Choose: ");
        if (choice == "1") {
            show_book_list(manager);
        } else if (choice == "2") {
            admin_add_book(manager);
        } else if (choice == "3") {
            admin_edit_book(manager);
        } else if (choice == "0") {
            return;
        } else {
            println("Invalid choice.");
        }
    }
}

static void list_accounts(const AccountManager& accountManager) {
    println("\n=== Accounts ===");
    const std::vector<Account>& accounts = accountManager.list_accounts();
    for (size_t i = 0; i < accounts.size(); ++i) {
        std::cout << "- " << accounts[i].username << " [" << role_to_string(accounts[i].role) << "]\n";
    }
}

static void admin_edit_account(Session& session) {
    AccountManager& accountManager = session.accounts;
    LoanRequestManager& loanManager = session.loans;
    if (accountManager.list_accounts().empty()) {
        println("No accounts found.");
        return;
    }

    list_accounts(accountManager);
    std::string username = prompt_required("Username to edit: ");
    if (!accountManager.get_account(username).has_value()) {
        println("User not found.");
        return;
    }

    std::string current = username;
    while (true) {
        println("\nEditing account: " + current);
        println("1. Change username");
        println("2. Reset password");
        println("3. Change role (admin/user)");
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
                println("Could not update username (maybe already used).");
            }
        } else if (choice == "2") {
            std::string newPassword = prompt_required("New password: ");
            if (accountManager.update_password(current, newPassword)) {
                println("Password reset.");
                save_accounts(session);
            } else {
                println("Could not update password.");
            }
        } else if (choice == "3") {
            std::string roleText = prompt_required("Role (admin/user): ");
            std::optional<Role> parsed = parse_role(roleText);
            if (!parsed.has_value()) {
                println("Please choose between admin or user.");
            } else if (accountManager.update_role(current, parsed.value())) {
                println("Role updated.");
                save_accounts(session);
            } else {
                println("Failed to update role.");
            }
        } else if (choice == "0") {
            return;
        } else {
            println("Invalid choice.");
        }
    }
}

static void admin_manage_accounts(Session& session) {
    AccountManager& accountManager = session.accounts;
    LoanRequestManager& loanManager = session.loans;
    while (true) {
        println("\n=== Account Management ===");
        println("1. List accounts");
        println("2. Edit an account");
        println("3. Create new account");
        println("0. Back");

        std::string choice = prompt_required("Choose: ");
        if (choice == "1") {
            list_accounts(accountManager);
        } else if (choice == "2") {
            admin_edit_account(session);
        } else if (choice == "3") {
            bool created = register_account(accountManager);
            if (created) {
                save_accounts(session);
            }
        } else if (choice == "0") {
            return;
        } else {
            println("Invalid choice.");
        }
    }
}

static void admin_review_requests(Session& session) {
    LoanRequestManager& loanManager = session.loans;
    BookManager& manager = session.books;
    if (loanManager.list_requests().empty()) {
        println("No loan requests yet.");
        return;
    }

    println("\n=== Loan Requests ===");
    const std::vector<LoanRequest>& requests = loanManager.list_requests();
    for (size_t i = 0; i < requests.size(); ++i) {
        print_request_card(requests[i], manager);
    }

    std::string idText = prompt("Enter request ID to approve/reject (leave blank to go back): ");
    if (idText.empty()) {
        return;
    }

    int id = 0;
    try {
        id = std::stoi(idText);
    } catch (const std::invalid_argument&) {
        println("Invalid request ID.");
        return;
    }

    std::optional<LoanRequest> req = loanManager.find_by_id(id);
    if (!req.has_value()) {
        println("Request not found.");
        return;
    }

    println("1. Approve");
    println("2. Reject");
    println("0. Back");
    std::string choice = prompt_required("Choose: ");
    if (choice == "1") {
        loanManager.set_status(id, LoanStatus::Approved);
        println("Request approved.");
        save_loans(session);
    } else if (choice == "2") {
        loanManager.set_status(id, LoanStatus::Rejected);
        println("Request rejected.");
        save_loans(session);
    } else if (choice == "0") {
        return;
    } else {
        println("Invalid choice.");
    }
}

void run_admin_menu(Session& session) {
    while (true) {
        println("\n=== Admin Menu ===");
        println("1. Manage books");
        println("2. Review loan requests");
        println("3. Manage accounts");
        println("0. Logout");

        std::string choice = prompt_required("Choose: ");
        if (choice == "1") {
            admin_book_menu(session.books);
        } else if (choice == "2") {
            admin_review_requests(session);
        } else if (choice == "3") {
            admin_manage_accounts(session);
        } else if (choice == "0") {
            return;
        } else {
            println("Invalid choice.");
        }
    }
}
