#include "../include/frame_dashboard.hpp"
#include "../include/mysql_connection.hpp"
#include "../include/frame_start.hpp"
#include <wx/dcbuffer.h>
#include <wx/wx.h>
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;

DashboardFrame::DashboardFrame(const wxString& title, const wxString& name, MySQLConnection& dbRef)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)), db(dbRef){
    monthlyExpenses = {
        {"Jan", 420.50},
        {"Feb", 560.20},
        {"Mar", 390.40},
        {"Apr", 615.90},
        {"May", 710.10},
        {"Jun", 530.00}
    };
    
    panel = new wxPanel(this);
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    contentPanel = new wxPanel(panel, wxID_ANY);
    wxBoxSizer* contentSizer = new wxBoxSizer(wxVERTICAL);
    contentPanel->SetSizer(contentSizer);

    wxStaticText* welcomeText = new wxStaticText(contentPanel, wxID_ANY, "Hello " + name + "!");
    wxFont font = welcomeText->GetFont();
    font.SetPointSize(18);
    font.SetWeight(wxFONTWEIGHT_BOLD);
    welcomeText->SetFont(font);
    contentSizer->Add(welcomeText, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 30);

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
}

void DashboardFrame::OnClose(wxCloseEvent& evt) {
    Destroy();
    wxTheApp->ExitMainLoop();
}

DashboardFrame::~DashboardFrame(){}

void DashboardFrame::OnProfileClicked(wxCommandEvent& event) {
    wxBoxSizer* sizer = (wxBoxSizer*)contentPanel->GetSizer();
    sizer->Clear(true);

    wxStaticText* txt = new wxStaticText(contentPanel, wxID_ANY, "Profile View");
    sizer->Add(txt, 0, wxALIGN_CENTER | wxTOP, 20);

    wxButton* btnLogout = new wxButton(contentPanel, wxID_ANY, "Logout");
    sizer->Add(btnLogout, 0, wxALIGN_CENTER | wxTOP, 20);

    btnLogout->Bind(wxEVT_BUTTON, &DashboardFrame::OnLogoutClicked, this);

    sizer->Layout();
    contentPanel->Layout();
    contentPanel->Refresh();
}

void DashboardFrame::OnHistoryClicked(wxCommandEvent& event) {
    wxBoxSizer* sizer = (wxBoxSizer*)contentPanel->GetSizer();
    sizer->Clear(true);

    wxStaticText* txt = new wxStaticText(contentPanel, wxID_ANY, "Expense Usage Diagram");
    wxFont headingFont = txt->GetFont();
    headingFont.SetPointSize(14);
    headingFont.SetWeight(wxFONTWEIGHT_BOLD);
    txt->SetFont(headingFont);
    sizer->Add(txt, 0, wxALIGN_CENTER | wxTOP, 20);
    sizer->Add(new wxStaticText(contentPanel, wxID_ANY, "Monthly spending overview"), 0, wxALIGN_CENTER | wxTOP, 8);

    wxPanel* chartPanel = CreateExpensesChart(contentPanel);
    sizer->Add(chartPanel, 1, wxEXPAND | wxALL, 25);

    sizer->Layout();
    contentPanel->Layout();
    contentPanel->Refresh();
}

void DashboardFrame::OnFriendsClicked(wxCommandEvent& event) {
    wxBoxSizer* sizer = (wxBoxSizer*)contentPanel->GetSizer();
    sizer->Clear(true);
    
    wxStaticText* txt = new wxStaticText(contentPanel, wxID_ANY, "Friends List");
    sizer->Add(txt, 0, wxALIGN_CENTER | wxTOP, 20);

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

wxPanel* DashboardFrame::CreateExpensesChart(wxWindow* parent) {
    wxPanel* chartPanel = new wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, 280));
    chartPanel->SetBackgroundStyle(wxBG_STYLE_PAINT);
    chartPanel->Bind(wxEVT_PAINT, &DashboardFrame::DrawExpensesChart, this);
    return chartPanel;
}

void DashboardFrame::DrawExpensesChart(wxPaintEvent& event) {
    wxPanel* chartPanel = static_cast<wxPanel*>(event.GetEventObject());
    wxAutoBufferedPaintDC dc(chartPanel);
    dc.Clear();

    const wxSize size = chartPanel->GetClientSize();
    if (size.GetWidth() <= 120 || size.GetHeight() <= 120 || monthlyExpenses.empty()) {
        return;
    }

    const int leftMargin = 55;
    const int rightMargin = 20;
    const int topMargin = 30;
    const int bottomMargin = 45;

    const int chartWidth = size.GetWidth() - leftMargin - rightMargin;
    const int chartHeight = size.GetHeight() - topMargin - bottomMargin;
    const int xAxisY = topMargin + chartHeight;

    double maxExpense = 0.0;
    for (const auto& [_, value] : monthlyExpenses) {
        maxExpense = std::max(maxExpense, value);
    }
    if (maxExpense <= 0) {
        return;
    }

    dc.SetPen(wxPen(wxColour(120, 120, 120), 2));
    dc.DrawLine(leftMargin, topMargin, leftMargin, xAxisY);
    dc.DrawLine(leftMargin, xAxisY, leftMargin + chartWidth, xAxisY);

    dc.SetPen(wxPen(wxColour(225, 225, 225), 1));
    dc.SetTextForeground(wxColour(90, 90, 90));
    for (int i = 1; i <= 4; ++i) {
        int y = xAxisY - (chartHeight * i / 4);
        dc.DrawLine(leftMargin, y, leftMargin + chartWidth, y);
        double value = maxExpense * i / 4.0;
        dc.DrawText(wxString::Format("$%.0f", value), 8, y - 8);
    }

    const int barCount = static_cast<int>(monthlyExpenses.size());
    const int segmentWidth = chartWidth / barCount;
    const int barWidth = std::max(18, segmentWidth - 20);

    for (int i = 0; i < barCount; ++i) {
        const auto& [label, value] = monthlyExpenses[i];
        const int barHeight = static_cast<int>((value / maxExpense) * chartHeight);
        const int barX = leftMargin + (i * segmentWidth) + (segmentWidth - barWidth) / 2;
        const int barY = xAxisY - barHeight;

        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(wxBrush(wxColour(74, 144, 226)));
        dc.DrawRoundedRectangle(barX, barY, barWidth, barHeight, 4);

        dc.SetTextForeground(wxColour(50, 50, 50));
        dc.DrawText(label, barX + 4, xAxisY + 8);
        dc.SetTextForeground(wxColour(30, 30, 30));
        dc.DrawText(wxString::Format("$%.0f", value), barX, barY - 18);
    }
}
