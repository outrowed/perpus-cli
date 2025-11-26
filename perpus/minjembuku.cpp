#include <string>
#include <vector>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <algorithm>

#include "../util/print.cpp"
#include "./book-manager.cpp"

#pragma once

struct Loan {
    std::string username;
    std::string isbn;
    std::time_t loanDate;
    bool isReturned;
};

class LoanManager {
private:
    std::vector<Loan> loans;
    BookManager& bookManager; // Referensi ke BookManager agar bisa update stok

public:
    // Constructor menerima referensi BookManager
    LoanManager(BookManager& bm) : bookManager(bm) {
        // Nanti di sini bisa tambahkan loadLoansFromFile();
    }

    // Fungsi helper untuk mendapatkan tanggal sekarang dalam string
    std::string getCurrentDateString() {
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d");
        return oss.str();
    }

    void borrowBook(std::string username, std::string isbn) {
        // 1. Validasi Stok
        if (!bookManager.isAvailable(isbn)) {
            println("Gagal meminjam: Buku tidak ditemukan atau stok habis!");
            return;
        }

        // 2. Cek apakah user sedang meminjam buku yang sama (belum dikembalikan)
        for (const auto& loan : loans) {
            if (loan.username == username && loan.isbn == isbn && !loan.isReturned) {
                println("Anda belum mengembalikan buku ini, tidak bisa meminjam lagi!");
                return;
            }
        }

        // 3. Proses Peminjaman
        Loan newLoan = {
            username,
            isbn,
            std::time(nullptr), 
            false 
        };

        loans.push_back(newLoan);
        bookManager.decreaseStock(isbn); // Kurangi stok fisik

        println("Berhasil meminjam buku: " + bookManager.getTitle(isbn));
    }

    void returnBook(std::string username, std::string isbn) {
        bool found = false;
        for (auto& loan : loans) {
            if (loan.username == username && loan.isbn == isbn && !loan.isReturned) {
                loan.isReturned = true; // Tandai sudah kembali
                bookManager.increaseStock(isbn); // Kembalikan stok
                println("Buku berhasil dikembalikan. Terima kasih!");
                found = true;
                break;
            }
        }

        if (!found) {
            println("Tidak ditemukan data peminjaman aktif untuk buku ini.");
        }
    }

    void listMyLoans(std::string username) {
        println("\n=== Riwayat Peminjaman Saya ===");
        bool hasLoan = false;
        for (const auto& loan : loans) {
            if (loan.username == username) {
                std::string status = loan.isReturned ? "[Dikembalikan]" : "[DIPINJAM]";
                
                char buffer[80];
                std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", std::localtime(&loan.loanDate));
                
                std::cout << status << " " 
                          << bookManager.getTitle(loan.isbn) 
                          << " (Tgl: " << buffer << ")" << std::endl;
                hasLoan = true;
            }
        }
        if (!hasLoan) println("Belum ada riwayat peminjaman.");
        println("===============================\n");
    }
};
