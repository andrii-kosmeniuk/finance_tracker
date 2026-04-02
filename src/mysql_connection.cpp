#include "../include/mysql_connection.hpp"
#include "../include/utils.hpp"
#include <wx/wx.h>
#include <iostream>
#include <string>
#include <string.h>
#include <cstdlib>
#include <sodium.h>
#include <sstream>
#include <cstdlib>
#include <limits>

using namespace std;

MySQLConnection::MySQLConnection(): conn(NULL), connected(false){
    const char* host_env = getenv("DB_HOST");
    const char* user_env = getenv("DB_USER");
    const char* password_env = getenv("DB_PASSWORD");
    const char* database_env = getenv("DB_NAME");
    const char* port_env = getenv("DB_PORT");

    host = host_env ? host_env : "localhost";
    user = user_env ? user_env : "root";
    password = password_env ? password_env : "your_password";
    database = database_env ? database_env : "manage_spendings";
    port = 3360;

    if (port_env != nullptr) {
        try {
            port = stoi(port_env);
        } catch (const exception&) {
            cerr << "Invalid DB_PORT value, using default 3360." << endl;
        }
    }

    conn = mysql_init(NULL);
    if(conn == NULL)
        cerr<<"MySQL initialization failed"<<endl;
}

MySQLConnection::~MySQLConnection(){
    disconnect();
}

bool MySQLConnection::connect()
{
    if (connected) return true;

    if (conn == nullptr) {
        conn = mysql_init(nullptr);
        if (conn == nullptr) {
            cerr << "mysql_init failed!" << endl;
            return false;
        }
    }

    if (mysql_real_connect(conn, host, user, password, database, port, NULL, 0) == NULL) {
        cerr << "Connection error: " << mysql_error(conn) << endl;
        return false;
    }
    connected = true;
    cout << "Connected to MySQL database: " << database << endl;
    return true;
}

void MySQLConnection::disconnect()
{
    if (connected && conn)
    {
        mysql_close(conn);
        conn = nullptr;
        connected = false;
        cout << "Disconnected from MySQL database" << endl;
    }
}

bool MySQLConnection::executeQuery(const string& query)
{
    if(!connected)
    {
        cerr<<"Database is not connected"<<endl;
        return false;
    }
    if(mysql_query(conn, query.c_str()) != 0)
    {
        cerr<<"Error during query execution: "<<mysql_error(conn)<<endl;
        return false;
    }
    return true;
}

bool MySQLConnection::registration(const string& nickname, const string& name,
                            const string& password, const string& email){
    if(!connected) {
        cerr << "Database is not connected" << endl;
        return false;
    }

    string hashedPassword = hashPassword(password);

    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    if (!stmt) {
        cerr << "Could not initialize statement" << endl;
        return false;
    }

    const char *query = "INSERT INTO User (nickname, name, password, email) VALUES (?, ?, ?, ?)";

    if (mysql_stmt_prepare(stmt, query, strlen(query)) != 0) {
        cerr << "Failed to prepare statement: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return false;
    }

    MYSQL_BIND bind[4];
    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = (void*)nickname.c_str();
    bind[0].buffer_length = nickname.length();

    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = (void*)name.c_str();
    bind[1].buffer_length = name.length();

    bind[2].buffer_type = MYSQL_TYPE_STRING;
    bind[2].buffer = (void*)hashedPassword.c_str();
    bind[2].buffer_length = hashedPassword.length();

    bind[3].buffer_type = MYSQL_TYPE_STRING;
    bind[3].buffer = (void*)email.c_str();
    bind[3].buffer_length = email.length();

    if (mysql_stmt_bind_param(stmt, bind) != 0) {
        cerr << "Failed to bind parameters: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return false;
    }

    if (mysql_stmt_execute(stmt) != 0) {
        cerr << "Registration failed: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return false;
    }

    mysql_stmt_close(stmt);
    cout << "User successfully registered!" << endl;
    return true;
}

bool MySQLConnection::logout(){
    if (!connected) {
        cerr << "Database is not connected" << endl;
        return false;
    }

    int answer = wxMessageBox("Are you sure you want to logout?", "Confirm Logout", wxYES_NO | wxICON_QUESTION);
    if (answer == wxYES) {
        disconnect();
        cout << "User logged out successfully!" << endl;
        return true;
    } else {
        cout << "Logout canceled." << endl;
        return false;
    }
}

