#include "../include/frame_register.hpp"
#include "../include/frame_login.hpp"
#include "../include/frame_start.hpp"
#include "../include/mysql_connection.hpp"
#include <wx/wx.h>
#include <string>
#include <iostream>

using namespace std;

StartFrame::StartFrame(const wxString& title, MySQLConnection& dbConn): wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)), db(dbConn){
    wxPanel* panel = new wxPanel(this);
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    wxButton* registerButton = new wxButton(panel, wxID_ANY, "Register");
    vbox->Add(registerButton, 0, wxALIGN_CENTER | wxALL, 20);

    wxButton* loginButton = new wxButton(panel, wxID_ANY, "Login");
    vbox->Add(loginButton, 0, wxALIGN_CENTER | wxALL, 20);

    panel->SetSizer(vbox);

    registerButton->Bind(wxEVT_BUTTON, &StartFrame::OnRegisterClick, this);
    loginButton->Bind(wxEVT_BUTTON, &StartFrame::OnLoginClick, this);
}

StartFrame::~StartFrame() {}

void StartFrame::OnRegisterClick(wxCommandEvent& evt) {
    RegisterFrame* regFrame = new RegisterFrame("Register", db);
    regFrame->Show(true);
    regFrame->Center();
    wxTheApp->SetTopWindow(regFrame);
    this->Destroy();
}

void StartFrame::OnLoginClick(wxCommandEvent& evt) {
    LoginFrame* loginFrame = new LoginFrame("Login", db);
    loginFrame->Show(true);
    loginFrame->Center();
    wxTheApp->SetTopWindow(loginFrame);
    this->Destroy();
}
