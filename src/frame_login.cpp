#include "../include/frame_login.hpp"
#include "../include/mysql_connection.hpp"
#include "../include/frame_dashboard.hpp"
#include <wx/wx.h>
#include <string>
#include <iostream>

using namespace std;

LoginFrame::LoginFrame(const wxString& title, MySQLConnection& dbConn): wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)), db(dbConn){
    wxPanel* panel = new wxPanel(this);
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* usernameSizer = new wxBoxSizer(wxVERTICAL);
    usernameSizer->Add(new wxStaticText(panel, wxID_ANY, "Username:"), 0, wxBOTTOM, 4);
    usernameInput = new wxTextCtrl(panel, wxID_ANY);
    usernameSizer->Add(usernameInput, 1);
    vbox -> Add(usernameSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP , 10);

    wxBoxSizer* passwordSizer = new wxBoxSizer(wxVERTICAL);
    passwordSizer->Add(new wxStaticText(panel, wxID_ANY, "Password: "), 0, wxBOTTOM, 4);
    passwordInput = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
    passwordSizer->Add(passwordInput, 1);
    vbox->Add(passwordSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);

    wxButton* loginButton = new wxButton(panel, wxID_ANY, "Login");
    vbox->Add(loginButton, 0, wxALIGN_CENTER | wxALL, 20);

    panel->SetSizer(vbox);

    loginButton->Bind(wxEVT_BUTTON, &LoginFrame::OnLoginClick, this);
    this->Bind(wxEVT_CLOSE_WINDOW, &LoginFrame::OnClose, this);
}

void LoginFrame::OnClose(wxCloseEvent& evt) {
    (void)evt;
    Destroy();
    wxTheApp->ExitMainLoop();
}


void LoginFrame::OnLoginClick(wxCommandEvent& evt){
    (void)evt;
    HandleUserLogin();
}

void LoginFrame::HandleUserLogin() {
    if (!db.connect()) {
        wxMessageBox("Failed to connect to the database.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    string username = usernameInput->GetValue().ToStdString();
    string password = passwordInput->GetValue().ToStdString();

    if(username.empty() || password.empty()){
        wxMessageBox("All fields are required.", "Error", wxOK | wxICON_ERROR);
        return;
    }
    if (db.login(username, password)) {
        wxMessageBox("User logged in successfully!", "Success", wxOK | wxICON_INFORMATION);
        DashboardFrame* dashboardFrame = new DashboardFrame("Dashboard Finance", wxString(username), db);
        dashboardFrame->Show(true);
        
        wxTheApp->SetTopWindow(dashboardFrame);
        this->Destroy();
    } 
    else {
        wxMessageBox("Login failed.", "Error", wxOK | wxICON_ERROR);
    }
    cout<<"Done login"<<endl;
} 
