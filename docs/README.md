# perpus_cli

CLI-based library manager with accounts, books, and loan requests.

## Overview
- Roles: Admin and User.
- Data on disk:
  - Books: `details/<book_id>.json` (metadata), PDFs at `pdf/<book_id>.pdf`.
  - Accounts: `data/accounts.txt` lines `username|hash|Role`.
  - Loans: `data/loans.txt` lines `id|username|bookId|timestamp|Status` (Pending/Approved/Rejected/Returned).
- Seeded accounts (if `data/` missing): `admin/admin` (Admin), `demo/password` (User).

## Build
- Requirements: CMake, C++20 compiler.
- Quick: `./build.sh` (uses `build/` or `$BUILD_DIR`).
- Manual: `cmake -S . -B build && cmake --build build`.
- Binary: `build/perpus_cli` (or `./perpus_cli` if built manually).

## Run
1. Build (see above).
2. Run `build/perpus_cli`.
3. Delete `data/` to reset accounts/loans; delete `details/` and `pdf/` to reset books.

## Usage
- Main menu: login or create account.
- User menu: browse, read (show PDF path), request loan, check status, return (mark approved as returned), manage account.
- Admin menu: manage books (list/add/edit), review loan requests (approve/reject), manage accounts (list/edit/create, change role).

## Docs
- See `docs/` for per-directory notes (`util.md`, `perpus.md`, `account.md`, `app.md`) and menu flow (`menus.md`, `PSEUDOCODE.txt`).
