#ifndef RAIN_STRUCTURES
#define RAIN_STRUCTURES

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <stdbool.h>

typedef enum{
//	NONE = 0,
	RAIN,
	BRESHER,	//hail, icey rain, but I frogot and wrote it in Albanian
	SNOW,
	STATICSNOW,
} RainType;

typedef struct Droplet{
	SDL_Rect dest;
	RainType type;// = SNOW;
	bool counted;// = false;
} Droplet; // the initialization of these droplets will also be done in the droplet initializer


typedef struct Textures {
	SDL_Texture* background[6]; //background texures
	SDL_Rect srcR, destR;
	SDL_Texture* rain[3]; //rain textures/types
	SDL_Rect rainSrc;
	SDL_Renderer* renderer;
} Textures;

typedef enum {
	STARTER = 0,
	SNOWY_1 = 1,
	SNOWY_2 = 2,
	SNOWY_3 = 3,
	MAX_SNOW = 4,
	STEAMY = 5,
} BackgroundLevel;

typedef struct Atmosphere {

	float wind; // the largest thing goes first, this is the horizontal speed due to wind
	int baseSpeed; //ground is the background counter, baseSpeed is the vertical falling speed of the rain
	int temperature;
	RainType currentType;
	BackgroundLevel ground;

} Atmosphere;

inline Atmosphere init_atmosphere(){
	return (Atmosphere) {
		.wind = 0.f,
		.baseSpeed = 5,
		.temperature = -5,
		.currentType = SNOW,	// considering the relation between temperature, currentType and ground; 
		// this is something I could modify later
		.ground = STARTER
	};
}

typedef struct Counter {
	Uint64 main;
	Uint32 re;
} Counter;

inline Counter init_counter(){
  return (Counter) {0,0};
}

typedef enum {
	NORMAL,
	QUIT,
	REASSIGN,
} Events;

#endif