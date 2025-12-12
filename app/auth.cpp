#include "auth.h"

#include <iostream>

#include "../util/prompt.h"
#include "../util/print.h"
#include "menu_helpers.h"

std::optional<std::string> login(AccountManager& accountManager) {
    println("\n=== Login ===");
    std::string username = prompt_required("Username: ");
    std::string password = prompt_required("Password: ");

    std::pair<LoginResult, Account*> result = accountManager.authenticate(username, password);
    if (result.first == LoginResult::Success && result.second != nullptr) {
        std::cout << "Welcome, " << result.second->username << " (" << role_to_string(result.second->role) << ")\n";
        return result.second->username;
    }
    if (result.first == LoginResult::InvalidPassword) {
        println("Invalid password.");
    } else {
        println("User not found.");
    }
    return std::nullopt;
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
