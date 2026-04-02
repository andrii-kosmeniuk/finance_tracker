#ifndef APP_HPP
#define APP_HPP

#include <wx/wx.h>
#include <memory>
#include "mysql_connection.hpp"

class App : public wxApp{
    public:
        bool OnInit();
    private:
        std::unique_ptr<MySQLConnection> db;
};

#endif
