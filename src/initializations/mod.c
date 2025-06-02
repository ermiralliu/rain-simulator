#include "../CONSTANTS.h"
#include <string.h>
#include <time.h>
#include <stdlib.h> // has rand
#include <SDL2/SDL_image.h>
#include "mod.h"

SDL_Texture* load_texture(const char* texture, SDL_Renderer* ren){
	SDL_Surface* tempSurface = IMG_Load(texture);
	SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, tempSurface);
	SDL_FreeSurface(tempSurface);
	return tex;
}

//from here on: texture loaders
void init_background(Textures* texture){
	texture->background[0] = load_texture("assets/sky1.png", texture->renderer);
	texture->background[1] = load_texture("assets/sky2.png", texture->renderer);
	texture->background[2] = load_texture("assets/sky3.png", texture->renderer);
	texture->background[3] = load_texture("assets/sky4.png", texture->renderer);
	texture->background[4] = load_texture("assets/sky5.png", texture->renderer); //getting progressively more snowy
	texture->background[5] = load_texture("assets/skyHot.png", texture->renderer); //5 is steamy
	SDL_QueryTexture(texture->background[0], NULL, NULL, &(texture->srcR.w), &(texture->srcR.h));
	texture->destR.x = texture->srcR.x = 0;
	texture->destR.y = texture->srcR.y = 0;
	texture->destR.w = X_DIMENSION;
	texture->destR.h = Y_DIMENSION;
}

// supportive:
void placeDrops(Droplet *drop, RainType currentType) {
  srand((unsigned)time(NULL));
  for (size_t a = 0; a < NrOf_RAINDROPS; a++) {
    drop[a].type = currentType;
    drop[a].dest.x = 2 + rand() % (X_DIMENSION - 2);
    drop[a].dest.y = -Y_DIMENSION + rand() % Y_DIMENSION;
  }
}

void init_rain(Droplet* drop, Textures* texture){
	texture->rain[BRESHER] = load_texture("assets/hail.png", texture->renderer);
	texture->rain[RAIN] = load_texture("assets/raindrop.png", texture->renderer);
	texture->rain[SNOW] = load_texture("assets/snow.png", texture->renderer);
	SDL_QueryTexture(texture->rain[RAIN], NULL, NULL, &(texture->rainSrc.w), &(texture->rainSrc.h));
	texture->rainSrc.x = 0;
	texture->rainSrc.y = 0;
	for (size_t a=0; a < NrOf_RAINDROPS; a++){
		drop[a].dest.h = texture->rainSrc.h + 3*(rand()%6); // I forgot we we're doing some 3D like shit, so we do need the h and w
		drop[a].dest.w = texture->rainSrc.w*( drop[a].dest.h / drop[a].dest.h );
	}
	placeDrops(drop, SNOW);
}

bool init(const char *title, int height, int width, int flags,
          SDL_Renderer **renderer, SDL_Window **window) {
  if (SDL_Init(SDL_INIT_EVERYTHING) >= 0) {
    *window =
        SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         width,height , flags);
    if (*window != NULL) {
      *renderer = SDL_CreateRenderer(*window, -1, 0);
      return *renderer != NULL;
    }
  }
  return false;
}