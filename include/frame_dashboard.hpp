#ifndef DASH_HPP
#define DASH_HPP

#include <wx/statline.h>
#include <wx/wx.h>
#include <vector>
#include "mysql_connection.hpp"

class DashboardFrame: public wxFrame {
    public:
        DashboardFrame(const wxString& title, const wxString& name, MySQLConnection& db);
        ~DashboardFrame();
    private:
        void OnClose(wxCloseEvent& evt);
        void OnProfileClicked(wxCommandEvent& event);
        void OnHistoryClicked(wxCommandEvent& event);
        void OnFriendsClicked(wxCommandEvent& event);
        void OnLogoutClicked(wxCommandEvent& event);
        wxPanel* CreateExpensesChart(wxWindow* parent);
        void DrawExpensesChart(wxPaintEvent& event);

        wxPanel* contentPanel;
        wxPanel* panel;
        wxStaticText* welcomeText;
        wxBoxSizer* navSizer;

        wxButton* btnProfile;
        wxButton* btnHistory;
        wxButton* btnFriends;

        MySQLConnection& db;
        std::vector<std::pair<wxString, double>> monthlyExpenses;
};

#endif
