#include "Supportive.h"
#define X_DIMENSION 1400 //if you want to change the screen dimensions to another size
#define Y_DIMENSION 800
#define NrOf_RAINDROPS 3072 //if you want to make the raindrops denser or rarer
#define FRAMEDELAY 15

int main(int argc, char* argv[]){
	showConsole(false);
	Counter count;
	Droplet drop[NrOf_RAINDROPS];
	Textures texture;
	Atmosphere atmo;
	if(init("Rain Simulator", X_DIMENSION, Y_DIMENSION, SDL_WINDOW_SHOWN| SDL_WINDOW_ALLOW_HIGHDPI, &texture.renderer)){
		rain(drop, &texture);
		background(&texture);
	}
	else	
		return 1;
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

bool init(const char* title, int height, int width, int flags, SDL_Renderer** renderer){	
	if(SDL_Init(SDL_INIT_EVERYTHING) >= 0){
		SDL_Window* g_pWindow = SDL_CreateWindow(title,SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,	height, width, flags);
		if(g_pWindow != 0){
			*renderer = SDL_CreateRenderer(g_pWindow, -1, 0);
			return true;
		}
	}
	return false;
}

//main loop
void render(const Droplet* drop,const Textures* texture, const Atmosphere atmo){
	SDL_SetRenderDrawColor(texture->renderer, 0, 0, 0, 255);
	SDL_RenderClear(texture->renderer);
	if(atmo.temperature>80)
		SDL_RenderCopy(texture->renderer, texture->background[5], &(texture->srcR), &(texture->destR));
	else
		SDL_RenderCopy(texture->renderer, texture->background[atmo.ground], &(texture->srcR), &(texture->destR));
	for(int a=0; a<NrOf_RAINDROPS; a++){
		if(drop[a].type == STATICSNOW || drop[a].type == SNOW)
			SDL_RenderCopy(texture->renderer, texture->rain[SNOW], &(texture->rainSrc), &drop[a].dest);
		else
			SDL_RenderCopyEx(texture->renderer, texture->rain[drop[a].type], &(texture->rainSrc), &drop[a].dest, atan(atmo.wind/atmo.baseSpeed)*180/M_PI, 0, SDL_FLIP_NONE);
	}
	SDL_RenderPresent(texture->renderer);
}

void update(Droplet* drop, const Atmosphere atmo, char* ground, unsigned long int* reCnt){
	for(int a=0 ; a<NrOf_RAINDROPS; a++){
		if(drop[a].type != STATICSNOW){
			if(drop[a].dest.y< Y_DIMENSION+30){
				drop[a].dest.y += atmo.baseSpeed* drop[a].dest.h /20;
				if(drop[a].dest.y >= Y_DIMENSION-20 && !drop[a].counted){ //Y_DIMENSION-20 cause the snow wont stick otherwise
					(*reCnt)++;
					drop[a].counted =true;
					if(atmo.currentType == SNOW){
						if(rand()%32==0 && (*ground)<4){
							drop[a].type= STATICSNOW;
							if((*reCnt)> 5000+ 5000*(*ground))
								++(*ground);
						}
						else if((*ground)>=4 && rand()%32==0)
							drop[a].type = STATICSNOW;
					}	
				}
				if(atmo.currentType == SNOW)
					drop[a].dest.x -= atmo.wind* drop[a].dest.h /20 -1 + (rand()%3);	
				else{
					drop[a].dest.x -= atmo.wind* drop[a].dest.h/20;
					if((*ground)>0 &&  (*reCnt) > 6000*(5-(*ground)))
						--(*ground);
				}
				if(drop[a].dest.x > X_DIMENSION)
					drop[a].dest.x = 2;
				else if(drop[a].dest.x < 1)
					drop[a].dest.x = X_DIMENSION;
			}
			else{
//				drop[a] = {{drop[a].dest.x, rand()%5 -10,drop[a].dest.w,drop[a].dest.h}, false, atmo.currentType };
				drop[a].dest.y = rand()%5 -10;
				drop[a].counted = false;
				drop[a].type = atmo.currentType;
			}
		}
		else if(atmo.temperature> 0 && ((*reCnt) >= 80000 || rand()%512 == 0))
			drop[a].type = atmo.currentType;
	}
}

Events event(Atmosphere* atmo, Counter* count,const Textures* texture){
	SDL_Event e;
	while(SDL_PollEvent(&e)){
		if (e.type== SDL_QUIT){
			showConsole(true);
			count->main += count->re;
			count->re= 0;
			return Quit(count->main);
		}
		else if(e.type == SDL_KEYDOWN) {
			int key = e.key.keysym.sym;
			if(key == SDLK_ESCAPE|| key == SDLK_w || key == SDLK_t || key ==SDLK_r)
				showConsole(true);
			if(e.key.keysym.sym == SDLK_ESCAPE){
				count->main += count->re;
				count->re= 0;
				return Quit(count->main);
			}
			else if(e.key.keysym.sym == SDLK_w){
				printf("Enter wind speed (pixel per second), positive values to the right:\n");
				double wind;
				scanf("%lf",&wind);
				atmo->wind = -wind/60;
			}
   			else if(e.key.keysym.sym == SDLK_t){
				if(count->re>3000){
					count->main += count->re;
					count->re= 0;
				}
				printf("Enter temperature: (in degree Celsius)\n");
				int temperature;
				scanf("%d",&temperature);
				while(temperature> 200 || temperature< -274){
					if(temperature> 200){
						atmo->ground=0;
						renderClear(texture, true);
						printf("\nTOO HOT\nAll the rain has vaporized\nEnter another temperature (less than 200):\n");
					}	
					else if(temperature< -274){
						renderClear(texture, false);
						printf("\nUnder absolute 0\nNo Sun, No hope, Nothing\nReenter another temperature (more than -274):\n");
					}
					else{
						atmo->temperature = temperature;
						arrange(atmo);
						exeToForeground();
						return REASSIGN;
					}
					scanf("%d",&temperature);
				}
				atmo->temperature = temperature;
				arrange(atmo);
			}
			else if(e.key.keysym.sym == SDLK_r){
				printf("Number of fallen raindrops fallen until now is: %d\n", count->main+ count->re);
				printf("Press Enter to continue:\n");
				char a= getch();
			}
		}
		exeToForeground();
	}
	return NORMAL;
}
//from here on: texture loaders
void background(Textures* texture){
	texture->background[0] = TextureLoader("assets/sky1.png", texture->renderer);
	texture->background[1] = TextureLoader("assets/sky2.png", texture->renderer);
	texture->background[2] = TextureLoader("assets/sky3.png", texture->renderer);
	texture->background[3] = TextureLoader("assets/sky4.png", texture->renderer);
	texture->background[4] = TextureLoader("assets/sky5.png", texture->renderer); //getting progressively more snowy
	texture->background[5] = TextureLoader("assets/skyHot.png", texture->renderer); //5 is steamy
	SDL_QueryTexture(texture->background[0], NULL, NULL, &(texture->srcR.w), &(texture->srcR.h));
	texture->destR.x = texture->srcR.x = 0;
	texture->destR.y = texture->srcR.y = 0;
	texture->destR.w = X_DIMENSION;
	texture->destR.h = Y_DIMENSION;
}

void rain(Droplet* drop, Textures* texture){
	texture->rain[BRESHER] = TextureLoader("assets/hail.png", texture->renderer);
	texture->rain[RAIN] = TextureLoader("assets/raindrop.png", texture->renderer);
	texture->rain[SNOW] = TextureLoader("assets/snow.png", texture->renderer);
	SDL_QueryTexture(texture->rain[RAIN], NULL, NULL, &(texture->rainSrc.w), &(texture->rainSrc.h));
	texture->rainSrc.x = 0;
	texture->rainSrc.y = 0;
	for(int a=0; a<NrOf_RAINDROPS; a++){
		drop[a].dest.h = texture->rainSrc.h + 3*(rand()%6);
		drop[a].dest.w = texture->rainSrc.w*( drop[a].dest.h / drop[a].dest.h );
	}
	placeDrops(drop, SNOW);
}
//supportive:
void placeDrops(Droplet* drop, RainType currentType){  
	srand((unsigned)time(NULL));
	for(int a=0; a< NrOf_RAINDROPS; a++){
		drop[a].type = currentType;
		drop[a].dest.x = 2+ rand()%(X_DIMENSION-2);
		drop[a].dest.y = -Y_DIMENSION + rand()% Y_DIMENSION;
	}
}