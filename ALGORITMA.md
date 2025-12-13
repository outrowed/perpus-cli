## Algoritma Sistem Perpustakaan Berbasis CLI

### 1. Inisialisasi Sistem

1. Sistem dijalankan.
2. Sistem memuat data akun dari berkas `data/accounts.txt`.
   Apabila berkas tidak ditemukan, sistem menginisialisasi data akun kosong.
3. Sistem memuat data peminjaman dari berkas `data/loans.txt`.
   Apabila berkas tidak ditemukan, sistem menginisialisasi data peminjaman kosong.
4. Sistem memuat data buku dari direktori `details/` dalam format berkas JSON.
5. Apabila data akun atau data buku belum tersedia, sistem membuat data awal (default), kemudian menyimpannya ke dalam media penyimpanan.

### 2. Menu Utama

6. Sistem menampilkan **Menu Utama** dengan pilihan:

   * Login
   * Pendaftaran akun baru
   * Keluar dari sistem
7. Sistem menjalankan perulangan menu utama sampai pengguna memilih keluar.
8. Jika pengguna memilih **Login**:

   * Sistem meminta username dan password.
   * Sistem melakukan proses autentikasi.
   * Jika peran pengguna adalah **admin**, sistem menampilkan Menu Admin.
   * Jika peran pengguna adalah **user**, sistem menampilkan Menu User.
9. Jika pengguna memilih **Pendaftaran Akun**:

   * Sistem meminta data akun baru.
   * Sistem menyimpan akun dengan password yang telah di-hash.
10. Jika pengguna memilih **Keluar**, sistem menghentikan proses.

### 3. Menu User

11. Setelah berhasil login sebagai user, sistem menampilkan **Menu User** yang terdiri atas:

    * Menampilkan daftar buku
    * Membaca buku
    * Mengajukan peminjaman buku
    * Melihat status peminjaman
    * Mengembalikan buku
    * Mengelola akun
    * Logout

12. Sistem menjalankan menu user secara berulang sampai user melakukan logout.
13. Jika user memilih **Menampilkan daftar buku**, sistem menampilkan seluruh data buku yang tersedia.
14. Jika user memilih **Membaca buku**, sistem menampilkan lokasi berkas PDF buku yang dipilih serta memberikan peringatan jika berkas tidak ditemukan.
15. Jika user memilih **Mengajukan peminjaman**, sistem mencatat permintaan peminjaman dengan status *Pending*.
16. Jika user memilih **Melihat status peminjaman**, sistem menampilkan seluruh riwayat peminjaman milik user beserta statusnya.
17. Jika user memilih **Mengembalikan buku**, sistem mengubah status peminjaman dari *Approved* menjadi *Returned*.
18. Jika user memilih **Mengelola akun**, sistem mengarahkan ke menu pengelolaan akun.
19. Jika user memilih **Logout**, sistem kembali ke Menu Utama.

### 4. Pengelolaan Akun User

20. Sistem menampilkan menu pengelolaan akun user.
21. User dapat melakukan perubahan username dengan syarat username baru belum digunakan.
22. Sistem memperbarui data username pada akun serta seluruh data peminjaman yang terkait.
23. User dapat melakukan perubahan password, kemudian sistem menyimpan password baru dalam bentuk hash.
24. User dapat kembali ke menu sebelumnya.

### 5. Menu Admin

25. Setelah login sebagai admin, sistem menampilkan **Menu Admin** yang terdiri atas:

    * Pengelolaan buku
    * Peninjauan permintaan peminjaman
    * Pengelolaan akun
    * Logout
26. Sistem menjalankan menu admin secara berulang sampai admin melakukan logout.

### 6. Pengelolaan Data Buku

27. Sistem menampilkan menu pengelolaan buku.
28. Admin dapat melihat daftar buku, menambahkan buku baru, atau mengedit data buku.
29. Pada proses penambahan buku:

    * Sistem meminta data buku secara lengkap.
    * Sistem melakukan validasi keunikan ISBN.
    * Sistem menyimpan data buku ke dalam berkas JSON.
30. Pada proses pengeditan buku:

    * Sistem memperbarui data buku yang dipilih.
    * Sistem menyimpan perubahan data.

### 7. Peninjauan Permintaan Peminjaman

31. Sistem menampilkan seluruh permintaan peminjaman buku.
32. Admin memilih permintaan yang akan diproses.
33. Admin dapat menyetujui (*Approved*) atau menolak (*Rejected*) permintaan peminjaman.
34. Sistem menyimpan perubahan status peminjaman ke dalam berkas.

### 8. Pengelolaan Akun oleh Admin

35. Sistem menampilkan menu pengelolaan akun.
36. Admin dapat melihat daftar akun, mengedit akun, atau menambahkan akun baru.
37. Pada proses pengeditan akun, admin dapat:

    * Mengubah username
    * Mengatur ulang password
    * Mengubah peran akun (admin atau user)
38. Sistem menyimpan seluruh perubahan data akun dan data peminjaman yang terkait.

### 9. Penyimpanan Data

39. Sistem menyimpan data secara permanen menggunakan sistem berkas, dengan ketentuan:

* Data buku disimpan dalam `details/<id>.json`
* Data akun disimpan dalam `data/accounts.txt`
* Data peminjaman disimpan dalam `data/loans.txt`

40. Setiap perubahan data langsung disinkronkan ke berkas penyimpanan.

### 10. Terminasi Sistem

41. Sistem dihentikan ketika pengguna memilih opsi keluar dari Menu Utama.

