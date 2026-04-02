#include "../include/frame_dashboard.hpp"
#include "../include/mysql_connection.hpp"
#include "../include/frame_start.hpp"
#include <wx/dcbuffer.h>
#include <wx/wx.h>
#include <wx/valnum.h>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

DashboardFrame::DashboardFrame(const wxString& title, const wxString& nickname, MySQLConnection& dbRef)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)), db(dbRef), currentNickname(nickname){

    panel = new wxPanel(this);
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    contentPanel = new wxPanel(panel, wxID_ANY);
    wxBoxSizer* contentSizer = new wxBoxSizer(wxVERTICAL);
    contentPanel->SetSizer(contentSizer);

    vbox->Add(contentPanel, 1, wxEXPAND);

    wxPanel* navPanel = new wxPanel(panel, wxID_ANY);
    navPanel->SetBackgroundColour(wxColour(240, 240, 240));

    navPanel->SetBackgroundStyle(wxBG_STYLE_PAINT);
    navPanel->Bind(wxEVT_PAINT, [](wxPaintEvent& evt) {
        wxPaintDC dc(static_cast<wxPanel*>(evt.GetEventObject()));
        wxSize size = dc.GetSize();
        dc.SetPen(wxPen(wxColour(200, 200, 200)));
        dc.DrawLine(0, 0, size.GetWidth(), 0);
    });

    wxBoxSizer* navSizer = new wxBoxSizer(wxHORIZONTAL);

    wxButton* btnProfile = new wxButton(navPanel, 1001, "Profile");
    wxButton* btnHistory = new wxButton(navPanel, 1002, "History");
    wxButton* btnFriends = new wxButton(navPanel, 1003, "Friends");

    navSizer->Add(btnProfile, 1, wxEXPAND | wxALL, 5);
    navSizer->Add(new wxStaticLine(navPanel, wxID_ANY, wxDefaultPosition, wxSize(1, -1), wxLI_VERTICAL),
                 0, wxEXPAND | wxTOP | wxBOTTOM, 5);
    navSizer->Add(btnHistory, 1, wxEXPAND | wxALL, 5);
    navSizer->Add(new wxStaticLine(navPanel, wxID_ANY, wxDefaultPosition, wxSize(1, -1), wxLI_VERTICAL),
                 0, wxEXPAND | wxTOP | wxBOTTOM, 5);
    navSizer->Add(btnFriends, 1, wxEXPAND | wxALL, 5);

    navPanel->SetSizer(navSizer);

    vbox->Add(navPanel, 0, wxEXPAND);

    panel->SetSizer(vbox);
    Center();

    btnProfile->Bind(wxEVT_BUTTON, &DashboardFrame::OnProfileClicked, this);
    btnHistory->Bind(wxEVT_BUTTON, &DashboardFrame::OnHistoryClicked, this);
    btnFriends->Bind(wxEVT_BUTTON, &DashboardFrame::OnFriendsClicked, this);

    this->Bind(wxEVT_CLOSE_WINDOW, &DashboardFrame::OnClose, this);

    ShowWelcome();
}

void DashboardFrame::OnClose(wxCloseEvent& evt) {
    Destroy();
    wxTheApp->ExitMainLoop();
}

DashboardFrame::~DashboardFrame(){}

void DashboardFrame::ShowWelcome() {
    wxBoxSizer* sizer = static_cast<wxBoxSizer*>(contentPanel->GetSizer());
    sizer->Clear(true);

    string userName = db.getUserName(currentNickname.ToStdString());
    wxString greeting = "Hello " + wxString(userName) + "!";

    wxStaticText* welcomeText = new wxStaticText(contentPanel, wxID_ANY, greeting);
    wxFont font = welcomeText->GetFont();
    font.SetPointSize(18);
    font.SetWeight(wxFONTWEIGHT_BOLD);
    welcomeText->SetFont(font);

    sizer->AddStretchSpacer(1);
    sizer->Add(welcomeText, 0, wxALIGN_CENTER | wxBOTTOM, 20);
    sizer->Add(new wxStaticText(contentPanel, wxID_ANY, "Use the navigation below to manage your finances."),
               0, wxALIGN_CENTER | wxBOTTOM, 20);
    sizer->AddStretchSpacer(1);

    sizer->Layout();
    contentPanel->Layout();
    contentPanel->Refresh();
}

