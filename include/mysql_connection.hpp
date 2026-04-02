#ifndef MY_SQL_CONNECTION_HPP
#define MY_SQL_CONNECTION_HPP

#include <string>
#include <vector>
#if __has_include(<mysql/mysql.h>)
#include <mysql/mysql.h>
#elif __has_include(<mysql.h>)
#include <mysql.h>
#elif __has_include(<mariadb/mysql.h>)
#include <mariadb/mysql.h>
#else
#error "MySQL/MariaDB headers not found. Install libmysqlclient-dev or libmariadb-dev."
#endif

using namespace std;

struct SpendingEntry {
    string createdAt;
    string category;
    double amount;
    string description;
};

class MySQLConnection{
    private:
        MYSQL* conn;
        const char* host;
        const char* user;
        const char* password;
        const char* database;
        int port;
        bool connected;
        string lastError;

        bool getUserId(const string& nickname, int& userId);
        bool getCategoryIdOrCreate(const string& categoryName, int& categoryId);
    public:
        MySQLConnection();
        ~MySQLConnection();

        bool connect();
        void disconnect();
        const string& getLastError() const;
        bool executeQuery(const string& query);
        bool registration(const string& nickname, const string& name,
                    const string& password, const string& email);
        bool login(const string& username, const string& password);
        bool logout();

        bool addSpending(const string& nickname, const string& category,
                         double amount, const string& description);
        vector<SpendingEntry> getSpendings(const string& nickname, int limit = 20);
        string getUserName(const string& nickname);
};

#endif
