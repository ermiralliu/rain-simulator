#include <stdio.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <time.h>
#include <math.h>
#include <windows.h>
#include <stdbool.h>
#include <conio.h>

#define FRAMEDELAY 16

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

SDL_Texture* TextureLoader(const char* texture, SDL_Renderer* ren);
bool init(const char* title, int height, int width, int flags, Droplet *drop);

bool event(Droplet* drop); //takes user input
void update(Droplet* drop); //updates droplet positions and background type
void render(Droplet* drop); //shows the updated image

void background(); //loads the different backgrounds
void rain(Droplet* drop); //initiates rain. I made it start from snow by default

void writeFile(); //writes the count of droplets in the raindrops.txt file
bool Quit(); // calls writeFile and closes app

void arrange(); //changes the current type (which the droplets turn to when they come back to the top)
void placeDrops(Droplet* drop); //puts the drops in their initial positions

void showConsole(bool show){ //shows console
	HWND consoleWindow = FindWindowA("ConsoleWindowClass", NULL);
	ShowWindow(consoleWindow, show);
} 

void exeToForeground(){ //hides the console after using it and brings application to foreground
	HWND gameWindow = FindWindowA(NULL, "Rain Simulator" );
	showConsole(false);
	SetActiveWindow(gameWindow);
}