#include "song_manager.cpp"

Main_Frame::Main_Frame(const wxString& title): wxFrame(nullptr, wxID_ANY, title) {
    SetBackgroundColour(wxColour(99,99,99));
    wxPanel* panel = new wxPanel(this);
    wxButton* button_left = new wxButton(panel, wxID_ANY, "Previous Song", wxPoint(50,350), wxSize(140,40));
    wxButton* button_right = new wxButton(panel, wxID_ANY, "Next Song", wxPoint(650,350), wxSize(140,40));
    wxButton* button_play = new wxButton(panel, wxID_ANY, "Play/Stop", wxPoint(400,350), wxSize(140,40));
    wxButton* button_pause = new wxButton(panel, wxID_ANY, "Pause", wxPoint(250,350), wxSize(140,40));

    wxStaticText* title_text = new wxStaticText(panel, wxID_ANY, "Small Simple Song Player", wxPoint(0, 20), wxSize(800, 100), wxALIGN_CENTRE_HORIZONTAL);
    title_text->SetFont(wxFont(24, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    title_text->SetForegroundColour(*wxRED);
    
    song_name_text = new wxStaticText(panel, wxID_ANY, "Current Song: ", wxPoint(0, 80), wxSize(800, 120), wxALIGN_CENTRE_HORIZONTAL);
    song_name_text->SetFont(wxFont(24, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

    button_play->Bind(wxEVT_BUTTON, &song_toggle);
    button_pause->Bind(wxEVT_BUTTON, &pause_song);

}

bool App::OnInit() {
    result = ma_engine_init(NULL, &engine);
    thread song_end_thread = thread(song_end_check);
    song_end_thread.detach();
    cout << "Enter Song Directory: ";
    cin >> song_dir;
    update_song_list(song_dir);
    MainFrame = new Main_Frame("Small Simple Song Player");
    MainFrame->SetClientSize(800,600);
    MainFrame->Center();
    MainFrame->Show();
    return true;
}

wxIMPLEMENT_APP(App);