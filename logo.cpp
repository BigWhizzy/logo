#include "logo.h"

using namespace  LogoScreen;

Logo::Logo(SDL_Surface *screen,  unsigned char logo_mem[], int mem_size, float speed, FromDirection direction, EaseFn easefn) 
	: screen(screen), logo_mem(logo_mem), mem_size(mem_size), speed(speed), direction(direction), easefn(easefn){

	rwops_png = SDL_RWFromConstMem(logo_mem, mem_size);
	logoimg = IMG_LoadPNG_RW(rwops_png);
    if (!logoimg) {
    	fprintf(stderr, "Error loading logo: %s\n", IMG_GetError());
    	exit( -1);
    }

	switch(direction){
		case FromDirection::Left:
			pt_start = {(float)-logoimg->w, (float)(screen->h - logoimg->h) / 2};
			break;
		case FromDirection::Right:
			pt_start = {(float)logoimg->w, (float)(screen->h - logoimg->h) / 2};
			break;
		case FromDirection::Above:
			pt_start = {(float)(screen->w - logoimg->w) / 2, (float)-logoimg->h};
			break;
		case FromDirection::Below:
			pt_start = {(float)(screen->w - logoimg->w) / 2, (float)logoimg->h};
			break;
		default:
			pt_start = {(float)(screen->w - logoimg->w) / 2, (float)-logoimg->h};
			break;
	}
	
	pt_end = {(float)(screen->w - logoimg->w) / 2,  (float)(screen->h - logoimg->h) / 2};
	pt_curr = {0, 0};
}

Logo::~Logo(){
	SDL_FreeRW(rwops_png);
	SDL_FreeSurface(logoimg);
}	

float Logo::flip(float x){
	return 1 - x;
}

float Logo::easeIn(float x){
	return x * x;
}

float Logo::easeOut(float x){
	float f = flip(x);
	return flip(f * f);
}

float Logo::easeInOut(float x){
	return lerp(easeIn(x), easeOut(x), x);
}

float Logo::bounceIn(float x){
	return 1.0f - bounceOut(1.0f - x);
}

float Logo::bounceOut(float x){
	if (x < (1.0f / 2.75f)) {
		return 7.5625f * x * x;				
	}
	else if (x < (2.0f / 2.75f)) {
		return 7.5625f* (x -= (1.5f / 2.75f)) * x + 0.75f;
	}
	else if (x < (2.5f / 2.75f)) {
		return 7.5625f * (x -= (2.25f / 2.75f)) * x + 0.9375f;
	}
	else {
		return 7.5625f * (x -= (2.625f / 2.75f)) * x + 0.984375f;
	}
}

float Logo::bounceInOut(float x){
	if (x < 0.5f){
		return bounceIn(x * 2.0f) *0.5f;
	}
	return bounceOut(x * 2.0f - 1.0f) * 0.5f + 0.5f;		
}

float Logo::lerp(float a, float b, float t){
	return a + (b - a) * t;
}

void Logo::lerp(Point& a, Point& b, Point& c, float t){
	c.x = lerp(a.x, b.x, t);
	c.y = lerp(a.y, b.y, t);	
}

bool Logo::finished(void){
	return t/speed >= 1;
}

void Logo::ease(float t){
	switch(easefn){
		case EaseFn::Lerp:
			lerp(pt_start, pt_end, pt_curr, t);
			break;
		case EaseFn::EaseIn:
			lerp(pt_start, pt_end, pt_curr, easeIn(t));
			break;
		case EaseFn::EaseOut:
			lerp(pt_start, pt_end, pt_curr, easeOut(t));
			break;
		case EaseFn::EaseInOut:
			lerp(pt_start, pt_end, pt_curr, easeInOut(t));
			break;
		case EaseFn::BounceIn:
			lerp(pt_start, pt_end, pt_curr, bounceIn(t));
			break;
		case EaseFn::BounceOut:
			lerp(pt_start, pt_end, pt_curr, bounceOut(t));
			break;
		case EaseFn::BounceInOut:
			lerp(pt_start, pt_end, pt_curr, bounceInOut(t));
			break;
		default:
			lerp(pt_start, pt_end, pt_curr, t);
			break;			
	}
}

void Logo::update(float delta){
	if(!finished()){
		t += delta;
		ease(t/speed);	
		dstrect.x = pt_curr.x;
		dstrect.y = pt_curr.y;
		dstrect.w = logoimg->w;
		dstrect.h = logoimg->h;	
	}else{
		dstrect.x = pt_end.x;
		dstrect.y = pt_end.y;
	}
	// always render
	SDL_BlitSurface(logoimg, NULL, screen, &dstrect);
}