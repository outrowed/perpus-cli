# app/

- `auth.*`: login/register flows.
- `menu_helpers.*`: shared UI helpers (role parsing, book listing, placeholder PDF creation).
- `session.*`: wires managers and handles load/save for accounts (`data/accounts.txt`) and loans (`data/loans.txt`); sets file paths.
- `seed.*`: seeds default admin/user and sample books.
- `user_menu.*`: user menu actions (browse, read, request, check status, return, manage account; saves when mutating).
- `admin_menu.*`: admin menu actions (book management, review loan requests, manage accounts; saves when mutating).
- `PSEUDOCODE.txt`: high-level flow for menus, loops, and storage format.
