#include "Object.h"

IPIG::IPIG
	(const std::string& vname,
	const ScreenSurface& screen,
	SDL_Rect* vpWalkRightBeg, SDL_Rect* vpWalkRightEnd,
	SDL_Rect* vpWalkLeftBeg, SDL_Rect* vpWalkLeftEnd,
	SDL_Rect* vpFlyRightBeg, SDL_Rect* vpFlyRightEnd,
	SDL_Rect* vpFlyLeftBeg, SDL_Rect* vpFlyLeftEnd,
	SDL_Rect* vpHitRightBeg,  SDL_Rect* vpHitRightEnd,
	SDL_Rect* vpHitLeftBeg,   SDL_Rect* vpHitLeftEnd,
	int vstdWidth, int vstdHeight,
	double vposX, double vposY,
	double vairKX, double vairKY,
	STATUS vStatus)
	:
	name(vname),
	pScreen(screen.point()),
	pWalkRightBeg(vpWalkRightBeg), pWalkRightEnd(vpWalkRightEnd),
	pWalkLeftBeg(vpWalkLeftBeg), pWalkLeftEnd(vpWalkLeftEnd),
	pFlyRightBeg(vpFlyRightBeg), pFlyRightEnd(vpFlyRightEnd),
	pFlyLeftBeg(vpFlyLeftBeg), pFlyLeftEnd(vpFlyLeftEnd),
	pHitRightBeg(vpHitRightBeg), pHitRightEnd(vpHitRightEnd),
	pHitLeftBeg(vpHitLeftBeg), pHitLeftEnd(vpHitLeftEnd),
	stdWidth(vstdWidth), stdHeight(vstdHeight),
	posX(vposX), posY(vposY),
	velX(0.0), velY(0.0),
	accX(0.0), accY(0.0),
	airKX(vairKX), airKY(vairKY),
	Status(vStatus),
	pStatus(pWalkRightBeg),
	collideFloor(PFloor()),
	land(false),
	hit(false) {
	pSurface = 0;
	SDL_Surface* pTemp;
	if ((pTemp = IMG_Load(name.c_str())) == 0) 
		throw "Can't open file";
	if ((pSurface = SDL_DisplayFormatAlpha(pTemp)) == 0)
		throw SDL_GetError();
	if (pTemp)
		SDL_FreeSurface(pTemp);
}

bool IPIG::collideJudge(PFloor colJudge) const {
	double tL = posX - (stdWidth / 2);
	double tR = tL + stdWidth;
	double tU = posY - (stdHeight / 2);
	double tD = tU + stdHeight;
	double fL = colJudge->getPosX() - (colJudge->getStatus()->w / 2);
	double fR = fL + colJudge->getStatus()->w;
	double fU = colJudge->getPosY() - (colJudge->getStatus()->h / 2);
	double fD = fU + colJudge->getStatus()->h;
	if (!collideFloor.empty()) 
		return tL < fR + EPS && tR > fL - EPS;
	else
		return (tL < fR + EPS && tR > fL - EPS) && (tD < fU + EPS && tD + velY > fU - EPS);
}

