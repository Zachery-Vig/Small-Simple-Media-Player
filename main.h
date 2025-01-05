#include <iostream>
#include <dirent.h>
#include <string.h>
#include <math.h>
#include <thread>
#include <wx-3.2/wx/popupwin.h>
#include <wx-3.2/wx/wx.h>
#include <wx-3.2/wx/event.h>
#include <wx-3.2/wx/timer.h>
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

using std::string;

ma_result result;
ma_sound sound;
ma_engine engine;

ma_uint64 paused_pcm;

class Main_Frame : public wxFrame{
    public:
    Main_Frame(const wxString& title);
    wxStaticText* song_name_text;
    wxStaticText* next_song_text;
    wxSlider* volume_slider;
    wxSlider* song_progress_bar;
    wxButton* button_play;
    wxTimer* timer;
};

class App : public wxApp
{
    public:
    bool OnInit();
};

class Input_Window : public wxPopupTransientWindow {
    public:
    wxTextCtrl* song_dir_input;
    wxStaticText* error_text;
    void submit_song_dir(wxCommandEvent& event);
    Input_Window(wxWindow* parent);
};

Main_Frame* MainFrame;
Input_Window* song_dir_window;

char songs[50][100];
char songs_dir[150];
int num_songs = 0;
int current_song = 0;

//Main status variables of current song to check if song is playing (true,false), if song is stopped (false,false), and if song is paused (true,true) 
bool playing_song = false;
bool song_paused = false;

ma_uint64 song_total_pcm = 100000000;
bool follow_song_progress = true;
bool user_updated_progress = false;

bool song_toggle_status = true;