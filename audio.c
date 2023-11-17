#include "audio.h"
#include "log.h"

#define MIXER_INIT_FLAGS	(MIX_INIT_FLAC | MIX_INIT_MP3 | MIX_INIT_OGG)

int audio_init()
{
	if(SDL_Init(SDL_INIT_AUDIO) == -1){
		LOG_ERROR("Cannot init SDL audio subsystem");
		return AUDIO_ERROR_CANT_INIT_SDL;
	}
	if(Mix_Init(MIXER_INIT_FLAGS) != MIXER_INIT_FLAGS){
		LOG_ERROR("Cannot init SDL mixer decoders");
		return AUDIO_ERROR_CANT_INIT_MIXER;
	}
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1){
		LOG_ERROR("Cannot open SDL mixer channel");
		return AUDIO_ERROR_CANT_OPEN_MIXER;
	}
	return 0;
}