void IPIG::change(double fx, double fy, PFloor colJudgeBeg, PFloor colJudgeEnd) {
	
	//计算位置开始
	accX = fx;
	accY = fy;
	velX += accX * DELTA_T;
	velY += accY * DELTA_T;
	velX *= 1.0 - airKX;
	velY *= 1.0 - airKY;
	if (!collideFloor.empty()) {
		if (!collideJudge(collideFloor))
			collideFloor = PFloor();
		else {
			velX *= collideFloor->getVelM();
			velY = 0.0;
			posX += collideFloor->getVelX() * DELTA_T; 
		}
	}
	else
	while (colJudgeBeg != colJudgeEnd) {
		if (collideJudge(colJudgeBeg)) {
			collideFloor = colJudgeBeg;
			land = true;
			if (collideFloor->getClass() == VANISH_FLOOR)
				collideFloor->setBlink(TIMES_VANISH);
			velY = 0.0;
			posX += collideFloor->getVelX() * DELTA_T; 
			posY = collideFloor->getPosY() - collideFloor->getStatus()->h / 2 - stdHeight / 2;
			break;
		}
		++colJudgeBeg;
	}
	posX += velX * DELTA_T;
	posY += velY * DELTA_T;

	if (posY - (stdHeight / 2) < 0) {
		posY = stdHeight / 2;
		velY = HIT_D_VEL;
		collideFloor = PFloor();
		hit = true;
	}

	if (posX - stdWidth / 2 < -EPS)
		posX = pScreen->w - stdWidth / 2;
	if (posX + stdWidth / 2 > pScreen->w + EPS) 
		posX = (stdWidth / 2);
	//------------------------------------------------------计算位置结束

	//计算动画开始
	if (Status == HIT_RIGHT_STATUS || Status == HIT_LEFT_STATUS) {
		++pStatus;
		if (pStatus == pHitRightEnd) {
			Status = FLY_RIGHT_STATUS;
			pStatus = pFlyRightBeg; 
		}
		if (pStatus == pHitLeftEnd) {
			Status = FLY_LEFT_STATUS;
			pStatus = pFlyLeftBeg;
		}	
		if (Status == WALK_RIGHT_STATUS || Status == FLY_RIGHT_STATUS) {
			Status = HIT_RIGHT_STATUS;
			pStatus = pHitRightBeg;
		}
		if (Status == WALK_LEFT_STATUS || Status == FLY_LEFT_STATUS) {
			Status = HIT_LEFT_STATUS;
			pStatus = pHitLeftBeg;
		}
	}
	else
	if (velY > EPS)
	{
		if (Status == WALK_RIGHT_STATUS || Status == HIT_RIGHT_STATUS) {
			Status = FLY_RIGHT_STATUS;
			pStatus = pFlyRightBeg - 1;
		}
		if (Status == WALK_LEFT_STATUS || Status == HIT_LEFT_STATUS) {
			Status = FLY_LEFT_STATUS;
			pStatus = pFlyLeftBeg - 1;
		}
		if (velX >= 0)	{
			if (Status == FLY_RIGHT_STATUS){
				++pStatus;
				if (pStatus == pFlyRightEnd)
					pStatus = pFlyRightBeg;
			}
			else {
				Status = FLY_RIGHT_STATUS;
				pStatus = pFlyRightBeg;
			}
		}
		else {
			if (Status == FLY_LEFT_STATUS) {
				++pStatus;
				if (pStatus == pFlyLeftEnd)
					pStatus = pFlyLeftBeg;
			}
			else {
				Status = FLY_LEFT_STATUS;
				pStatus = pFlyLeftBeg;
			}
		}
	}
	else
	{
		if (Status == FLY_RIGHT_STATUS || Status == HIT_RIGHT_STATUS) {
			Status = WALK_RIGHT_STATUS;
			pStatus = pWalkRightBeg - 1;
		}
		if (Status == FLY_LEFT_STATUS || Status == HIT_LEFT_STATUS) {
			Status = WALK_LEFT_STATUS;
			pStatus = pWalkLeftBeg - 1;
		}
		if (std::abs(velX) <= 0.5) {
			pStatus = (Status == WALK_RIGHT_STATUS ? pWalkRightBeg : pWalkLeftBeg);
		}
		if (velX > 0.5) {
			if (Status == WALK_RIGHT_STATUS) {
				++pStatus;
				if (pStatus == pWalkRightEnd)
					pStatus = pWalkRightBeg;
			}
			else {
				Status = WALK_RIGHT_STATUS;
				pStatus = pWalkRightBeg;
			}
		}
		if (velX < -0.5) {
			if (Status == WALK_LEFT_STATUS) {
				++pStatus;
				if (pStatus == pWalkLeftEnd)
					pStatus = pWalkLeftBeg;
			}
			else {
				Status = WALK_LEFT_STATUS;
				pStatus = pWalkLeftBeg;
			}
		}
	}
	//---------------------------------------------------------计算动画结束
}

bool IPIG::isLand() {
	if (!land) return false;
	land = false;
	return true;
}

bool IPIG::isHit() {
	if (!hit) return false;
	hit = false;
	return true;
}
	
void IPIG::move(double dx, double dy) {
	posX += dx;
	posY += dy;
}

void IPIG::show() const {
	SDL_Rect pTemp;
	pTemp.x = round(posX) - (pStatus->w / 2);
	pTemp.y = round(posY) - (pStatus->h / 2);
	pTemp.w = 0;
	pTemp.h = 0;
	if (SDL_BlitSurface(pSurface, pStatus, pScreen, &pTemp) == -1)
		throw SDL_GetError();

}

double IPIG::getPosX() const {
	return posX;
}

double IPIG::getPosY() const {
	return posY;
}

Floor::Floor()
	:cFloor(FLOOR_CLASS(0)), StaticImage(),
	 flipTimes(0),	 pStatusBlink(0) {
							
}

