#include <optional>
#include <string>

#include "app/admin_menu.h"
#include "app/auth.h"
#include "app/menu_helpers.h"
#include "app/seed.h"
#include "app/session.h"
#include "app/user_menu.h"
#include "util/prompt.h"
#include "util/print.h"

int main() {
    println(
        "==========================================\n"
        "  Selamat datang ke Perpustakaan Sigma\n"
        "=========================================="
    );

    Session session;
    load_books(session);
    seed_sample_data(session);

    while (true) {
        println("\n=== Main Menu ===");
        println("1. Login");
        println("2. Create account");
        println("0. Exit");

        std::string choice = prompt_required("Choose: ");
        if (choice == "1") {
            std::optional<std::string> loggedIn = login(session.accounts);
            if (!loggedIn.has_value()) {
                continue;
            }

            std::optional<Account> account = session.accounts.get_account(loggedIn.value());
            if (!account.has_value()) {
                println("Account no longer exists.");
                continue;
            }

            if (account.value().role == Role::Admin) {
                run_admin_menu(session);
            } else {
                run_user_menu(session, loggedIn.value());
            }
        } else if (choice == "2") {
            register_account(session.accounts);
        } else if (choice == "0") {
            println("Goodbye!");
            break;
        } else {
            println("Invalid choice.");
        }
    }

    return 0;
}
