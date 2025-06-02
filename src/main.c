#include "CONSTANTS.h"
#include <SDL2/SDL.h>
#include "structures/mod.h"
#include "initializations/mod.h"
#include "main_loop.h"

int main(int argc, char* argv[]){
	// showConsole(false); // We won't need this in linux for one
	SDL_Window* window;
	
	Counter count = init_counter();
	Droplet drop[NrOf_RAINDROPS];
	Textures texture;
	Atmosphere atmo;

	if(init("Rain Simulator", X_DIMENSION, Y_DIMENSION, SDL_WINDOW_SHOWN| SDL_WINDOW_ALLOW_HIGHDPI, &texture.renderer, &window)){
		init_rain(drop, &texture);
		init_background(&texture);
	} else {
		return 1;
	}
	Uint32 frameStart;
	int frameTime;
	while(true){
		Events e = event(&atmo, &count, &texture);
		
		if(e == QUIT)
			break;
		else if(e == REASSIGN)
			placeDrops(drop, atmo.currentType);
		
			frameStart = SDL_GetTicks();
		update(drop, atmo, &atmo.ground, &count.re);
		render(drop, &texture, atmo);
		
		frameTime = SDL_GetTicks()-frameStart;
		if(FRAMEDELAY > frameTime)
			SDL_Delay(FRAMEDELAY - frameTime);
	}
	SDL_Quit();
	return 0;
}