Floor::Floor(const std::string& vname,
							const ScreenSurface& screen,
							SDL_Rect* vpStatus,
							SDL_Rect* vpStatusBlink,
							double vposX, double vposY,
							double vvelX, double vvelY,
							double vvelM,
							FLOOR_CLASS vcFloor)
	:StaticImage(vname, screen, vpStatus, vposX, vposY),
	 velX(vvelX), velY(vvelY), velM(vvelM),
	 cFloor(vcFloor), flipTimes(-1), 
	 pStatusBlink(vpStatusBlink) {
}

void Floor::move(double dx, double dy) {
	posX += dx + velX * DELTA_T;
	posY += dy + velY * DELTA_T;
	if (posX - (pStatus->w / 2) < WIDTH_SCREEN_BORDER)
		velX = std::abs(velX);
	if (posX - (pStatus->w / 2) + pStatus->w > pScreen->w - WIDTH_SCREEN_BORDER) 
		velX = -std::abs(velX);
}

void Floor::show() {
	if (flipTimes > 0) {
		--flipTimes;
		SDL_Rect* pTmp = pStatus;
		pStatus = pStatusBlink;
		pStatusBlink = pTmp;
	}
	if (flipTimes == 0)
		vanish();
	SDL_Rect pTemp;
	pTemp.x = round(posX) - (pStatus->w / 2);
	pTemp.y = round(posY) - (pStatus->h / 2);
	pTemp.w = 0;
	pTemp.h = 0;
	
	if (SDL_BlitSurface(pSurface, pStatus, pScreen, &pTemp) == -1)
		throw SDL_GetError();
}

double Floor::getPosX() const {
	return posX;
}

double Floor::getPosY() const {
	return posY;
}

double Floor::getVelX() const {
	return velX;
}

double Floor::getVelY() const {
	return velY;
}

double Floor::getVelM() const {
	return velM;
}

FLOOR_CLASS Floor::getClass() const {
	return cFloor;
}

void Floor::setBlink(int Times) {
	if (cFloor != VANISH_FLOOR || flipTimes != -1) return; 
	flipTimes = Times;
}

void Floor::vanish() {
	posX = -SCREEN_OUTSIDE;
	posY = -SCREEN_OUTSIDE;
}


SDL_Rect* Floor::getStatus() const {
	return pStatus;
}

PFloor::PFloor()
	:p(0), beg(0), end(0) {
}

PFloor::PFloor(Floor* pp, Floor* pbeg, Floor* pend)
	:beg(pbeg),  end(pend) {
	p = (pp == 0 ? beg : pp);
}

Floor& PFloor::operator*() {
	return *p;
}

Floor* PFloor::operator->() {
	return p;
}

PFloor PFloor::operator++() {
	++p;
	if (p == end) 
		p = beg;
	return *this;
}

PFloor PFloor::operator++(int) {
	PFloor tmp(*this);
	++p;
	if (p == end)
		p = beg;
	return tmp;
}

PFloor PFloor::operator--() {
	--p;
	if (p < beg) 
		p = end - 1;
	return *this;
}

PFloor PFloor::operator--(int) {
	
	PFloor tmp(*this);
	--p;
	if (p < beg)
		p = end - 1;
	return tmp;
}

PFloor& PFloor::operator+=(int d) {
	p += d;
	while (p >= end)
		p -= end - beg;
	return *this;
}

PFloor& PFloor::operator-=(int d) {
	p -= d;
	while (p < beg)
		p += end - beg;
	return *this;
}

PFloor operator+(const PFloor& lhs, int d) {
	PFloor tmp(lhs);
	tmp.p += d;
	while (tmp.p >= tmp.end)
		tmp.p -= tmp.end - tmp.beg;
	return tmp;
}

PFloor operator-(const PFloor& lhs, int d) {
	PFloor tmp(lhs);
	tmp.p -= d;
	while (tmp.p < tmp.beg)
		tmp.p += tmp.end - tmp.beg;
	return tmp;
}

int operator-(const PFloor& lhs, const PFloor& rhs) {
	int tmp = lhs.p - rhs.p;
	while (tmp < 0)
		tmp += lhs.end - lhs.beg;
	return tmp;
}

bool operator==(const PFloor& lhs, const PFloor& rhs) {
	return lhs.p == rhs.p;
}

bool operator!=(const PFloor& lhs, const PFloor& rhs) {
	return !(lhs == rhs);
}

bool PFloor::empty() const {
	return p == 0;
}

Cloud::Cloud()
	:cirHeight(0) {
}