bool MySQLConnection::login(const string& username, const string& password){
    if(!connected) {
        cerr << "Database is not connected" << endl;
        return false;
    }

    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    if (!stmt) {
        cerr << "Could not initialize statement" << endl;
        return false;
    }

    const char *query = "SELECT password FROM User WHERE nickname = ?";

    if (mysql_stmt_prepare(stmt, query, strlen(query)) != 0) {
        cerr << "Failed to prepare statement: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return false;
    }

    MYSQL_BIND bind[1];
    memset(bind, 0, sizeof(bind));
    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = (void*)username.c_str();
    bind[0].buffer_length = username.length();

    if (mysql_stmt_bind_param(stmt, bind) != 0) {
        cerr << "Failed to bind parameters: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return false;
    }

    if (mysql_stmt_execute(stmt) != 0) {
        cerr << "Query execution failed: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return false;
    }

    MYSQL_BIND result[1];
    memset(result, 0, sizeof(result));

    char hash_buffer[crypto_pwhash_STRBYTES];
    unsigned long hash_length = 0;
    bool is_null = false;

    result[0].buffer_type = MYSQL_TYPE_STRING;
    result[0].buffer = hash_buffer;
    result[0].buffer_length = sizeof(hash_buffer);
    result[0].length = &hash_length;
    result[0].is_null = &is_null;

    if (mysql_stmt_bind_result(stmt, result) != 0) {
        cerr << "Failed to bind result: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return false;
    }

    int fetchStatus = mysql_stmt_fetch(stmt);
    if (fetchStatus == MYSQL_NO_DATA) {
        mysql_stmt_close(stmt);
        cout << "Invalid username or password." << endl;
        return false;
    }

    if (fetchStatus == 1) {
        cerr << "Failed to fetch result: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return false;
    }

    if (is_null) {
        mysql_stmt_close(stmt);
        cout << "Invalid username or password." << endl;
        return false;
    }

    string storedHash(hash_buffer, hash_length);
    mysql_stmt_close(stmt);

    bool success = verifyPassword(storedHash, password);

    if (success) {
        cout << "Successful login!" << endl;
    } else {
        cout << "Invalid username or password." << endl;
    }

    return success;
}

bool MySQLConnection::getUserId(const string& nickname, int& userId) {
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        return false;
    }

    const char* query = "SELECT id FROM User WHERE nickname = ?";
    if (mysql_stmt_prepare(stmt, query, strlen(query)) != 0) {
        mysql_stmt_close(stmt);
        return false;
    }

    MYSQL_BIND bind[1] = {};
    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = (void*)nickname.c_str();
    bind[0].buffer_length = nickname.length();

    if (mysql_stmt_bind_param(stmt, bind) != 0 || mysql_stmt_execute(stmt) != 0) {
        mysql_stmt_close(stmt);
        return false;
    }

    MYSQL_BIND result[1] = {};
    int fetchedId = 0;
    bool isNull = false;

    result[0].buffer_type = MYSQL_TYPE_LONG;
    result[0].buffer = &fetchedId;
    result[0].is_null = &isNull;

    if (mysql_stmt_bind_result(stmt, result) != 0) {
        mysql_stmt_close(stmt);
        return false;
    }

    int status = mysql_stmt_fetch(stmt);
    mysql_stmt_close(stmt);

    if (status == MYSQL_NO_DATA || isNull) {
        return false;
    }

    userId = fetchedId;
    return true;
}

