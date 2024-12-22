#include "song_manager.cpp"

Main_Frame::Main_Frame(const wxString& title): wxFrame(nullptr, wxID_ANY, title) {
    SetBackgroundColour(wxColour(21, 56, 77));
    wxPanel* panel = new wxPanel(this);
    wxButton* button_left = new wxButton(panel, wxID_ANY, "Previous Song", wxPoint(50,350), wxSize(140,40));
    wxButton* button_pause = new wxButton(panel, wxID_ANY, "Pause", wxPoint(240,350), wxSize(140,40));
    wxButton* button_play = new wxButton(panel, wxID_ANY, "Play/Stop", wxPoint(420,350), wxSize(140,40));
    wxButton* button_right = new wxButton(panel, wxID_ANY, "Next Song", wxPoint(610,350), wxSize(140,40));
    //button_left->SetBackgroundColour(wxColor(0, 145, 235));

    wxStaticText* title_text = new wxStaticText(panel, wxID_ANY, "Small Simple Song Player", wxPoint(0, 20), wxSize(800, 100), wxALIGN_CENTRE_HORIZONTAL);
    title_text->SetFont(wxFont(42, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    title_text->SetForegroundColour(wxColor(24, 123, 181));
    
    song_name_text = new wxStaticText(panel, wxID_ANY, "Current Song:\nNone Playing", wxPoint(0, 120), wxSize(800, 120), wxALIGN_CENTRE_HORIZONTAL);
    song_name_text->SetFont(wxFont(24, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    song_name_text->SetForegroundColour(wxColor(200, 200, 200));

    wxStaticText* volume_title_text = new wxStaticText(panel, wxID_ANY, "Volume:", wxPoint(0, 240), wxSize(800, 100), wxALIGN_CENTRE_HORIZONTAL);
    volume_title_text->SetFont(wxFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    volume_title_text->SetForegroundColour(wxColor(255, 255, 255));

    volume_slider = new wxSlider(panel, wxID_ANY, 100, 0, 100, wxPoint(250, 270), wxSize(300, 50), wxSL_HORIZONTAL);

    song_progress_bar = new wxSlider(panel, wxID_ANY, 0, 0, 100, wxPoint(100, 300), wxSize(600, 50), wxSL_HORIZONTAL);

    button_play->Bind(wxEVT_BUTTON, &song_toggle);
    button_pause->Bind(wxEVT_BUTTON, &pause_song);
    button_right->Bind(wxEVT_BUTTON, &next_song_switch);
    button_left->Bind(wxEVT_BUTTON, &prev_song_switch);

    //Tells Backend when user tries to manually update progress bar and to not set automatically.
    song_progress_bar->Bind(wxEVT_COMMAND_SLIDER_UPDATED, [this](wxCommandEvent& event) {
            if (follow_song_progress){
                follow_song_progress = false;
                wxCommandEvent null;
                pause_song(null);
            }
       });

    //Tells Backend user is done manually updating progress bar and to set song time to what user has specified.
    song_progress_bar->Bind(wxEVT_LEFT_UP, [this](wxMouseEvent& event) {
        if (!follow_song_progress){
            user_updated_progress = true;
        }
       });

    std::thread sc_thread(status_check_thread);
    sc_thread.detach();
}

bool App::OnInit() {
    result = ma_engine_init(NULL, &engine);

    std::cout << "Enter Song Directory: ";
    std::cin >> song_dir;
    update_song_list(song_dir);

    MainFrame = new Main_Frame("Small Simple Song Player");
    MainFrame->SetClientSize(800,600);
    MainFrame->Center();
    MainFrame->Show();
    return true;
}

wxIMPLEMENT_APP(App);