Cloud::Cloud(const std::string& vname,
		const ScreenSurface& screen,
		SDL_Rect* vpStatus,
		double vposX, double vposY,
		int cH)
		:StaticImage(vname, screen, vpStatus, vposX, vposY),
		 cirHeight(cH) {
}

void Cloud::move(double dx, double dy) {
	posX += dx;
	posY += dy;
	if (posY <= -cirHeight)
		posY += cirHeight;
}

void Number::getDigit() {
	if (num < 0)
		num = 0;
	digit = 1;
	int tmp = num;
	while (tmp >= 10)
	{
		tmp /= 10;
		++digit;
	}
}

Number::Number()
	:num(0), flag(true), digit(0), StaticImage() {
}

Number::Number(const std::string& vname,
		const ScreenSurface& screen,
		SDL_Rect vpNumGreen[],
		SDL_Rect vpNumRed[],
		int posX, int posY,
		int vnum)
		:num(vnum), flag(true), StaticImage(vname, screen, 0, posX, posY) {
	for (int i = 0; i != 10; ++i)
	{
		pNumGreen[i] = vpNumGreen + i;
		if (vpNumRed) 
			pNumRed[i] = vpNumRed + i;
	}
	getDigit();
}

Number& Number::operator+=(int dN) {
	num += dN;
	getDigit();
	return *this;
}

Number& Number::operator-=(int dN) {
	num -= dN;
	getDigit();
	return *this;
}

Number& Number::operator*=(int dN) {
	num *= dN;
	getDigit();
	return *this;
}

Number& Number::operator/=(int dN) {
	num /= dN;
	getDigit();
	return *this;
}

Number& Number::operator=(int dN) {
	num = dN;
	getDigit();
	return *this;
}

void Number::set(int L) {
	num = L;
	getDigit();
}

int Number::get() const {
	return num;
}

void Number::changeGreen() {
	flag = true;
}

void Number::changeRed() {
	flag = false;
}

void Number::show() const {
	int tmpD = digit;
	int tmpN = num;
	while (tmpD > 0) {
		SDL_Rect pTmp;
		if (flag) {
			pTmp.x = round(posX) + (pNumGreen[0]->w) * (tmpD - 1);
			pTmp.y = round(posY);
			pTmp.w = 0;
			pTmp.h = 0;
			if (SDL_UpperBlit(pSurface, pNumGreen[tmpN % 10], pScreen, &pTmp) == -1)
				throw SDL_GetError();
		}
		else {
			pTmp.x = round(posX) + (pNumRed[0]->w) * (tmpD - 1);
			pTmp.y = round(posY);
			pTmp.w = 0;
			pTmp.h = 0;
			if (SDL_UpperBlit(pSurface, pNumRed[tmpN % 10], pScreen, &pTmp) == -1)
				throw SDL_GetError();
		}
		tmpN /= 10;
		--tmpD;
	}
}

String::String()
	:str(""), StaticImage() {
}

String::String(const std::string& vname, const ScreenSurface& screen, SDL_Rect pCharBeg[],
		   int posX, int posY, const std::string& s)
		   :str(s), StaticImage(vname, screen, 0, posX, posY) {
	for (int i = 0; i != TOTAL_0_to_9 + TOTAL_ALPHABET + 1; ++i)
		pChar[i] = pCharBeg + i;
}

void String::append(char ch) {
	if (str.size() < MAX_NAME_LENGTH)
		str += ch;
}

void String::backSpace() {
	if (!str.empty())
		str.erase(str.size() - 1, 1);
}

void String::clear() {
	str.clear();
}

void String::show() {
	double t = posX, totalWidth = 0;
	for (int i = 0; i != str.size(); ++i) {
		int c = TOTAL_0_to_9 + TOTAL_ALPHABET;
		if (std::isalpha(str[i]))
			c = std::toupper(str[i]) - 'A';
		if (std::isdigit(str[i]))
			c = str[i] - '0' + TOTAL_ALPHABET;
		totalWidth += pChar[c]->w;
	}
	posX -= totalWidth / 2;
	for (int i = 0; i != str.size(); ++i)
	{
		int c = TOTAL_0_to_9 + TOTAL_ALPHABET;
		if (std::isalpha(str[i]))
			c = std::toupper(str[i]) - 'A';
		if (std::isdigit(str[i]))
			c = str[i] - '0' + TOTAL_ALPHABET;
		pStatus = pChar[c];
		StaticImage::show();
		posX += pChar[c]->w;
	}
	posX = t;
}

std::string String::get() {
	if (str == "") 
		return "NONAME";
	else 
		return str;
}
