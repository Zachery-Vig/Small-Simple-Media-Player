#include <iostream>
#include <dirent.h>
#include <string.h>
#include <math.h>
#include <thread>
#include <wx-3.2/wx/wx.h>
#include <wx-3.2/wx/event.h>
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
    wxSlider* volume_slider;
    wxSlider* song_progress_bar;
};

class App : public wxApp
{
    public:
    bool OnInit();
};

Main_Frame* MainFrame;

void update_song_list(const char songs_dir[100]);

void play_song();

void song_toggle(wxCommandEvent& event);

void pause_song(wxCommandEvent& event);

void status_check_thread();

void next_song_switch(wxCommandEvent& event);

void prev_song_switch(wxCommandEvent& event);

void test_func(wxCommandEvent& event);