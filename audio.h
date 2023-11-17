#ifndef AUDIO_H
#define AUDIO_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"

#define AUDIO_ERROR_CANT_INIT_SDL	-1
#define AUDIO_ERROR_CANT_INIT_MIXER	-2
#define AUDIO_ERROR_CANT_OPEN_MIXER	-3

typedef Mix_Chunk* sound;
typedef Mix_Music* music;

int audio_init();

#define audio_load_sound(fname) (Mix_LoadWAV(fname))
#define audio_load_music(fname) (Mix_LoadMusic(fname))

#define audio_play_sound(snd) (Mix_PlayChannel(-1, (snd), 0))
#define audio_play_music(music) (Mix_PlayMusic((music), -1))
#define audio_stop_music() (Mix_HaltMusic())

#endif
