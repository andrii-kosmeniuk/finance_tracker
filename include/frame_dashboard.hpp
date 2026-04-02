#ifndef DASH_HPP
#define DASH_HPP

#include <wx/statline.h>
#include <wx/wx.h>
#include <vector>
#include "mysql_connection.hpp"

class DashboardFrame: public wxFrame {
    public:
        DashboardFrame(const wxString& title, const wxString& nickname, MySQLConnection& db);
        ~DashboardFrame();
    private:
        void OnClose(wxCloseEvent& evt);
        void OnProfileClicked(wxCommandEvent& event);
        void OnHistoryClicked(wxCommandEvent& event);
        void OnFriendsClicked(wxCommandEvent& event);
        void OnLogoutClicked(wxCommandEvent& event);
        void OnAddSpendingClicked(wxCommandEvent& event);

        void ShowWelcome();

        wxPanel* contentPanel;
        wxPanel* panel;

        MySQLConnection& db;
        wxString currentNickname;
};

#endif
