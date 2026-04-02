#include "../include/utils.hpp"
#include "../include/mysql_connection.hpp"
#include "../include/app.hpp"
#include "../include/frame_start.hpp"
#include <iostream>

using namespace std;

bool App::OnInit(){
    loadEnvFile();
    db = std::make_unique<MySQLConnection>();

    if(!db->connect())
        return false;
    cout << "Program is running!"<<endl;

    const string create_db_query ="CREATE DATABASE IF NOT EXISTS manage_spendings";
    if(db->executeQuery(create_db_query))
        cout<<"Database created successfully!"<<endl;

    if(executeSQLFromFile(*db, "queries/user/create_table.sql"))
        cout<<"User table created successfully!"<<endl; 

    if(executeSQLFromFile(*db, "queries/category/create_table.sql"))
        cout<<"Category table created successfully!"<<endl;

    if(executeSQLFromFile(*db, "queries/spendings/create_table.sql"))
        cout<<"Spendings table created successfully!"<<endl;

    if(executeSQLFromFile(*db, "queries/friendship_request/create_table.sql"))
        cout<<"Friendship table created successfully!"<<endl;

    if(executeSQLFromFile(*db, "queries/friends/create_table.sql"))
        cout<<"Friends table created successfully!"<<endl;

    if(executeSQLFromFile(*db, "queries/visibility/create_table.sql"))
        cout<<"Visibility table created successfully!"<<endl;                                     

    StartFrame* start = new StartFrame("Welcome", *db);
    start->SetClientSize(800, 600);
    start->Center();
    start->Show(true);
    SetTopWindow(start);

    cout<<"Gui created in the center"<<endl;
    return true;
}
