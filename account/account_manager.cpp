#include "account_manager.h"

#include "bcrypt_cpp.h"

using std::string;
using std::string_view;
using std::vector;

AccountManager::AccountManager() {}

const vector<Account>& AccountManager::list_accounts() const {
    return accounts;
}

bool AccountManager::username_exists(string_view username) const {
    return find_const(username) != nullptr;
}

std::optional<Account> AccountManager::get_account(string_view username) const {
    const Account* account = find_const(username);
    if (account != nullptr) {
        return *account;
    }
    return std::nullopt;
}

std::pair<LoginResult, Account*> AccountManager::authenticate(string_view username, string_view password) {
    Account* account = find_mutable(username);
    if (account == nullptr) {
        return std::make_pair(LoginResult::UserNotFound, static_cast<Account*>(nullptr));
    }
    bool passwordOk = bcrypt::validatePassword(string(password), account->passwordHash);
    if (!passwordOk) {
        return std::make_pair(LoginResult::InvalidPassword, static_cast<Account*>(nullptr));
    }
    return std::make_pair(LoginResult::Success, account);
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
    Account* account = find_mutable(currentUsername);
    if (account == nullptr) {
        return false;
    }
    account->username = newUsername;
    return true;
}

bool AccountManager::update_password(string_view username, string newPassword) {
    Account* account = find_mutable(username);
    if (account == nullptr) {
        return false;
    }
    account->passwordHash = bcrypt::generateHash(newPassword);
    return true;
}

bool AccountManager::update_role(string_view username, Role newRole) {
    Account* account = find_mutable(username);
    if (account == nullptr) {
        return false;
    }
    account->role = newRole;
    return true;
}

Account* AccountManager::find_mutable(string_view username) {
    for (vector<Account>::iterator it = accounts.begin(); it != accounts.end(); ++it) {
        if (it->username == username) {
            return &(*it);
        }
    }
    return nullptr;
}

const Account* AccountManager::find_const(string_view username) const {
    for (vector<Account>::const_iterator it = accounts.begin(); it != accounts.end(); ++it) {
        if (it->username == username) {
            return &(*it);
        }
    }
    return nullptr;
}
