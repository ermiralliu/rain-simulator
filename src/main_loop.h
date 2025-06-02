#include <stdbool.h>
#include "structures/mod.h"


void render(const Droplet* drop, const Textures* texture, const Atmosphere atmo);
void update(Droplet* drop, const Atmosphere atmo, BackgroundLevel* ground, Uint32* reCnt);
Events event(Atmosphere* atmo, Counter* count,const Textures* texture);