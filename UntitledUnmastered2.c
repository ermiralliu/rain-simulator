#include "Supportive.h"
#define X_DIMENSION 1400 //if you want to change the screen dimensions to another size
#define Y_DIMENSION 800
#define NrOf_RAINDROPS 3072 //if you want to make the raindrops denser or rarer

SDL_Renderer* renderer = 0;
//bool running = false;

SDL_Texture* m_pTexture[6]; //background texures
SDL_Rect srcR, destR;

SDL_Texture* raintext[3]; //rain textures/types
SDL_Rect rainSrc;

Uint32 count=0;
short int temperature= -1;
RainType currentType = SNOW;
int ground = 0, baseSpeed = 5; //ground is the background counter, baseSpeed is the vertical falling speed of the rain
double wind = 0.0;
unsigned long int reCnt = 0; //counter needed to change backgrounds;

int main(int argc, char* argv[]){
	showConsole(false);
	Droplet drop[NrOf_RAINDROPS];
	bool running = false;
	if(init("Rain Simulator", X_DIMENSION, Y_DIMENSION, SDL_WINDOW_SHOWN| SDL_WINDOW_ALLOW_HIGHDPI, drop))
		running = true;
	else
		return 1;
	Uint32 frameStart;
	int frameTime;
	while(running){
		frameStart = SDL_GetTicks();
		running = event(drop);
		update(drop);
		render(drop);
		frameTime = SDL_GetTicks()-frameStart;
		if(FRAMEDELAY > frameTime)
			SDL_Delay(FRAMEDELAY - frameTime);
	}
	SDL_Quit();
	return 0;
}

bool init(const char* title, int height, int width, int flags, Droplet *drop){	
	if(SDL_Init(SDL_INIT_EVERYTHING) >= 0){
		SDL_Window* g_pWindow = SDL_CreateWindow(title,SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,	height, width, flags);
		if(g_pWindow != 0){
			renderer = SDL_CreateRenderer(g_pWindow, -1, 0);
			rain(drop);
			background();
		}
	}
	else 
		return false;
	return true;
}

void render(Droplet* drop){
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	if(temperature>80 && ground==0 )
		SDL_RenderCopy(renderer, m_pTexture[5], &srcR, &destR);
	else
		SDL_RenderCopy(renderer, m_pTexture[ground], &srcR, &destR);
	for(int a=0; a<NrOf_RAINDROPS; a++){
		if(drop[a].type == STATICSNOW || drop[a].type == SNOW)
			SDL_RenderCopy(renderer, raintext[SNOW], &rainSrc, &drop[a].dest);
		else
			SDL_RenderCopyEx(renderer, raintext[drop[a].type], &rainSrc, &drop[a].dest, atan(wind/baseSpeed)*180/M_PI, 0, SDL_FLIP_NONE);
	}
	SDL_RenderPresent(renderer);
}

void update(Droplet* drop){
	for(int a=0 ; a<NrOf_RAINDROPS; a++){
		if(drop[a].type != STATICSNOW){
			if(drop[a].dest.y< Y_DIMENSION+30){
				drop[a].dest.y += baseSpeed* drop[a].dest.h /20;
				if(drop[a].dest.y >= Y_DIMENSION-20 && !drop[a].counted){ //Y_DIMENSION-20 cause the snow wont stick otherwise
					reCnt++;
					count++;
					drop[a].counted =true;
					if(currentType == SNOW){
						if(rand()%32==0 && ground<4){
							drop[a].type= STATICSNOW;
							if(reCnt> 5000+ 5000*ground)
								++ground;
						}
						else if(ground>=4 && rand()%32==0)
							drop[a].type = STATICSNOW;
					}	
				}
				if(currentType == SNOW)
					drop[a].dest.x -= wind* drop[a].dest.h /20 -1 + (rand()%3);	
				else if(currentType == RAIN){
					drop[a].dest.x -= wind* drop[a].dest.h/20;
					if(ground>0 &&  reCnt > 6000*(5-ground))
						--ground;
				}
				if(drop[a].dest.x > X_DIMENSION)
					drop[a].dest.x = 2;
				else if(drop[a].dest.x < 1)
					drop[a].dest.x = X_DIMENSION;
			}
			else{
				drop[a].dest.y = rand()%5 -10;
				drop[a].counted = false;
				drop[a].type = currentType;
			}
		}
		else if(temperature> 0 && (reCnt >= 80000 || rand()%512 == 0))
			drop[a].type = currentType;
	}
}