bool MySQLConnection::getCategoryIdOrCreate(const string& categoryName, int& categoryId) {
    MYSQL_STMT* selectStmt = mysql_stmt_init(conn);
    if (!selectStmt) {
        return false;
    }

    const char* selectQuery = "SELECT id FROM Category WHERE name = ?";
    if (mysql_stmt_prepare(selectStmt, selectQuery, strlen(selectQuery)) != 0) {
        mysql_stmt_close(selectStmt);
        return false;
    }

    MYSQL_BIND selectBind[1] = {};
    selectBind[0].buffer_type = MYSQL_TYPE_STRING;
    selectBind[0].buffer = (void*)categoryName.c_str();
    selectBind[0].buffer_length = categoryName.length();

    if (mysql_stmt_bind_param(selectStmt, selectBind) != 0 || mysql_stmt_execute(selectStmt) != 0) {
        mysql_stmt_close(selectStmt);
        return false;
    }

    MYSQL_BIND selectResult[1] = {};
    int fetchedId = 0;
    bool isNull = false;
    selectResult[0].buffer_type = MYSQL_TYPE_LONG;
    selectResult[0].buffer = &fetchedId;
    selectResult[0].is_null = &isNull;

    if (mysql_stmt_bind_result(selectStmt, selectResult) != 0) {
        mysql_stmt_close(selectStmt);
        return false;
    }

    int status = mysql_stmt_fetch(selectStmt);
    mysql_stmt_close(selectStmt);

    if (status != MYSQL_NO_DATA && !isNull) {
        categoryId = fetchedId;
        return true;
    }

    MYSQL_STMT* insertStmt = mysql_stmt_init(conn);
    if (!insertStmt) {
        return false;
    }

    const char* insertQuery = "INSERT INTO Category (name) VALUES (?)";
    if (mysql_stmt_prepare(insertStmt, insertQuery, strlen(insertQuery)) != 0) {
        mysql_stmt_close(insertStmt);
        return false;
    }

    MYSQL_BIND insertBind[1] = {};
    insertBind[0].buffer_type = MYSQL_TYPE_STRING;
    insertBind[0].buffer = (void*)categoryName.c_str();
    insertBind[0].buffer_length = categoryName.length();

    if (mysql_stmt_bind_param(insertStmt, insertBind) != 0 || mysql_stmt_execute(insertStmt) != 0) {
        mysql_stmt_close(insertStmt);
        return false;
    }

    mysql_stmt_close(insertStmt);
    return getCategoryIdOrCreate(categoryName, categoryId);
}

bool MySQLConnection::addSpending(const string& nickname, const string& category,
                                  double amount, const string& description) {
    if (!connected) {
        return false;
    }

    int userId = 0;
    if (!getUserId(nickname, userId)) {
        return false;
    }

    int categoryId = 0;
    if (!getCategoryIdOrCreate(category, categoryId)) {
        return false;
    }

    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        return false;
    }

    const char* query = "INSERT INTO Spendings (category_id, user_id, amount, description) VALUES (?, ?, ?, ?)";
    if (mysql_stmt_prepare(stmt, query, strlen(query)) != 0) {
        mysql_stmt_close(stmt);
        return false;
    }

    MYSQL_BIND bind[4] = {};
    bind[0].buffer_type = MYSQL_TYPE_LONG;
    bind[0].buffer = &categoryId;

    bind[1].buffer_type = MYSQL_TYPE_LONG;
    bind[1].buffer = &userId;

    bind[2].buffer_type = MYSQL_TYPE_DOUBLE;
    bind[2].buffer = &amount;

    bind[3].buffer_type = MYSQL_TYPE_STRING;
    bind[3].buffer = (void*)description.c_str();
    bind[3].buffer_length = description.length();

    if (mysql_stmt_bind_param(stmt, bind) != 0 || mysql_stmt_execute(stmt) != 0) {
        mysql_stmt_close(stmt);
        return false;
    }

    mysql_stmt_close(stmt);
    return true;
}

