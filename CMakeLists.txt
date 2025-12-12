cmake_minimum_required(VERSION 3.16)

project(perpus_cli LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES
    main.cpp
    account/account_manager.cpp
    app/admin_menu.cpp
    app/auth.cpp
    app/menu_helpers.cpp
    app/seed.cpp
    app/session.cpp
    app/user_menu.cpp
    perpus/book_manager.cpp
    perpus/loan_request_manager.cpp
    bcrypt/bcrypt.cpp
    bcrypt/blowfish.cpp
)

add_executable(perpus_cli ${SOURCES})

target_include_directories(perpus_cli PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