bool event(Droplet* drop){
	SDL_Event e;
	while(SDL_PollEvent(&e)){
		if (e.type== SDL_QUIT)
			return Quit();
		else if(e.type == SDL_KEYDOWN) {
			if(e.key.keysym.sym == SDLK_ESCAPE)
				return Quit();
			else if(e.key.keysym.sym == SDLK_w){
					showConsole(true);
					printf("Enter wind speed (pixel per second), positive values to the right:\n");
					scanf("%lf",&wind);
					wind/=-60;
					exeToForeground();
			}
   			else if(e.key.keysym.sym == SDLK_t){
				showConsole(true);
				if(reCnt>3000)
					reCnt=0;
				printf("Enter temperature: (in degree Celsius)\n");
				scanf("%d",&temperature);
				arrange();
				while(temperature> 200 || temperature< -274){
					if(temperature> 200){
						ground=0;
						SDL_RenderClear(renderer);
						SDL_RenderCopy(renderer, m_pTexture[5], &srcR, &destR);
						SDL_RenderPresent(renderer);
						printf("\nTOO HOT\nAll the rain has vaporized\nEnter another temperature (less than 200):\n");
					}	
					else if(temperature< -274){
						SDL_RenderClear(renderer);
						SDL_RenderPresent(renderer);
						printf("\nUnder absolute 0\nNo Sun, No hope, Nothing\nReenter another temperature (more than -274):\n");
					}
					scanf("%d",&temperature);
					if(temperature>=-274 && temperature <=200){
						arrange();
						placeDrops(drop);
					}
				}
				exeToForeground();
				
			}
			else if(e.key.keysym.sym == SDLK_r){
				showConsole(true);
				printf("Number of fallen raindrops fallen until now is: %d\n",count);
				printf("Press Enter to continue:\n");
				char a= getch();
				exeToForeground();
			}
		}
	}
	return true;
}
//from here to the next comment: texture loaders
void background(){
	m_pTexture[0] = TextureLoader("assets/sky1.png", renderer);
	m_pTexture[1] = TextureLoader("assets/sky2.png", renderer);
	m_pTexture[2] = TextureLoader("assets/sky3.png", renderer);
	m_pTexture[3] = TextureLoader("assets/sky4.png", renderer);
	m_pTexture[4] = TextureLoader("assets/sky5.png", renderer); //getting progressively more snowy
	m_pTexture[5] = TextureLoader("assets/skyHot.png", renderer); //5 is steamy
	SDL_QueryTexture(m_pTexture[0], NULL, NULL, &srcR.w, &srcR.h);
	destR.x = srcR.x = 0;
	destR.y = srcR.y = 0;
	destR.w = X_DIMENSION;
	destR.h = Y_DIMENSION;
}

void rain(Droplet* drop){
	raintext[BRESHER] = TextureLoader("assets/hail.png", renderer);
	raintext[RAIN] = TextureLoader("assets/raindrop.png", renderer);
	raintext[SNOW] = TextureLoader("assets/snow.png", renderer);
	SDL_QueryTexture(raintext[RAIN], NULL, NULL, &rainSrc.w, &rainSrc.h);
	rainSrc.x = 0;
	rainSrc.y = 0;
	placeDrops(drop);
}
// from here on, supportive functions to write less lines
void placeDrops(Droplet* drop){
	srand((unsigned)time(NULL));
	for(int a=0; a<NrOf_RAINDROPS; a++){
		drop[a].type = currentType;
		drop[a].dest.x = 2+ rand()%(X_DIMENSION-2);
		drop[a].dest.y = -Y_DIMENSION + rand()% Y_DIMENSION;
		drop[a].dest.h = rainSrc.h + 3*(rand()%6);
		drop[a].dest.w = rainSrc.w*( drop[a].dest.h / drop[a].dest.h );
	}
}

void arrange(){
	if(temperature>=0){
		if(rand()%4 ==0 && temperature <10){
			currentType = BRESHER;
			baseSpeed=6;
		}	
		else{
			currentType = RAIN;
			baseSpeed=5;
		}
	}
	else{
		currentType = SNOW;
		baseSpeed=4;
	}	
}

SDL_Texture* TextureLoader(const char* texture, SDL_Renderer* ren){
	SDL_Surface* tempSurface = IMG_Load(texture);
	SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, tempSurface);
	SDL_FreeSurface(tempSurface);
	return tex;
}

void writeFile(){
	FILE *fptr = fopen("Raindrops.txt", "w");
	fprintf(fptr, "Number of fallen raindrops is:%d\n", count);
	fclose(fptr);
}

bool Quit(){
	showConsole(true);
	writeFile();
	printf("Number of fallen raindrops in the end is: %d\n", count);
	return false;
}