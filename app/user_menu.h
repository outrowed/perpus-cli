#pragma once

#include <string>

#include "session.h"

// Entry point for user menus (browse/read/request/return/manage account).
void run_user_menu(Session& session, const std::string& username);
