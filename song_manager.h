#include "main.h"
#include "GUI.h"

ma_sound sound;
ma_engine engine;
ma_result result = ma_engine_init(NULL, &engine);

void update_song_list(const char songs_dir[100]);

void play_song(wxCommandEvent& event);

void start_song_thread(wxCommandEvent& event);

void stop_song(wxCommandEvent& event);

void pause_song(wxCommandEvent& event);

void next_song_switch(wxCommandEvent& event);

void prev_song_switch(wxCommandEvent& event);
