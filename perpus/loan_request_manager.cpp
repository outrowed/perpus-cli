#include "loan_request_manager.h"

#include <ctime>
#include <iomanip>
#include <sstream>

#include "book_manager.h"

using std::string;
using std::vector;

namespace {
    string status_to_string_internal(LoanStatus status) {
        switch (status) {
            case LoanStatus::Pending: return "Pending";
            case LoanStatus::Approved: return "Approved";
            case LoanStatus::Rejected: return "Rejected";
            case LoanStatus::Returned: return "Returned";
        }
        return "Unknown";
    }
}

LoanRequestManager::LoanRequestManager(BookManager& manager)
    : bookManager(manager), nextId(1) {}

const vector<LoanRequest>& LoanRequestManager::list_requests() const {
    return requests;
}

std::optional<LoanRequest> LoanRequestManager::find_by_id(int id) const {
    for (vector<LoanRequest>::const_iterator it = requests.begin(); it != requests.end(); ++it) {
        if (it->id == id) {
            return *it;
        }
    }
    return std::nullopt;
}

LoanRequest* LoanRequestManager::find_mutable(int id) {
    for (vector<LoanRequest>::iterator it = requests.begin(); it != requests.end(); ++it) {
        if (it->id == id) {
            return &(*it);
        }
    }
    return nullptr;
}

LoanRequest* LoanRequestManager::get_request(int id) {
    return find_mutable(id);
}

std::vector<LoanRequest> LoanRequestManager::requests_for_user(const std::string& username) const {
    vector<LoanRequest> result;
    for (vector<LoanRequest>::const_iterator it = requests.begin(); it != requests.end(); ++it) {
        if (it->username == username) {
            result.push_back(*it);
        }
    }
    return result;
}

bool LoanRequestManager::user_has_pending_for_book(const std::string& username, const std::string& bookId) const {
    for (vector<LoanRequest>::const_iterator it = requests.begin(); it != requests.end(); ++it) {
        if (it->username == username && it->bookId == bookId && it->status == LoanStatus::Pending) {
            return true;
        }
    }
    return false;
}

bool LoanRequestManager::create_request(const std::string& username, const std::string& bookId) {
    const Book* book = bookManager.get_book_by_id(bookId);
    if (book == nullptr) {
        return false;
    }
    if (user_has_pending_for_book(username, bookId)) {
        return false;
    }

    LoanRequest request;
    request.id = nextId++;
    request.username = username;
    request.bookId = bookId;
    request.requestedAt = std::time(nullptr);
    request.status = LoanStatus::Pending;

    requests.push_back(request);
    return true;
}

bool LoanRequestManager::set_status(int id, LoanStatus status) {
    LoanRequest* request = find_mutable(id);
    if (request == nullptr) {
        return false;
    }
    request->status = status;
    return true;
}

void LoanRequestManager::rename_user_requests(const std::string& oldUsername, const std::string& newUsername) {
    for (vector<LoanRequest>::iterator it = requests.begin(); it != requests.end(); ++it) {
        if (it->username == oldUsername) {
            it->username = newUsername;
        }
    }
}

std::string LoanRequestManager::human_date(std::time_t time) {
    std::ostringstream oss;
    std::tm tm = *std::localtime(&time);
    oss << std::put_time(&tm, "%Y-%m-%d");
    return oss.str();
}

std::string LoanRequestManager::status_label(LoanStatus status) {
    return status_to_string_internal(status);
}

void LoanRequestManager::clear() {
    requests.clear();
    nextId = 1;
}

void LoanRequestManager::add_request_raw(const LoanRequest& request) {
    requests.push_back(request);
}

void LoanRequestManager::set_next_id(int next) {
    if (next > 1) {
        nextId = next;
    } else {
        nextId = 1;
    }
}
