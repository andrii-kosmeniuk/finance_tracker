#include "../include/utils.hpp"
#include "../include/mysql_connection.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <sodium.h>

using namespace std;

bool loadEnvFile(const string& filepath)
{
    ifstream file(filepath);
    if(!file.is_open())
    {
        cerr << "Warning: .env file not found at '" << filepath
             << "'. Using built-in DB defaults (host=127.0.0.1, port=3306)." << endl;
        return false;
    }

    string line;
    while(getline(file,line))
    {
        if(line.empty() || line[0] == '#') continue;

        size_t pos = line.find('=');
        if(pos == string::npos) continue;

        string key = line.substr(0,pos);
        string value = line.substr(pos+1);

        key.erase(0, key.find_first_not_of( "\t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of( "\t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        setenv(key.c_str(),value.c_str(),1);
    }
    cout<<"Variables from .env file loaded successfully!"<<endl;
    return true;
}

string hashPassword(const string& password)
{
    if(!(sodium_init() == 0))
        throw runtime_error("Failed to initialize libsodium\n");
 
    char hash[crypto_pwhash_STRBYTES];
    if(crypto_pwhash_str(hash, password.c_str(),password.length(),
                        crypto_pwhash_OPSLIMIT_INTERACTIVE,crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0)
        throw runtime_error("Out of memory while hashing password\n");
    return string(hash);
}

bool verifyPassword(const string& hash, const string& password)
{
    return crypto_pwhash_str_verify(hash.c_str(), password.c_str(), password.length()) == 0;
}

bool executeSQLFromFile(MySQLConnection& db, const string& filepath){
    ifstream file(filepath);
    if(!file.is_open()){
        cerr<<"Failed to open SQL file: "<<filepath<<endl;
        return false;
    }

    stringstream buffer;
    buffer << file.rdbuf();

    string sql = buffer.str();

    size_t pos = 0;
    while((pos = sql.find(';')) != string::npos){
        string query = sql.substr(0, pos+1);
        sql.erase(0,pos+1);
        if(query.find_first_not_of(" \n\t\r") != string::npos){
            if (!db.executeQuery(query)) {
                return false;
            }
        }
    }
    return true;
}
