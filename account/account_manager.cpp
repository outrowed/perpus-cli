#include "account_manager.h"

#include "bcrypt_cpp.h"
#include <stdexcept>

using std::string;
using std::string_view;
using std::vector;

AccountManager::AccountManager() {}

const vector<Account>& AccountManager::list_accounts() const {
    return accounts;
}

bool AccountManager::username_exists(string_view username) const {
    for (vector<Account>::const_iterator it = accounts.begin(); it != accounts.end(); ++it) {
        if (it->username == username) {
            return true;
        }
    }
    return false;
}

Account AccountManager::get_account(string_view username) const {
    size_t index = find_index(username);
    return accounts[index];
}

Account AccountManager::authenticate(string_view username, string_view password) {
    size_t index = find_index(username);
    Account& account = accounts[index];
    bool passwordOk = bcrypt::validatePassword(string(password), account.passwordHash);
    if (!passwordOk) {
        throw std::runtime_error("Invalid password.");
    }
    return account;
}

bool AccountManager::create_account(string username, string password, Role role) {
    if (username_exists(username)) {
        return false;
    }
    Account account;
    account.username = username;
    account.passwordHash = bcrypt::generateHash(password);
    account.role = role;
    accounts.push_back(account);
    return true;
}

bool AccountManager::update_username(string_view currentUsername, string newUsername) {
    if (username_exists(newUsername)) {
        return false;
    }
    size_t index = 0;
    try {
        index = find_index(currentUsername);
    } catch (const std::exception&) {
        return false;
    }

    accounts[index].username = newUsername;
    return true;
}

bool AccountManager::update_password(string_view username, string newPassword) {
    size_t index = 0;
    try {
        index = find_index(username);
    } catch (const std::exception&) {
        return false;
    }

    accounts[index].passwordHash = bcrypt::generateHash(newPassword);
    return true;
}

bool AccountManager::update_role(string_view username, Role newRole) {
    size_t index = 0;
    try {
        index = find_index(username);
    } catch (const std::exception&) {
        return false;
    }

    accounts[index].role = newRole;
    return true;
}

void AccountManager::clear() {
    accounts.clear();
}

void AccountManager::add_account_raw(const Account& account) {
    accounts.push_back(account);
}

size_t AccountManager::find_index(string_view username) const {
    for (size_t i = 0; i < accounts.size(); ++i) {
        if (accounts[i].username == username) {
            return i;
        }
    }
    throw std::runtime_error("Account not found: " + string(username));
}
