#include "song_manager.cpp"

Input_Window::Input_Window(wxWindow* parent) : wxPopupTransientWindow(parent, wxBORDER_SIMPLE) {
    wxPanel* popup_panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(400, 200));

    wxStaticText* enter_dir_text = new wxStaticText(popup_panel, wxID_ANY, "Enter Songs Directory", wxPoint(0, 10), wxSize(400, 100), wxALIGN_CENTRE_HORIZONTAL);
    enter_dir_text->SetFont(wxFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    enter_dir_text->SetForegroundColour(wxColor(255, 255, 255));

    song_dir_input = new wxTextCtrl(popup_panel, wxID_ANY, "", wxPoint(50, 75), wxSize(300, 20));
    song_dir_input->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    song_dir_input->SetFocus();

    wxButton* button_close = new wxButton(popup_panel, wxID_ANY, "Cancel", wxPoint(120,110), wxSize(80,40));
    wxButton* button_submit = new wxButton(popup_panel, wxID_ANY, "Enter", wxPoint(210,110), wxSize(80,40));

    button_submit->Bind(wxEVT_BUTTON, &Input_Window::submit_song_dir, this);
    button_close->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) {
            song_dir_window->Destroy();
            MainFrame->Destroy();
       });

    error_text = new wxStaticText(popup_panel, wxID_ANY, "", wxPoint(0, 150), wxSize(400, 100), wxALIGN_CENTRE_HORIZONTAL);
    error_text->SetFont(wxFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    error_text->SetForegroundColour(wxColor(255, 0, 0));
}

void Input_Window::submit_song_dir(wxCommandEvent& event){
    int dir_status = update_song_list(song_dir_input->GetValue());
    if (dir_status == -1){
        this->error_text->SetLabel("Directory Doesn't Exist!");
    }
    else if (dir_status == -2){
        this->error_text->SetLabel("Directory Doesn't Contain Any Songs!");
    } else {
        MainFrame->song_name_text->SetLabel("Current Song:\n" + (string)songs[0]);
        song_dir_window->Destroy();
    }
}

Main_Frame::Main_Frame(const wxString& title): wxFrame(nullptr, wxID_ANY, title) {
    SetBackgroundColour(wxColour(21, 56, 77));
    
    wxPanel* panel = new wxPanel(this);
    
    song_dir_window = new Input_Window(this);
    song_dir_window->Position(wxPoint(1320+200,400+150), wxSize(0,0));
    song_dir_window->SetClientSize(400,200);
    song_dir_window->Show();

    //Sets up control buttons (Play Button initilized in header as its label is edited by backend)
    wxButton* button_left = new wxButton(panel, wxID_ANY, "<-", wxPoint(50,400), wxSize(80,40));
    wxButton* button_stop = new wxButton(panel, wxID_ANY, "Stop", wxPoint(240,400), wxSize(80,40));
    button_play = new wxButton(panel, wxID_ANY, "Play", wxPoint(420,400), wxSize(80,40));
    wxButton* button_right = new wxButton(panel, wxID_ANY, "->", wxPoint(650,400), wxSize(80,40));
    
    button_play->SetBackgroundColour(wxColor(0, 36, 94));
    button_left->SetBackgroundColour(wxColor(0, 36, 94));
    button_right->SetBackgroundColour(wxColor(0, 36, 94));
    button_stop->SetBackgroundColour(wxColor(0, 36, 94));
    
    wxStaticText* title_text = new wxStaticText(panel, wxID_ANY, "Small Simple Song Player", wxPoint(0, 20), wxSize(800, 100), wxALIGN_CENTRE_HORIZONTAL);
    title_text->SetFont(wxFont(42, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    title_text->SetForegroundColour(wxColor(24, 123, 181));
    
    song_name_text = new wxStaticText(panel, wxID_ANY, "Current Song:\nNone", wxPoint(0, 120), wxSize(800, 120), wxALIGN_CENTRE_HORIZONTAL);
    song_name_text->SetFont(wxFont(24, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    song_name_text->SetForegroundColour(wxColor(200, 200, 200));

    wxStaticText* volume_title_text = new wxStaticText(panel, wxID_ANY, "Volume:", wxPoint(0, 240), wxSize(800, 100), wxALIGN_CENTRE_HORIZONTAL);
    volume_title_text->SetFont(wxFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    volume_title_text->SetForegroundColour(wxColor(255, 255, 255));

    volume_slider = new wxSlider(panel, wxID_ANY, 100, 0, 100, wxPoint(200, 270), wxSize(400, 50), wxSL_HORIZONTAL);

    song_progress_bar = new wxSlider(panel, wxID_ANY, 0, 0, 100, wxPoint(50, 340), wxSize(700, 50), wxSL_HORIZONTAL);

    //Binds Buttons to backend functions (some go to specific call functions to avoid backend needing to supply wxCommandEvent when used outside button press)
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

    //Timer used for song end check, setting volume, etc. Originally I tried to do this through multi-threading but due too wxwidgets lack of thread safety (and me being not good at implementing multi-threading) I got a lot of seg faults.
    timer = new wxTimer(this);
    timer->Start(30);
    Bind(wxEVT_TIMER, &status_check);
}

bool App::OnInit() {
    result = ma_engine_init(NULL, &engine);

    MainFrame = new Main_Frame("Small Simple Song Player");
    MainFrame->SetClientSize(800,600);
    MainFrame->Center();
    MainFrame->Show();
    return true;
}

wxIMPLEMENT_APP(App);