#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>

#include <stdio.h>
#include <stdlib.h>

namespace LogoScreen{
/* 
 * Easing functions translated from from Tween.js - Licensed under the MIT license
 * at https://github.com/sole/tween.js
 *
 */
	class Logo {
		
		public:
		
		enum class FromDirection{
			Left,
			Right,
			Above,
			Below
		};

		enum class EaseFn{
			Lerp,
			EaseIn,
			EaseOut,
			EaseInOut,
			BounceIn,
			BounceOut,
			BounceInOut,			
		};

		private:

		typedef struct Point{
			float x;
			float y;
		} Point;

		
		SDL_Surface *screen;
		SDL_RWops *rwops_png;
		SDL_Surface *logoimg;
		SDL_Rect dstrect;
		const void *logo_mem;
		int mem_size;
		
		Point pt_start;
		Point pt_end;
		Point pt_curr;
		
		FromDirection direction;
		EaseFn easefn;
		
		float speed;
		float t = 0;
		
		float lerp(float a, float b, float t);
		void lerp(Point& a, Point& b, Point& c, float t);
		float easeIn(float x);
		float easeOut(float x);
		float easeInOut(float x);
		float flip(float x);
		float bounceIn(float x);
		float bounceOut(float x);
		float bounceInOut(float x);
		void ease(float t);
		
		public:
		
		Logo(SDL_Surface *screen, unsigned char logo_mem[], int mem_size, float speed, FromDirection direction, EaseFn easefn);		
		~Logo();
		
		void update(float delta);
		bool finished(void);

	};
}