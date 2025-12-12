# Menus and Flows

## Main
- Load accounts, loans, books; seed defaults if missing; save accounts/loans.
- Main menu: Login / Create account / Exit. Admin -> admin menu; user -> user menu.

## Login/Register
- Login: prompt username/password; authenticate by role.
- Register: create user (hash password), save accounts.

## User Menu
- Browse library: list books.
- Read: show PDF path for a book id.
- Request loan: create Pending (if not already pending), save loans.
- Check loan status: list this user's requests with status/date.
- Return book: list Approved requests, mark Returned, save loans.
- Manage account: change username/password; saves accounts and updates loan usernames.

## Admin Menu
- Manage books: list/add/edit (isbn/title/author/publisher/year/stock); saves to `details/<id>.json` and PDF path.
- Review loan requests: list all, approve/reject by ID; saves loans.
- Manage accounts: list/edit/create; change username/password/role (propagate usernames to loans), save accounts and loans when needed.
