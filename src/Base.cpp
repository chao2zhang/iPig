#include "Base.h"


int ScreenSurface::number = 0;

ScreenSurface::ScreenSurface(int vwidth, int vheight, int vbpp, Uint32 vflags, const std::string& vname, const std::string& vicon)
	:width(vwidth), height(vheight), bpp(vbpp), flags(vflags), name(vname)
{
	if (number++ > 0)
		throw "Can't create more than one screen";
	if (SDL_Init(SDL_INIT_EVERYTHING))
		throw SDL_GetError();
	if ((pScreen = SDL_SetVideoMode(width, height, bpp, flags)) == 0)
		throw SDL_GetError();
	SDL_WM_SetCaption(name.c_str(), 0);

}

ScreenSurface::~ScreenSurface()
{
	--number;
	if (pScreen != 0)
		SDL_FreeSurface(pScreen);
	SDL_Quit();
}

void ScreenSurface::flip() const
{
	if (SDL_Flip(pScreen) == -1)
		throw SDL_GetError();
}

SDL_Surface* ScreenSurface::point() const
{
	return pScreen;
}

RGBSurface::RGBSurface()
	:width(0), height(0), bpp(0),
	 alpha(SDL_ALPHA_OPAQUE), pScreen(0),
	 pSurface(0)
{
}

RGBSurface::RGBSurface(const ScreenSurface& screen, int vwidth, int vheight, int vbpp)
	:width(vwidth), height(vheight), bpp(vbpp),
	 alpha(SDL_ALPHA_TRANSPARENT), pScreen(screen.point())
{
	if ((pSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, bpp, 0, 0, 0, 0)) == 0)
	{
		throw SDL_GetError();
	}
}

void RGBSurface::show()
{
	if (SDL_UpperBlit(pSurface, 0, pScreen, 0) == -1)
		throw SDL_GetError();
}

void RGBSurface::setAlpha(Uint8 a)
{
	alpha = a;
	if (SDL_SetAlpha(pSurface, SDL_SRCALPHA, alpha) == -1)
		throw SDL_GetError();
}

Uint8 RGBSurface::getAlpha() const
{
	return alpha;
}

StaticImage::StaticImage()
	:name(""), pScreen(0), pSurface(0), pStatus(0), posX(0), posY(0)
{
}
StaticImage::StaticImage
			(const std::string& vname, const ScreenSurface& screen,
			 SDL_Rect* vpStatus, double vposX, double vposY)
	:name(vname), pScreen(screen.point()),
	 pStatus(vpStatus), posX(vposX), posY(vposY)
{
	pSurface = 0;
	SDL_Surface* pTemp;
	if ((pTemp = IMG_Load(name.c_str())) == 0)
		throw SDL_GetError();
	if ((pSurface = SDL_DisplayFormatAlpha(pTemp)) == 0)
	{
		throw SDL_GetError();
	}
	if (pTemp != 0)
		SDL_FreeSurface(pTemp);
}

void StaticImage::show() const
{
	SDL_Rect pTmp;
	pTmp.x = round(posX);
	pTmp.y = round(posY);
	pTmp.w = 0;
	pTmp.h = 0;
	if (SDL_UpperBlit(pSurface, pStatus, pScreen, &pTmp) == -1)
		throw SDL_GetError();
}

