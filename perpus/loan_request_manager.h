#pragma once

#include <optional>
#include <string>
#include <vector>
#include <ctime>

class BookManager;

enum class LoanStatus {
    Pending,
    Approved,
    Rejected,
    Returned
};

struct LoanRequest {
    int id;
    std::string username;
    std::string bookId;
    std::time_t requestedAt;
    LoanStatus status;
};

class LoanRequestManager {
public:
    explicit LoanRequestManager(BookManager& bookManager);

    const std::vector<LoanRequest>& list_requests() const;
    std::optional<LoanRequest> find_by_id(int id) const;
    LoanRequest* get_request(int id);

    std::vector<LoanRequest> requests_for_user(const std::string& username) const;
    bool user_has_pending_for_book(const std::string& username, const std::string& bookId) const;

    bool create_request(const std::string& username, const std::string& bookId);
    bool set_status(int id, LoanStatus status);
    void rename_user_requests(const std::string& oldUsername, const std::string& newUsername);

    static std::string human_date(std::time_t time);
    static std::string status_label(LoanStatus status);

    void clear();
    void add_request_raw(const LoanRequest& request);
    void set_next_id(int next);

private:
    BookManager& bookManager;
    int nextId;
    std::vector<LoanRequest> requests;

    LoanRequest* find_mutable(int id);
};
