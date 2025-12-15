#include <string>
#include <exception>

#include "app/admin_menu.h"
#include "app/auth.h"
#include "app/menu_helpers.h"
#include "app/seed.h"
#include "app/session.h"
#include "app/user_menu.h"
#include "util/prompt.h"
#include "util/print.h"
using namespace std;
int main() {
    println(
        "==========================================\n"
        "  Selamat datang ke Perpustakaan Sigma\n"
        "=========================================="
    );

    Session session;
    load_accounts(session);
    load_loans(session);
    load_books(session);
    seed_sample_data(session);
    save_accounts(session);
    save_loans(session);

    while (true) {
        println("\n=== Main Menu ===");
        println("1. Login");
        println("2. Create account");
        println("0. Exit");
        printIn("\n=================");

        string choice = prompt_required("Choose: ");
        if (choice == "1") {
            try {
                string loggedIn = login(session.accounts);
                Account account = session.accounts.get_account(loggedIn);
                if (account.role == Role::Admin) {
                    run_admin_menu(session);
                } else {
                    run_user_menu(session, loggedIn);
                }
            } catch (const std::exception& ex) {
                println(ex.what());
            }
        } else if (choice == "2") {
            bool created = register_account(session.accounts);
            if (created) {
                save_accounts(session);
            }
        } else if (choice == "0") {
            println("Goodbye!");
            break;
        } else {
            println("Invalid choice.");
        }
    }

    return 0;
}
