#pragma once

#include <optional>
#include <string>

#include "../account/account_manager.h"
#include "../perpus/book_manager.h"
#include "../perpus/loan_request_manager.h"
#include "../util/prompt.h"
#include "../util/print.h"

std::string role_to_string(Role role);
std::optional<Role> parse_role(const std::string& raw);
int prompt_int_with_default(const std::string& message, int currentValue);
void show_book_list(const BookManager& manager);
void print_request_card(const LoanRequest& request, const BookManager& manager);
void ensure_pdf_placeholder(const BookManager& manager, const Book& book);