void DashboardFrame::OnProfileClicked(wxCommandEvent& event) {
    wxBoxSizer* sizer = static_cast<wxBoxSizer*>(contentPanel->GetSizer());
    sizer->Clear(true);

    string userName = db.getUserName(currentNickname.ToStdString());

    sizer->Add(new wxStaticText(contentPanel, wxID_ANY, "Profile"), 0, wxALIGN_CENTER | wxTOP, 20);
    sizer->Add(new wxStaticText(contentPanel, wxID_ANY, "Nickname: " + currentNickname), 0, wxLEFT | wxTOP, 20);
    sizer->Add(new wxStaticText(contentPanel, wxID_ANY, "Name: " + wxString(userName)), 0, wxLEFT | wxTOP, 8);

    wxButton* btnLogout = new wxButton(contentPanel, wxID_ANY, "Logout");
    sizer->Add(btnLogout, 0, wxALIGN_CENTER | wxTOP, 30);

    btnLogout->Bind(wxEVT_BUTTON, &DashboardFrame::OnLogoutClicked, this);

    sizer->Layout();
    contentPanel->Layout();
    contentPanel->Refresh();
}

void DashboardFrame::OnAddSpendingClicked(wxCommandEvent& event) {
    wxDialog dialog(this, wxID_ANY, "Add Spending", wxDefaultPosition, wxSize(420, 340));
    wxBoxSizer* root = new wxBoxSizer(wxVERTICAL);

    wxTextCtrl* categoryInput = new wxTextCtrl(&dialog, wxID_ANY);
    wxFloatingPointValidator<double> amountValidator(2, nullptr, wxNUM_VAL_ZERO_AS_BLANK);
    amountValidator.SetRange(0.01, 100000000.0);
    wxTextCtrl* amountInput = new wxTextCtrl(&dialog, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, amountValidator);
    wxTextCtrl* descriptionInput = new wxTextCtrl(&dialog, wxID_ANY, "", wxDefaultPosition, wxSize(-1, 80), wxTE_MULTILINE);

    root->Add(new wxStaticText(&dialog, wxID_ANY, "Category"), 0, wxLEFT | wxTOP, 12);
    root->Add(categoryInput, 0, wxEXPAND | wxLEFT | wxRIGHT, 12);
    root->Add(new wxStaticText(&dialog, wxID_ANY, "Amount"), 0, wxLEFT | wxTOP, 12);
    root->Add(amountInput, 0, wxEXPAND | wxLEFT | wxRIGHT, 12);
    root->Add(new wxStaticText(&dialog, wxID_ANY, "Description"), 0, wxLEFT | wxTOP, 12);
    root->Add(descriptionInput, 1, wxEXPAND | wxLEFT | wxRIGHT, 12);

    wxStdDialogButtonSizer* buttons = new wxStdDialogButtonSizer();
    buttons->AddButton(new wxButton(&dialog, wxID_OK));
    buttons->AddButton(new wxButton(&dialog, wxID_CANCEL));
    buttons->Realize();
    root->Add(buttons, 0, wxEXPAND | wxALL, 12);

    dialog.SetSizerAndFit(root);

    if (dialog.ShowModal() != wxID_OK) {
        return;
    }

    string category = categoryInput->GetValue().ToStdString();
    string amountRaw = amountInput->GetValue().ToStdString();
    string description = descriptionInput->GetValue().ToStdString();

    if (category.empty() || amountRaw.empty()) {
        wxMessageBox("Category and amount are required.", "Validation", wxOK | wxICON_WARNING);
        return;
    }

    double amount = 0.0;
    try {
        amount = stod(amountRaw);
    } catch (const exception&) {
        wxMessageBox("Amount must be a valid number.", "Validation", wxOK | wxICON_WARNING);
        return;
    }

    if (amount <= 0.0) {
        wxMessageBox("Amount must be greater than 0.", "Validation", wxOK | wxICON_WARNING);
        return;
    }

    if (!db.addSpending(currentNickname.ToStdString(), category, amount, description)) {
        wxMessageBox("Failed to save spending entry.", "Database Error", wxOK | wxICON_ERROR);
        return;
    }

    wxMessageBox("Spending added successfully.", "Success", wxOK | wxICON_INFORMATION);
    wxCommandEvent refreshEvent;
    OnHistoryClicked(refreshEvent);
}

