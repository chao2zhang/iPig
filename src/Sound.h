#pragma once

#include "Base.h"

class BaseMixSound
{
private:
    static int MixNUM;
protected:
    BaseMixSound();
public:
    virtual ~BaseMixSound();
};

class EffectSound: public BaseMixSound
{
private:
    Mix_Chunk* sound;
public:
	EffectSound();
    EffectSound(const std::string& sound_fileName);
    void play() const;
};

class MusicSound: public BaseMixSound
{
private:
	bool ing;
    Mix_Music* music;
public:
	MusicSound();
    MusicSound(const std::string& music_fileName);
    void play();
	bool playing() const;
    void stop();
};