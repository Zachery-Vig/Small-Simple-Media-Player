#include "main.h"

char songs[10][100];
char song_dir[100];

int current_song = 0;
int num_songs = 0;

bool playing_song = false;
bool song_paused = false;
bool song_thread_started = false;

ma_uint64 song_total_pcm;
bool follow_song_progress = true;
bool user_updated_progress = false;

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
                num_songs++;
            }
        }
        closedir(d);
    } else {
        std::cout << "ERROR: Invalid Directory '" << songs_dir << "'" << std::endl;
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
    ma_sound_get_length_in_pcm_frames(&sound, &song_total_pcm);
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
        usleep(10000); //Prevents overly high cpu usage and startup crashes
        if (playing_song && !song_paused && ma_sound_at_end(&sound)){ //Checks if song is at end and switches to next song.
            wxCommandEvent null;
            next_song_switch(null);
        }
        if (playing_song){
            ma_sound_set_volume(&sound, MainFrame->volume_slider->GetValue()/100.0); //Sets volume of song based on slider value.
        }
        if (follow_song_progress && ma_sound_get_time_in_pcm_frames(&sound) > 0){ //Checks if song progress bar should be following the progress of the song. (the over 0 check is to prevent jumps from 0 to correct time on user setted progress.)
            MainFrame->song_progress_bar->SetValue(ceil((ma_sound_get_time_in_pcm_frames(&sound)/(float)song_total_pcm) * 100.0));
        }
        else if (user_updated_progress){
            if (song_paused){
                paused_pcm = ceil((MainFrame->song_progress_bar->GetValue()/100.0)*song_total_pcm);
                play_song();
            }
            user_updated_progress = false;
            follow_song_progress = true;
        }
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