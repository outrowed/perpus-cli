#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

enum class Role {
    Admin,
    User
};

struct Account {
    std::string username;
    std::string passwordHash;
    Role role;
};

enum class LoginResult {
    Success,
    InvalidPassword,
    UserNotFound
};

class AccountManager {
public:
    AccountManager();

    // Accessors and queries
    const std::vector<Account>& list_accounts() const;
    bool username_exists(std::string_view username) const;
    std::optional<Account> get_account(std::string_view username) const;

    // Auth and mutations
    std::pair<LoginResult, Account*> authenticate(std::string_view username, std::string_view password);
    bool create_account(std::string username, std::string password, Role role = Role::User);
    bool update_username(std::string_view currentUsername, std::string newUsername);
    bool update_password(std::string_view username, std::string newPassword);
    bool update_role(std::string_view username, Role newRole);

    // Raw helpers for persistence
    void clear();
    void add_account_raw(const Account& account);

private:
    std::vector<Account> accounts;

    Account* find_mutable(std::string_view username);
    const Account* find_const(std::string_view username) const;
};
