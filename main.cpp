#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>

#include <stdio.h>
#include <stdlib.h>
#include "assets.h"
#include "logo.h"


//------------------- PARAMETERS --------------------//

// screen size
#define WIDTH  320
#define HEIGHT 240

// background color (RGB)
#if defined LOGO_CUSTOM  && defined VERSION_POCKETGO

# define R 255
# define G 255
# define B 255

#else

# define R 96
# define G 148
# define B 8

#endif
	
// delay until the logo starts being visible(unit: milliseconds)
#define ANIMDELAY 1000

// time from the moment the logo stops moving and sound is played until the logo app closes (unit: frames) (60 frames = 1 sec)
#define ENDDELAY 3000

//speed at which the logo moves (unit: pixels per frame)
#if defined LOGO_CUSTOM  && defined VERSION_POCKETGO

#define ANIMTIME 2

#else

#define ANIMTIME 1

#endif

using namespace LogoScreen;

//---------------------------------------------------//

int main(int argc, char* argv[]) {
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0) {
		printf("Unable to initialize SDL: %s\n", SDL_GetError());
		return -1;
	}

	SDL_Surface *screen;
	static unsigned char *logo_mem1, *logo_mem2, *logo_mem3;
	int mem_size1, mem_size2, mem_size3;

	screen = SDL_SetVideoMode(WIDTH, HEIGHT, 16, SDL_SWSURFACE);
	SDL_ShowCursor(SDL_DISABLE);
	
#ifdef VERSION_POCKETGO	
# ifdef LOGO_CUSTOM
	logo_mem1 = png_logo_pocketgo1; 
	mem_size1 = sizeof(png_logo_pocketgo1); 
	logo_mem2 = png_logo_pocketgo2; 
	mem_size2 = sizeof(png_logo_pocketgo2); 
	logo_mem3 = png_logo_pocketgo3; 
	mem_size3 = sizeof(png_logo_pocketgo3);
# else
	logo_mem1 = png_logo_pocketgo; 
	mem_size1 = sizeof(png_logo_pocketgo); 
# endif
	
#elif VERSION_POWKIDDY
	logo_mem1 = png_logo_powkiddy; 
	mem_size1 = sizeof(png_logo_powkiddy); 
#else
	logo_mem1 = png_logo_bittboy; 
	mem_size1 = sizeof(png_logo_bittboy); 
#endif
	
#if defined LOGO_CUSTOM && defined VERSION_POCKETGO
	Logo logo1(screen, logo_mem1, mem_size1, ANIMTIME, Logo::FromDirection::Above, Logo::EaseFn::BounceOut);
	Logo logo2(screen, logo_mem2, mem_size2, ANIMTIME, Logo::FromDirection::Left, Logo::EaseFn::EaseOut);
	Logo logo3(screen, logo_mem3, mem_size3, ANIMTIME, Logo::FromDirection::Right, Logo::EaseFn::Lerp);
#else
	Logo logo1(screen, logo_mem1, mem_size1, ANIMTIME, Logo::FromDirection::Above, Logo::EaseFn::Lerp);
#endif	

    Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 1024);
	Mix_AllocateChannels(2);
	SDL_RWops *RWops_wav;
	Mix_Chunk *logosound;

	RWops_wav = SDL_RWFromConstMem(wav_logosound, sizeof(wav_logosound));
    logosound = Mix_LoadWAV_RW(RWops_wav, 0);
    if (!logosound) {
    	printf("Error loading sound: %s\n", Mix_GetError());
    	return -1;
    }

	uint32_t curr_time = SDL_GetTicks();
	uint32_t old_time = curr_time;
	uint32_t color = SDL_MapRGB(screen->format, R, G, B);	
	bool finished = false;
	float delta = 0;

	SDL_Rect rect{ 0, 0, (uint16_t)screen->w, (uint16_t)screen->h};
	
	// delay before logo appears 
	SDL_FillRect(screen, &rect, color);
	SDL_Flip(screen);
	while (curr_time < old_time + ANIMDELAY) {
		curr_time = SDL_GetTicks();
	}

	curr_time = SDL_GetTicks();
	old_time = curr_time;
	
	while(!finished){
		
		SDL_FillRect(screen, &rect, color);
		
		while (curr_time < old_time + 16) {
			curr_time = SDL_GetTicks();
		}
		delta =  (float)(curr_time - old_time)/1000;
		
		logo1.update(delta);		
#if defined LOGO_CUSTOM  && defined VERSION_POCKETGO
		logo2.update(delta);		
		logo3.update(delta);		
#endif		
		old_time = curr_time;				
		SDL_Flip(screen);
#if defined LOGO_CUSTOM  && defined VERSION_POCKETGO
		if(logo1.finished() && logo2.finished() && logo3.finished()){
#else
		if(logo1.finished()){
#endif
			Mix_PlayChannel(-1, logosound, 0);	
			finished = true;
		}
	}	

	SDL_Delay(ENDDELAY);
	SDL_FreeRW(RWops_wav);
	Mix_FreeChunk(logosound);
	Mix_CloseAudio();
	atexit(SDL_Quit); // using atexit to give Logo destructor a chance to clean up
	return 0;
}
