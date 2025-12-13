#pragma once

#include <string>

#include "../account/account_manager.h"

std::string login(AccountManager& accountManager);
bool register_account(AccountManager& accountManager);
