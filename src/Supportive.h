#ifndef RAIN_SUPPORTER
#define RAIN_SUPPORTER

#include "structures/mod.h"
#include <stdbool.h>

SDL_Texture* load_texture(const char* texture, SDL_Renderer* ren);

void init_background(Textures *texture); //loads the different backgrounds
void init_rain(Droplet* drop, Textures *texture); //initiates rain. I made it start from snow by default


Events Quit(const Uint32 count);

void renderClear(const Textures* texture, bool steamy);

void placeDrops(Droplet* drop, RainType currentType);  //puts the drops in their initial positions

void arrange(Atmosphere* atmo);

#endif