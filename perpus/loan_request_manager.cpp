#include "loan_request_manager.h"

#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "book_manager.h"

using std::string;
using std::vector;

namespace {
    // Simple helpers to map status to string and parse time formatting.
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

LoanRequest LoanRequestManager::get_by_id(int id) const {
    size_t index = find_index(id);
    return requests[index];
}

LoanRequest& LoanRequestManager::get_request(int id) {
    size_t index = find_index(id);
    return requests[index];
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
    string normalizedBookId;
    try {
        const Book& book = bookManager.get_book_by_id(bookId);
        normalizedBookId = book.isbn;
    } catch (const std::exception&) {
        return false;
    }
    if (user_has_pending_for_book(username, normalizedBookId)) {
        return false;
    }

    LoanRequest request;
    request.id = nextId++;
    request.username = username;
    request.bookId = normalizedBookId;
    request.requestedAt = std::time(0);
    request.status = LoanStatus::Pending;

    requests.push_back(request);
    return true;
}

bool LoanRequestManager::set_status(int id, LoanStatus status) {
    try {
        LoanRequest& request = get_request(id);
        if (request.status == status) {
            return true;
        }

        // Approving consumes stock; returning replenishes it.
        if (status == LoanStatus::Approved) {
            if (request.status != LoanStatus::Pending) {
                return false;
            }
            Book book = bookManager.get_book_by_id(request.bookId);
            if (book.stock <= 0) {
                return false;
            }
            book.stock -= 1;
            if (!bookManager.update_book_by_id(book.isbn, book)) {
                return false;
            }
            request.status = status;
            return true;
        }

        if (status == LoanStatus::Returned) {
            if (request.status != LoanStatus::Approved) {
                return false;
            }
            Book book = bookManager.get_book_by_id(request.bookId);
            book.stock += 1;
            if (!bookManager.update_book_by_id(book.isbn, book)) {
                return false;
            }
            request.status = status;
            return true;
        }

        request.status = status;
        return true;
    } catch (const std::exception&) {
        return false;
    }
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

size_t LoanRequestManager::find_index(int id) const {
    for (size_t i = 0; i < requests.size(); ++i) {
        if (requests[i].id == id) {
            return i;
        }
    }
    throw std::runtime_error("Loan request not found: " + std::to_string(id));
}
