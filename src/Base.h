#pragma once

#include "EXTERN_SDL.h"
#include <iostream>
#include <string>
#include <cmath>
#include <cctype>
#include <algorithm>

inline int round(double V)
{
	return int(V + 0.5);
}

const double EPS = 1e-6;
const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 280;
const int SCREEN_BPP = 32;
const int SCREEN_OUTSIDE = 500;
const int GAME_W = 320;
const int GAME_H = 240;
const int GAME_X = 0;
const int GAME_Y = 0;

enum STATUS 
{
	WALK_RIGHT_STATUS, 
	WALK_LEFT_STATUS,
	FLY_RIGHT_STATUS,
	FLY_LEFT_STATUS,
	HIT_RIGHT_STATUS,
	HIT_LEFT_STATUS
};

class ScreenSurface
{
private:
	static int number;
	int width,height,bpp;
	std::string name;
	std::string icon;
	Uint32 flags;
	SDL_Surface* pScreen;
public:
	ScreenSurface(int vwidth, int vheight, int vbpp, Uint32 vflags = 0U, const std::string& vname = "", const std::string& vicon = "");
	~ScreenSurface();
	void flip() const;
	SDL_Surface* point() const;
};

class RGBSurface
{
private:
	int width, height, bpp;
	Uint8 alpha;
	SDL_Surface* pScreen;
	SDL_Surface* pSurface;
public:
	RGBSurface();
	RGBSurface(const ScreenSurface& screen, int width, int height, int bpp);
	void show();
	void setAlpha(Uint8 alpha);
	Uint8 getAlpha() const;
};


class StaticImage
{
protected:
	std::string name;
	SDL_Surface* pSurface;
	SDL_Surface* pScreen;
	SDL_Rect* pStatus;
	double posX, posY;
public:
	StaticImage();
	StaticImage(const std::string& name, const ScreenSurface& screen,
				 SDL_Rect* pStatus = 0, double posX = 0.0, double posY = 0.0);
	virtual void show() const;
};

