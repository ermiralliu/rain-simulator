#include <stdio.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <time.h>
#include <math.h>
#include <windows.h>
#include <stdbool.h>
#include <conio.h>

typedef enum{
//	NONE = 0,
	RAIN,
	BRESHER,	//hail, icey rain, but I frogot and wrote it in Albanian
	SNOW,
	STATICSNOW,
}RainType;

typedef struct{
	SDL_Rect dest;
	bool counted = false;
	RainType type = SNOW;
}Droplet;

typedef struct{
	SDL_Texture* background[6]; //background texures
	SDL_Rect srcR, destR;
	SDL_Texture* rain[3]; //rain textures/types
	SDL_Rect rainSrc;
	SDL_Renderer* renderer = 0;
}Textures;

typedef struct{
	short int temperature= -1;
	RainType currentType = SNOW;
	char ground = 0;
	int baseSpeed = 5; //ground is the background counter, baseSpeed is the vertical falling speed of the rain
	double wind = 0.0;
}Atmosphere;

typedef struct{
	Uint32 main=0;
	unsigned long int re=0;
}Counter;

typedef enum{
	QUIT,
	REASSIGN,
	NORMAL,
}Events;

SDL_Texture* TextureLoader(const char* texture, SDL_Renderer* ren);
bool init(const char* title, int height, int width, int flags, SDL_Renderer** renderer);

Events event(Atmosphere* atmo, Counter* count,const Textures* texture); //takes user input
void update(Droplet* drop, const Atmosphere atmo, char* ground, unsigned long int* reCount); //updates droplet positions and background type
void render(const Droplet* drop, const Textures *texture, const Atmosphere atmo); //shows the updated image

void background(Textures *texture); //loads the different backgrounds
void rain(Droplet* drop, Textures *texture); //initiates rain. I made it start from snow by default

void showConsole(bool show){ //shows console
	HWND consoleWindow = FindWindowA("ConsoleWindowClass", NULL);
	ShowWindow(consoleWindow, show);
} 

Events Quit(const Uint32 count){ // writes File and closes app
	FILE *fptr = fopen("Raindrops.txt", "w");
	fprintf(fptr, "Number of fallen raindrops is:%d\n", count);
	fclose(fptr);
	printf("Number of fallen raindrops in the end is: %d\n", count);
	return QUIT;
} 

void renderClear(const Textures* texture, bool steamy){
	SDL_RenderClear(texture->renderer);
	if(steamy)
		SDL_RenderCopy(texture->renderer, texture->background[5], &texture[0].srcR, &texture[0].destR);
	SDL_RenderPresent(texture->renderer);
}

void placeDrops(Droplet* drop, RainType currentType);  //puts the drops in their initial positions

void arrange(Atmosphere* atmo){ //changes the current type (which the droplets turn to when they come back to the top)
	if(atmo->temperature>=0){
		if(rand()%3 ==0 && atmo->temperature <10){
			atmo->currentType = BRESHER;
			atmo->baseSpeed=6;
		}	
		else{
			atmo->currentType = RAIN;
			atmo->baseSpeed=5;
		}
	}
	else{
		atmo->currentType = SNOW;
		atmo->baseSpeed=4;
	}	
}

SDL_Texture* TextureLoader(const char* texture, SDL_Renderer* ren){
	SDL_Surface* tempSurface = IMG_Load(texture);
	SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, tempSurface);
	SDL_FreeSurface(tempSurface);
	return tex;
}

void exeToForeground(){ //hides the console after using it and brings application to foreground
	HWND gameWindow = FindWindowA(NULL, "Rain Simulator" );
	showConsole(false);
	SetActiveWindow(gameWindow);
}