void DashboardFrame::OnHistoryClicked(wxCommandEvent& event) {
    wxBoxSizer* sizer = static_cast<wxBoxSizer*>(contentPanel->GetSizer());
    sizer->Clear(true);

    wxStaticText* title = new wxStaticText(contentPanel, wxID_ANY, "Transaction History");
    wxFont titleFont = title->GetFont();
    titleFont.SetPointSize(14);
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(titleFont);
    sizer->Add(title, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 12);

    wxButton* addButton = new wxButton(contentPanel, wxID_ANY, "Add Spending");
    addButton->Bind(wxEVT_BUTTON, &DashboardFrame::OnAddSpendingClicked, this);
    sizer->Add(addButton, 0, wxALIGN_CENTER | wxBOTTOM, 12);

    vector<SpendingEntry> spendings = db.getSpendings(currentNickname.ToStdString(), 20);
    if (spendings.empty()) {
        sizer->Add(new wxStaticText(contentPanel, wxID_ANY, "No spendings yet. Add your first one."),
                   0, wxALIGN_CENTER | wxTOP, 20);
    } else {
        for (const SpendingEntry& entry : spendings) {
            ostringstream amountStream;
            amountStream << fixed << setprecision(2) << entry.amount;

            wxString row = wxString::Format("%s | %s | $%s", entry.createdAt, entry.category, amountStream.str());
            sizer->Add(new wxStaticText(contentPanel, wxID_ANY, row), 0, wxLEFT | wxRIGHT | wxTOP, 12);

            if (!entry.description.empty()) {
                sizer->Add(new wxStaticText(contentPanel, wxID_ANY, "  " + wxString(entry.description)),
                           0, wxLEFT | wxRIGHT | wxTOP, 12);
            }

            sizer->Add(new wxStaticLine(contentPanel, wxID_ANY), 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
        }
    }

    sizer->Layout();
    contentPanel->Layout();
    contentPanel->Refresh();
}

void DashboardFrame::OnFriendsClicked(wxCommandEvent& event) {
    wxBoxSizer* sizer = static_cast<wxBoxSizer*>(contentPanel->GetSizer());
    sizer->Clear(true);

    wxStaticText* txt = new wxStaticText(contentPanel, wxID_ANY, "Friends");
    wxFont titleFont = txt->GetFont();
    titleFont.SetPointSize(14);
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    txt->SetFont(titleFont);

    sizer->Add(txt, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 20);
    sizer->Add(new wxStaticText(contentPanel, wxID_ANY, "Friends module is ready for extension."),
               0, wxALIGN_CENTER | wxTOP, 8);
    sizer->Add(new wxStaticText(contentPanel, wxID_ANY, "You can now register, login, and manage spendings end-to-end."),
               0, wxALIGN_CENTER | wxTOP, 8);

    sizer->Layout();
    contentPanel->Layout();
    contentPanel->Refresh();
}

void DashboardFrame::OnLogoutClicked(wxCommandEvent& event) {
    if (db.logout()) {
        StartFrame* startFrame = new StartFrame("Start Finance App", db);
        startFrame->Show(true);
        startFrame->Center();

        wxTheApp->SetTopWindow(startFrame);

        Destroy();
    }
}
