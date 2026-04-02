#include "../include/frame_register.hpp"
#include "../include/mysql_connection.hpp"
#include "../include/frame_dashboard.hpp"
#include <wx/wx.h>
#include <string>
#include <iostream>

using namespace std;

RegisterFrame::RegisterFrame(const wxString& title, MySQLConnection& dbConn): wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)), db(dbConn){
    wxPanel* panel = new wxPanel(this);
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* usernameSizer = new wxBoxSizer(wxVERTICAL);
    usernameSizer->Add(new wxStaticText(panel, wxID_ANY, "Username:"), 0, wxBOTTOM, 4);
    usernameInput = new wxTextCtrl(panel, wxID_ANY);
    usernameSizer->Add(usernameInput, 1);
    vbox -> Add(usernameSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP , 10);

    wxBoxSizer* nameSizer = new wxBoxSizer(wxVERTICAL);
    nameSizer->Add(new wxStaticText(panel, wxID_ANY, "Name:"), 0, wxBOTTOM, 4);
    nameInput = new wxTextCtrl(panel, wxID_ANY);
    nameSizer->Add(nameInput, 1);
    vbox -> Add(nameSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP , 10);

    wxBoxSizer* emailSizer = new wxBoxSizer(wxVERTICAL);
    emailSizer->Add(new wxStaticText(panel, wxID_ANY, "Email:"), 0, wxBOTTOM, 4);
    emailInput = new wxTextCtrl(panel, wxID_ANY);
    emailSizer->Add(emailInput, 1);
    vbox -> Add(emailSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP , 10);

    wxBoxSizer* passwordSizer = new wxBoxSizer(wxVERTICAL);
    passwordSizer->Add(new wxStaticText(panel, wxID_ANY, "Password: "), 0, wxBOTTOM, 4);
    passwordInput = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
    passwordSizer->Add(passwordInput, 1);
    vbox->Add(passwordSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);

    wxButton* registerButton = new wxButton(panel, wxID_ANY, "Register");
    vbox->Add(registerButton, 0, wxALIGN_CENTER | wxALL, 20);

    panel->SetSizer(vbox);

    registerButton->Bind(wxEVT_BUTTON, &RegisterFrame::OnRegisterClick, this);
    this->Bind(wxEVT_CLOSE_WINDOW, &RegisterFrame::OnClose, this);
}

void RegisterFrame::OnClose(wxCloseEvent& evt) {
    Destroy();
    wxTheApp->ExitMainLoop();
}

void RegisterFrame::OnRegisterClick(wxCommandEvent& evt){
    HandleUserRegistration();
}

void RegisterFrame::HandleUserRegistration() {
    if (!db.connect()) {
        wxMessageBox("Failed to connect to the database.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    string username = usernameInput->GetValue().ToStdString();
    string name = nameInput->GetValue().ToStdString();
    string email = emailInput->GetValue().ToStdString();
    string password = passwordInput->GetValue().ToStdString();

    if(username.empty() || name.empty() || email.empty() || password.empty()){
        wxMessageBox("All fields are required.", "Error", wxOK | wxICON_ERROR);
        return;
    }
    if (db.registration(username, name, password, email)) {
        wxMessageBox("User registered successfully!", "Success", wxOK | wxICON_INFORMATION);
        DashboardFrame* dashboardFrame = new DashboardFrame("Dashboard Finance", wxString(username), db);
        dashboardFrame->Show(true);
        this->Destroy();
        wxTheApp->SetTopWindow(dashboardFrame);
    } 
    else {
        wxMessageBox("Registration failed.", "Error", wxOK | wxICON_ERROR);
    }
    cout<<"Done registration"<<endl;
}