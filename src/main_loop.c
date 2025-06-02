#include "main_loop.h"
#include <SDL2/SDL_events.h>
#include <math.h>
#include "CONSTANTS.h"
#include "Supportive.h"
#include "structures/mod.h"
#include "user_input/window_helpers.h"

// main loop
void render(const Droplet *drop, const Textures *texture,
            const Atmosphere atmo) {
  // SDL_SetRenderDrawColor(texture->renderer, 0, 0, 0, 255);
  SDL_RenderClear(texture->renderer);
  if (atmo.temperature > 80)
    SDL_RenderCopy(texture->renderer, texture->background[5], &(texture->srcR),
                   &(texture->destR));
  else
    SDL_RenderCopy(texture->renderer, texture->background[atmo.ground],
                   &(texture->srcR), &(texture->destR));
  for (int a = 0; a < NrOf_RAINDROPS; a++) {
    switch (drop[a].type) {
      case RAIN:
      case BRESHER:
        SDL_RenderCopyEx(texture->renderer, texture->rain[drop[a].type],
                       &(texture->rainSrc), &drop[a].dest,
                       atan(atmo.wind / atmo.baseSpeed) * 180 / M_PI, 0,
                       SDL_FLIP_NONE);
        break;
      case SNOW:
      case STATICSNOW:
        SDL_RenderCopy(texture->renderer, texture->rain[SNOW],
                     &(texture->rainSrc), &drop[a].dest);
        break;
    }
  }
  SDL_RenderPresent(texture->renderer);
}

void update(Droplet *drop, const Atmosphere atmo, BackgroundLevel *ground, Uint32 *reCnt) {
  for (int a = 0; a < NrOf_RAINDROPS; a++) {
    if (drop[a].type != STATICSNOW) {
      if (drop[a].dest.y < Y_DIMENSION + 30) {
        drop[a].dest.y += atmo.baseSpeed * drop[a].dest.h / 20;
        if (drop[a].dest.y >= Y_DIMENSION - 20 &&
            !drop[a].counted) { // Y_DIMENSION-20 cause the snow wont stick
                                // otherwise
          (*reCnt)++;
          drop[a].counted = true;
          if (atmo.currentType == SNOW) {
            if (rand() % 32 == 0 && (*ground) < 4) {
              drop[a].type = STATICSNOW;
              if ((*reCnt) > 5000 + 5000 * (*ground))
                ++(*ground);
            } else if ((*ground) >= 4 && rand() % 32 == 0)
              drop[a].type = STATICSNOW;
          }
        }
        if (atmo.currentType == SNOW)
          drop[a].dest.x -= atmo.wind * drop[a].dest.h / 20 - 1 + (rand() % 3);
        else {
          drop[a].dest.x -= atmo.wind * drop[a].dest.h / 20;
          if ((*ground) > 0 && (*reCnt) > 6000 * (5 - (*ground)))
            --(*ground);
        }
        if (drop[a].dest.x > X_DIMENSION)
          drop[a].dest.x = 2;
        else if (drop[a].dest.x < 1)
          drop[a].dest.x = X_DIMENSION;
      } else {
        //				drop[a] = {{drop[a].dest.x, rand()%5
        //-10,drop[a].dest.w,drop[a].dest.h}, false, atmo.currentType };
        drop[a].dest.y = rand() % 5 - 10;
        drop[a].counted = false;
        drop[a].type = atmo.currentType;
      }
    } else if (atmo.temperature > 0 && ((*reCnt) >= 80000 || rand() % 512 == 0))
      drop[a].type = atmo.currentType;
  }
}

Events event(Atmosphere *atmo, Counter *count, const Textures *texture) {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      showConsole(true);
      count->main += count->re;
      count->re = 0;
      return Quit(count->main);
    } else if (e.type == SDL_KEYDOWN) {
      int key = e.key.keysym.sym;
      if (key == SDLK_ESCAPE || key == SDLK_w || key == SDLK_t || key == SDLK_r)
        showConsole(true);
      if (e.key.keysym.sym == SDLK_ESCAPE) {
        count->main += count->re;
        count->re = 0;
        return Quit(count->main);
      } else if (e.key.keysym.sym == SDLK_w) {
        printf("Enter wind speed (pixel per second), positive values to the "
               "right:\n");
        double wind;
        scanf("%lf", &wind);
        atmo->wind = -wind / 60;
      } else if (e.key.keysym.sym == SDLK_t) {
        if (count->re > 3000) {
          count->main += count->re;
          count->re = 0;
        }
        printf("Enter temperature: (in degree Celsius)\n");
        int temperature;
        scanf("%d", &temperature);
        while (temperature > 200 || temperature < -274) {
          if (temperature > 200) {
            atmo->ground = 0;
            renderClear(texture, true);
            printf("\nTOO HOT\nAll the rain has vaporized\nEnter another "
                   "temperature (less than 200):\n");
          } else if (temperature < -274) {
            renderClear(texture, false);
            printf("\nUnder absolute 0\nNo Sun, No hope, Nothing\nReenter "
                   "another temperature (more than -274):\n");
          } else {
            atmo->temperature = temperature;
            arrange(atmo);
            exeToForeground();
            return REASSIGN;
          }
          scanf("%d", &temperature);
        }
        atmo->temperature = temperature;
        arrange(atmo);
      } else if (e.key.keysym.sym == SDLK_r) {
        printf("Number of fallen raindrops fallen until now is: %lu\n",
               count->main + count->re);
        printf("Press Enter to continue:\n");
        // char a = getc();
      }
    }
    exeToForeground();
  }
  return NORMAL;
}