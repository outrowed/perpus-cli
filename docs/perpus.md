# perpus/

- `book.h`: Book struct (id, isbn, title, author, publisher, year, stock).
- `book_manager.h/.cpp`: load/save books to `details/<id>.json`, resolve PDF path `pdf/<id>.pdf`, list/add/update/remove, generate ids.
- `loan_request_manager.h/.cpp`: loan requests (Pending/Approved/Rejected/Returned), list/find/create, set status, rename user in requests, next-id tracking.
