#include "bcrypt_cpp.h"

#include <string>
#include <vector>
#include <ranges>

#include "./util/print.cpp"

#pragma once

struct Account {
    std::string username;
    std::string password; // hashed
};

enum class LoginResult {
    SUCCESS,
    INVALID_PASSWORD,
    USER_NOT_FOUND
};

class AccountManager {
    std::vector<Account> accounts;

public:
    AccountManager() = default;

    auto get_usernames() const {
        return accounts | std::views::transform([]  (const Account& account) {
            return std::string_view(account.username);
        });
    }

    bool username_exists(std::string_view username) {
        auto view = get_usernames();
        return std::ranges::find(view, username) != view.end();
    }

    LoginResult login(std::string_view username, std::string_view password) {
        for (const auto& account : accounts) {
            if (account.username == username) {
                if (bcrypt::validatePassword(std::string(password), account.password)) {
                    return LoginResult::SUCCESS;
                } else {
                    return LoginResult::INVALID_PASSWORD;
                }
            }
        }
        return LoginResult::USER_NOT_FOUND;
    }

    void create_account(std::string username, std::string password) {
        accounts.push_back({
            username,
            bcrypt::generateHash(password)
        });
    }
};
