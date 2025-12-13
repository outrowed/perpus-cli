#include "auth.h"

#include <iostream>

#include "../util/prompt.h"
#include "../util/print.h"
#include "menu_helpers.h"

std::string login(AccountManager& accountManager) {
    println("\n=== Login ===");
    std::string username = prompt_required("Username: ");
    std::string password = prompt_required("Password: ");

    Account account = accountManager.authenticate(username, password);
    std::cout << "Welcome, " << account.username << " (" << role_to_string(account.role) << ")\n";
    return account.username;
}

bool register_account(AccountManager& accountManager) {
    println("\n=== Create Account ===");
    std::string username = prompt_required("Choose a username: ");
    if (accountManager.username_exists(username)) {
        println("That username is already in use.");
        return false;
    }

    std::string password = prompt_required("Choose a password: ");
    bool created = accountManager.create_account(username, password, Role::User);
    if (created) {
        println("Account created. You can now log in.");
    } else {
        println("Could not create account.");
    }
    return created;
}
