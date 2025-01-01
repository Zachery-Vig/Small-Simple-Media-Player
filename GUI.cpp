#include "song_manager.cpp"

Main_Frame::Main_Frame(const wxString& title): wxFrame(nullptr, wxID_ANY, title) {
    SetBackgroundColour(wxColour(21, 56, 77));
    
    wxPanel* panel = new wxPanel(this);
    wxButton* button_left = new wxButton(panel, wxID_ANY, "<<<", wxPoint(50,400), wxSize(80,40));
    wxButton* button_stop = new wxButton(panel, wxID_ANY, "Stop", wxPoint(240,400), wxSize(80,40));
    button_play = new wxButton(panel, wxID_ANY, "Play", wxPoint(420,400), wxSize(80,40));
    wxButton* button_right = new wxButton(panel, wxID_ANY, ">>>", wxPoint(650,400), wxSize(80,40));
    button_play->SetBackgroundColour(wxColor(0, 36, 94));
    button_left->SetBackgroundColour(wxColor(0, 36, 94));
    button_right->SetBackgroundColour(wxColor(0, 36, 94));
    button_stop->SetBackgroundColour(wxColor(0, 36, 94));
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

    song_progress_bar = new wxSlider(panel, wxID_ANY, 0, 0, 100, wxPoint(100, 340), wxSize(600, 50), wxSL_HORIZONTAL);

    button_play->Bind(wxEVT_BUTTON, &song_toggle);
    button_stop->Bind(wxEVT_BUTTON, &stop_song_call);
    button_right->Bind(wxEVT_BUTTON, &next_song_switch_call);
    button_left->Bind(wxEVT_BUTTON, &prev_song_switch);

    //Tells Backend when user tries to manually update progress bar and to not set automatically.
    song_progress_bar->Bind(wxEVT_COMMAND_SLIDER_UPDATED, [this](wxCommandEvent& event) {
            if (follow_song_progress){
                follow_song_progress = false;
                pause_song();
            }
       });

    //Tells Backend user is done manually updating progress bar and to set song time to what user has specified.
    song_progress_bar->Bind(wxEVT_LEFT_UP, [this](wxMouseEvent& event) {
        if (!follow_song_progress){
            user_updated_progress = true;
        }
       });

    //Timer used for song end check, setting volume, etc (Tried to implement multi-threading but due to wxwidgets lack of thread safety this was simpilar to implement.)
    timer = new wxTimer(this);
    timer->Start(30);
    Bind(wxEVT_TIMER, &status_check);
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