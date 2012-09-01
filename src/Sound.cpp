#include "Sound.h"

int BaseMixSound::MixNUM = 0;

BaseMixSound::BaseMixSound() {
    if ( MixNUM == 0 )
        if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 1024 ) == -1 )
			throw SDL_GetError();
    MixNUM++;
}

BaseMixSound::~BaseMixSound() {
    MixNUM--;
    if ( MixNUM == 0 )
        Mix_CloseAudio();
}

EffectSound::EffectSound()
	:sound(0) {}

EffectSound::EffectSound(const std::string& sound_fileName) {
    sound = Mix_LoadWAV(sound_fileName.c_str());
    if ( sound == 0 )
		 throw SDL_GetError();
}

void EffectSound::play() const {
     if( Mix_PlayChannel(-1, sound, 0) == -1 )
        throw SDL_GetError();
}

MusicSound::MusicSound()
	:music(0), ing(false) {}

MusicSound::MusicSound(const std::string& music_fileName)
	:ing(false) {
    music = Mix_LoadMUS(music_fileName.c_str());
    if ( music == 0 )
		throw SDL_GetError();
}

void MusicSound::play() {
    if(!Mix_PlayingMusic()) {
		ing = true;
        if( Mix_PlayMusic( music, -1 ) == -1 )
		  throw SDL_GetError();
    }
    else  {
        if (Mix_PausedMusic()) 
            Mix_ResumeMusic();
        else 
            Mix_PauseMusic();
    }
}

bool MusicSound::playing() const {
	return ing;
}

void MusicSound::stop() {
	ing = false;
    Mix_HaltMusic();
}
