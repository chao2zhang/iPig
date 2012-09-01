#pragma once

#include "Base.h"

const int DELTA_T = 1;
const int TOTAL_0_to_9 = 10;
const int TOTAL_ALPHABET = 26;
const int MAX_NAME_LENGTH = 10;
const int WIDTH_SCREEN_BORDER = 20;

const int TIMES_VANISH = 20;//该次数后消失
const double HIT_D_VEL = 5.0;//撞击速度改变

enum FLOOR_CLASS
{
	ORDINARY_FLOOR_3_BLOCKS,
	ORDINARY_FLOOR_2_BLOCKS,
	ORDINARY_FLOOR_1_BLOCK,
	MOVING_FLOOR_LOWER_SPEED,
	MOVING_FLOOR_HIGHER_SPEED,
	ICE_FLOOR,
	GRASS_FLOOR,
	VANISH_FLOOR
};

class Floor: public StaticImage
{
private:
	FLOOR_CLASS cFloor;
	double velX;
	double velY;
	double velM;
	int flipTimes;
	SDL_Rect* pStatusBlink;
	void fix();
	void vanish();
public:
	Floor();
	Floor(const std::string& vname,
				 const ScreenSurface& screen,
				 SDL_Rect* vpStatus,
				 SDL_Rect* vpStatusBlink,
				 double posX, double posY,
				 double velX, double velY,
				 double velM,
				 FLOOR_CLASS cFloor);
	void move(double dx, double dy);
	void show();
	double getPosY() const;
	double getPosX() const;
	double getVelX() const;
	double getVelY() const;
	double getVelM() const;
	FLOOR_CLASS getClass() const;
	void setBlink(int Times);
	SDL_Rect* getStatus() const;
};

class PFloor
{
private:
	Floor* p;
	Floor* beg;
	Floor* end;
public:
	PFloor();
	PFloor(Floor* p, Floor* beg, Floor* end);
	Floor& operator*();
	Floor* operator->();
	PFloor operator++();
	PFloor operator++(int);
	PFloor operator--();
	PFloor operator--(int);
	PFloor& operator+=(int);
	PFloor& operator-=(int);
	friend PFloor operator+(const PFloor&, int);
	friend PFloor operator-(const PFloor&, int);
	friend int operator-(const PFloor&, const PFloor&);
	friend bool operator==(const PFloor&, const PFloor&);
	friend bool operator!=(const PFloor&, const PFloor&);
	bool empty() const;
};

class Cloud: public StaticImage
{
private:
	int cirHeight;
public:
	Cloud();
	Cloud(const std::string& name,
		const ScreenSurface& screen,
		SDL_Rect* pStatus,
		double posX, double posY,
		int cirHeight);
	void move(double dx, double dy);
};

class Number: public StaticImage
{
private:
	SDL_Rect* pNumGreen[TOTAL_0_to_9];
	SDL_Rect* pNumRed[TOTAL_0_to_9];
	int num;
	int digit;
	bool flag;
	inline void getDigit();
public:
	Number();
	Number(const std::string& vname,
		const ScreenSurface& screen,
		SDL_Rect pNumGreen[],
		SDL_Rect pNumRed[],
		int posX, int posY,
		int num = 0);
	Number& operator+=(int dN);
	Number& operator-=(int dN);
	Number& operator*=(int dN);
	Number& operator/=(int dN);
	Number& operator=(int dN);
	void set(int L);
	int get() const;
	void changeGreen();
	void changeRed();
	void show() const;
};

class String: public StaticImage
{
private:
	static const int MAX_NAME_LENGTH = 16;
	SDL_Rect* pChar[TOTAL_ALPHABET + TOTAL_0_to_9 + 1];
	std::string str;
public:
	String();
	String(const std::string& vname, const ScreenSurface& screen, SDL_Rect pCharBeg[],
		   int posX, int posY, const std::string& s);
	void append(char ch);
	void backSpace();
	void clear();
	void show();
	std::string get();
};

class IPIG
{
private:
	std::string name;
	SDL_Surface* pSurface;
	SDL_Surface* pScreen;
	SDL_Rect *pWalkRightBeg,*pWalkRightEnd;
	SDL_Rect *pWalkLeftBeg, *pWalkLeftEnd;
	SDL_Rect *pFlyRightBeg, *pFlyRightEnd;
	SDL_Rect *pFlyLeftBeg,  *pFlyLeftEnd;
	SDL_Rect *pHitRightBeg, *pHitRightEnd;
	SDL_Rect *pHitLeftBeg,  *pHitLeftEnd;
	SDL_Rect *pStatus;
	int stdWidth;
	int stdHeight;
	double posX, posY;
	double velX, velY;
	double accX, accY;
	double airKX, airKY;
	STATUS Status;
	bool hit;
	bool land;
	PFloor collideFloor;
	bool collideJudge(PFloor colJudge) const;
public:
	IPIG(
				const std::string& name,
				const ScreenSurface& screen,
				SDL_Rect* pWalkRightBeg, SDL_Rect* pWalkRightEnd,
				SDL_Rect* pWalkLeftBeg,  SDL_Rect* pWalkLeftEnd,
				SDL_Rect* pFlyRightBeg,  SDL_Rect* pFlyRightEnd,
				SDL_Rect* pFlyLeftBeg,   SDL_Rect* pFlyLeftEnd,
				SDL_Rect* pHitRightBeg,  SDL_Rect* pHitRightEnd,
				SDL_Rect* pHitLeftBeg,   SDL_Rect* pHitLeftEnd,
				int vstdWidth, int vstdHeight,
				double posX, double posY,
				double airKX, double airKY,
				STATUS Status = WALK_RIGHT_STATUS
				);
	void change(double fx, double fy, PFloor colJudgeBeg, PFloor colJudgeEnd);
	bool isLand();//指是否为着陆瞬间
	bool isHit();//指是否为撞击瞬间
	void move(double dx, double dy);
	void show() const;
	double getPosX() const;
	double getPosY() const;
};
