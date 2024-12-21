#include "main.h"

char songs[10][100];
char song_dir[100];

int current_song = 0;
int num_songs = 0;

bool playing_song = false;
bool song_paused = false;
bool song_thread_started = false;

void update_song_list(const char songs_dir[100]){
    struct dirent *dir;
    DIR *d = opendir(songs_dir);
    if (d) {
        num_songs = 0;
        while ((dir = readdir(d)) != NULL) {
            char file[100];
            strcpy(file, dir->d_name);
            int name_length = strlen(file);
            if (file[name_length-3] == 'm' && file[name_length-2] == 'p' && file[name_length-1] == '3'){
                char full_dir[100] = "";
                strcat(full_dir, songs_dir);
                strcat(full_dir, "/");
                strcat(full_dir, file);
                strcat(songs[num_songs], full_dir);
                cout << songs[num_songs] << endl;
                num_songs++;
            }
        }
        closedir(d);
    } else {
        cout << "ERROR: Invalid Directory '" << songs_dir << "'" << endl;
    }
}

void play_song(){
    result = ma_sound_init_from_file(&engine, songs[current_song], 0, NULL, NULL, &sound);
    MainFrame->song_name_text->SetLabel("Current Song:\n" + (string)songs[current_song]);
    if (song_paused){
        ma_sound_seek_to_pcm_frame(&sound, paused_pcm);
    }
    playing_song = true;
    song_paused = false;
    ma_sound_start(&sound);
}

void stop_song(){
    ma_sound_stop(&sound);
    ma_sound_uninit(&sound);
}

void pause_song(wxCommandEvent& event){
    if (!song_paused && playing_song){
        song_paused = true;
        paused_pcm = ma_sound_get_time_in_pcm_frames(&sound);
        stop_song();
    }
}

void song_toggle(wxCommandEvent& event){
    playing_song = !playing_song;
    if (playing_song || song_paused){
        play_song();
    } else{
        playing_song = false;
        stop_song();
    }
}

//Function that runs on seperate thread and checks and responds to UI/Sound Engine events.
void status_check_thread(){
    while (true){
        if (playing_song && !song_paused && ma_sound_at_end(&sound)){
            wxCommandEvent null;
            next_song_switch(null);
        }
        ma_sound_set_volume(&sound, MainFrame->volume_slider->GetValue()/100.0);
        usleep(10000);
    }
}

void next_song_switch(wxCommandEvent& event){
    if (playing_song && !song_paused){
        playing_song = false;
        stop_song();
        current_song++;
        if (current_song == num_songs){ current_song = 0; }
        play_song();
    }
}

void prev_song_switch(wxCommandEvent& event){
    if (playing_song && !song_paused){
        playing_song = false;
        stop_song();
        current_song--;
        if (current_song < 0){ current_song = num_songs-1; }
        play_song();
    }
}