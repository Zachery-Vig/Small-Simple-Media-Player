#include "main.h"

//Scans user specified directory for songs and adds their paths to the songs array. Returns status of directory read, -1 for directory not existing, -2 for directory for no songs in directory, and 0 for success.
int update_song_list(const char songs_dir[100]){
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
        if (num_songs == 0){
            return -2;
        } else {
            return 0;
        }
    } else {
        return -1;
    }
}

void play_song(){
    MainFrame->song_name_text->SetLabel("Current Song:\n" + (string)songs[current_song]);
    MainFrame->button_play->SetLabel("Pause");
    result = ma_sound_init_from_file(&engine, songs[current_song], 0, NULL, NULL, &sound);
    if (song_paused){
        ma_sound_seek_to_pcm_frame(&sound, paused_pcm);
    }
    playing_song = true;
    song_paused = false;
    ma_sound_start(&sound);
    ma_sound_get_length_in_pcm_frames(&sound, &song_total_pcm);
}

void stop_song(){
    if (!song_paused && playing_song){
        MainFrame->song_progress_bar->SetValue(0);
        MainFrame->button_play->SetLabel("Play");
        playing_song = false;
        ma_sound_stop(&sound);
        ma_sound_uninit(&sound);
    }
}

void stop_song_call(wxCommandEvent& event){
    stop_song();
}

void pause_song(){
    if (!song_paused && playing_song){
        MainFrame->button_play->SetLabel("Play");
        song_paused = true;
        paused_pcm = ma_sound_get_time_in_pcm_frames(&sound);
        ma_sound_stop(&sound);
        ma_sound_uninit(&sound);
    }
}

void song_toggle(wxCommandEvent& event){
    if (!playing_song){ song_toggle_status = true; }
    else { song_toggle_status = !song_toggle_status; }
    if (song_toggle_status){
        play_song();
    } else {
        song_paused = false;
        pause_song();
    }
}

void next_song_switch(){
    stop_song();
    current_song++;
    if (current_song == num_songs){ current_song = 0; }
    play_song();
}

void next_song_switch_call(wxCommandEvent& event){
    next_song_switch();
}

void prev_song_switch(wxCommandEvent& event){
    stop_song();
    current_song--;
    if (current_song < 0){ current_song = num_songs-1; }
    play_song();
}

void reset_playlist(wxCommandEvent& event){
    stop_song();
    current_song = 0;
    play_song();
}

void status_check(wxTimerEvent& event){
    if (playing_song && !song_paused && ma_sound_at_end(&sound)){ //Checks if song is at end and switches to next song.
        next_song_switch();
    }
    if (playing_song){
        ma_sound_set_volume(&sound, MainFrame->volume_slider->GetValue()/100.0); //Sets volume of song based on slider value.
    }
    if (follow_song_progress && ma_sound_get_time_in_pcm_frames(&sound) > 0 && playing_song){ //Checks if song progress bar should be following the progress of the song. (the over 0 check is to prevent jumps from 0 to correct time on user setted progress.)
        MainFrame->song_progress_bar->SetValue(floor((ma_sound_get_time_in_pcm_frames(&sound)/(float)song_total_pcm) * 100.0));
    }
    else if (user_updated_progress){
        if (song_paused || !playing_song){
            paused_pcm = ((MainFrame->song_progress_bar->GetValue()/100.0)*song_total_pcm);
            play_song();
        }
        song_toggle_status = true;
        user_updated_progress = false;
        follow_song_progress = true;
    }
}