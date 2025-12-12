#pragma once

#include <optional>
#include <string>

#include "../account/account_manager.h"

std::optional<std::string> login(AccountManager& accountManager);
bool register_account(AccountManager& accountManager);
