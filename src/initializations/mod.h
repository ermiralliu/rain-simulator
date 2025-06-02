#ifndef RAIN_INITIALIZATIONS
#define RAIN_INITIALIZATIONS

#include "../structures/mod.h"

//from here on: texture loaders
void init_background(Textures* texture);

void init_rain(Droplet* drop, Textures* texture);

bool init(const char *title, int height, int width, int flags,
          SDL_Renderer **renderer, SDL_Window **window);

void placeDrops(Droplet *drop, RainType currentType);

#endif