vector<SpendingEntry> MySQLConnection::getSpendings(const string& nickname, int limit) {
    vector<SpendingEntry> spendings;

    if (!connected) {
        return spendings;
    }

    if (limit <= 0) {
        limit = 20;
    }

    int userId = 0;
    if (!getUserId(nickname, userId)) {
        return spendings;
    }

    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        return spendings;
    }

    const char* query =
        "SELECT DATE_FORMAT(s.date, '%Y-%m-%d %H:%i'), c.name, s.amount, COALESCE(s.description, '') "
        "FROM Spendings s "
        "JOIN Category c ON c.id = s.category_id "
        "WHERE s.user_id = ? "
        "ORDER BY s.date DESC "
        "LIMIT ?";

    if (mysql_stmt_prepare(stmt, query, strlen(query)) != 0) {
        mysql_stmt_close(stmt);
        return spendings;
    }

    MYSQL_BIND bind[2] = {};
    bind[0].buffer_type = MYSQL_TYPE_LONG;
    bind[0].buffer = &userId;

    bind[1].buffer_type = MYSQL_TYPE_LONG;
    bind[1].buffer = &limit;

    if (mysql_stmt_bind_param(stmt, bind) != 0 || mysql_stmt_execute(stmt) != 0) {
        mysql_stmt_close(stmt);
        return spendings;
    }

    MYSQL_BIND result[4] = {};
    char dateBuffer[32] = {};
    unsigned long dateLength = 0;
    bool dateNull = false;

    char categoryBuffer[64] = {};
    unsigned long categoryLength = 0;
    bool categoryNull = false;

    double amount = 0.0;
    bool amountNull = false;

    char descriptionBuffer[512] = {};
    unsigned long descriptionLength = 0;
    bool descriptionNull = false;

    result[0].buffer_type = MYSQL_TYPE_STRING;
    result[0].buffer = dateBuffer;
    result[0].buffer_length = sizeof(dateBuffer);
    result[0].length = &dateLength;
    result[0].is_null = &dateNull;

    result[1].buffer_type = MYSQL_TYPE_STRING;
    result[1].buffer = categoryBuffer;
    result[1].buffer_length = sizeof(categoryBuffer);
    result[1].length = &categoryLength;
    result[1].is_null = &categoryNull;

    result[2].buffer_type = MYSQL_TYPE_DOUBLE;
    result[2].buffer = &amount;
    result[2].is_null = &amountNull;

    result[3].buffer_type = MYSQL_TYPE_STRING;
    result[3].buffer = descriptionBuffer;
    result[3].buffer_length = sizeof(descriptionBuffer);
    result[3].length = &descriptionLength;
    result[3].is_null = &descriptionNull;

    if (mysql_stmt_bind_result(stmt, result) != 0) {
        mysql_stmt_close(stmt);
        return spendings;
    }

    while (true) {
        int status = mysql_stmt_fetch(stmt);
        if (status == MYSQL_NO_DATA) {
            break;
        }
        if (status == 1) {
            break;
        }

        SpendingEntry entry;
        entry.createdAt = dateNull ? "" : string(dateBuffer, dateLength);
        entry.category = categoryNull ? "" : string(categoryBuffer, categoryLength);
        entry.amount = amountNull ? 0.0 : amount;
        entry.description = descriptionNull ? "" : string(descriptionBuffer, descriptionLength);
        spendings.push_back(entry);
    }

    mysql_stmt_close(stmt);
    return spendings;
}

string MySQLConnection::getUserName(const string& nickname) {
    if (!connected) {
        return nickname;
    }

    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        return nickname;
    }

    const char* query = "SELECT name FROM User WHERE nickname = ?";
    if (mysql_stmt_prepare(stmt, query, strlen(query)) != 0) {
        mysql_stmt_close(stmt);
        return nickname;
    }

    MYSQL_BIND bind[1] = {};
    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = (void*)nickname.c_str();
    bind[0].buffer_length = nickname.length();

    if (mysql_stmt_bind_param(stmt, bind) != 0 || mysql_stmt_execute(stmt) != 0) {
        mysql_stmt_close(stmt);
        return nickname;
    }

    MYSQL_BIND result[1] = {};
    char nameBuffer[256] = {};
    unsigned long nameLength = 0;
    bool isNull = false;

    result[0].buffer_type = MYSQL_TYPE_STRING;
    result[0].buffer = nameBuffer;
    result[0].buffer_length = sizeof(nameBuffer);
    result[0].length = &nameLength;
    result[0].is_null = &isNull;

    if (mysql_stmt_bind_result(stmt, result) != 0) {
        mysql_stmt_close(stmt);
        return nickname;
    }

    int status = mysql_stmt_fetch(stmt);
    mysql_stmt_close(stmt);

    if (status == MYSQL_NO_DATA || isNull) {
        return nickname;
    }

    return string(nameBuffer, nameLength);
}
