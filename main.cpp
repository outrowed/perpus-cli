#include <iostream>
#include <ranges>
#include <string>
#include <format>

#include "./util/prompt.cpp"
#include "./util/print.cpp"
#include "./perpus/book.cpp"
#include "./perpus/book-manager.cpp"

#include "./account/account-manager.cpp"

int main() {
    println(
        "==========================================\n"
        "  Selamat datang ke Perpustakaan Sigma\n"
        "==========================================s\n"
    );

    AccountManager accountManager {};

    println("Silahkan masuk akun terlebih dahulu.");

    for (auto username : accountManager.get_usernames()) {
        std::cout << "* " << username << std::endl;
    }

    println("For admin account, type: \"admin\"");

    std::string_view login_username;

    while (true) {
        auto answer = prompt_required("Type account name: ");

        if (accountManager.username_exists(answer)) {
            login_username = answer;
        }

        println(std::format("Account with \"{}\" username does not exist!"));
    }

    auto login_password = prompt_required("Enter password: ");

    auto login_result = accountManager.login(login_username, login_password);

    if (login_result == LoginResult::SUCCESS) {
        std::cout << "You have successfully log into " << login_username << std::endl;
    }
    else if (login_result == LoginResult::INVALID_PASSWORD) {
        println("Invalid password!");
    }
    else {
        println("User not found!");
    }

    return 0;
}
