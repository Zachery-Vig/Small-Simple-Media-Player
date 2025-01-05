#include "main.h"

//Scans user specified directory for songs and adds their paths to the songs array. Returns status of directory read, -1 for directory not existing, -2 for directory for no songs in directory, and 0 for success.
int update_song_list(){
    std::cout << songs_dir << std::endl;
    struct dirent *dir;
    DIR *d = opendir(songs_dir);
    if (d) {
        num_songs = 0;
        while ((dir = readdir(d)) != NULL) {
            char file[100];
            strcpy(file, dir->d_name);
            //Gets the format of the file.
            int name_len = strlen(file);
            string format = "000";
            for (int i = name_len-3; i < name_len; i++){ 
                format[i-(name_len-3)] = std::tolower(file[i]);
                std::cout << file[i] << std::endl;
            }

            if (format == "mp3" || format == "wav"){
                strcat(songs[num_songs], file);
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
    if ((current_song+1)==num_songs){
        MainFrame->next_song_text->SetLabel("Next Song: " + (string)songs[0]);
    } else {
        MainFrame->next_song_text->SetLabel("Next Song: " + (string)songs[current_song+1]);
    }
    MainFrame->button_play->SetLabel("Pause");
    //Takes the songs directory and the current song and creates a full path for the song to give to miniaudio.
    char full_song_dir[200];
    string full_song_dir_str = ((string)songs_dir + "/" + (string)songs[current_song]);
    strcpy(full_song_dir, full_song_dir_str.c_str());

    result = ma_sound_init_from_file(&engine, full_song_dir, 0, NULL, NULL, &sound);
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
        MainFrame->song_progress_bar->SetValue(ceil((ma_sound_get_time_in_pcm_frames(&sound)/(float)song_total_pcm) * 100.0));
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