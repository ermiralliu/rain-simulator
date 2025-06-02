#include "Supportive.h"
#include <stdio.h>
#include <stdint.h>

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