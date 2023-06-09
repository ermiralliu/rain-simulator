#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <windows.h>
#include <stdbool.h>
#include <conio.h>

SDL_Window* g_pWindow = 0;
SDL_Renderer*renderer = 0;
bool running = false;

SDL_Texture* m_pTexture; // the new SDL_Texture variable
SDL_Rect srcR, destR; // source rectangle and where it should go

SDL_Texture* raintext;
SDL_Rect rainSrc;
SDL_Rect rainDest[4608];

int v[4608];

unsigned long int count=0;

HWND window;

int l=5;
int temp= 1;
short int w=0;

const int FPS=60;
const int frameDelay = 1000/FPS;

Uint32 frameStart;
int frameTime;

SDL_Texture* TextureLoader(const char* texture, SDL_Renderer* ren){
	SDL_Surface* tempSurface = IMG_Load(texture);
	SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, tempSurface);
	SDL_FreeSurface(tempSurface);
	
	return tex;
}

int n=1;
 
void background(int n){
	if(n==1){m_pTexture = TextureLoader("assets/sky1.png", renderer);}
	if(n==2){m_pTexture = TextureLoader("assets/sky2.png", renderer);}
	if(n==3){m_pTexture = TextureLoader("assets/sky3.png", renderer);}
	if(n==4){m_pTexture = TextureLoader("assets/sky4.png", renderer);}
	if(n==5){m_pTexture = TextureLoader("assets/sky5.png", renderer);}
	SDL_QueryTexture(m_pTexture, NULL, NULL, &srcR.w, &srcR.h);
	destR.x = srcR.x = 0;
	destR.y = srcR.y = 0;
	destR.w = srcR.w;
	destR.h = srcR.h;
}

unsigned long int CountOnlySnow=0;

void rain(){
	srand((unsigned)time(NULL));
	if(temp>0){
		if(temp<10 && rand()%4==0){
			raintext = TextureLoader("assets/hail.png", renderer);
			l=6;
		}
		else{
			raintext = TextureLoader("assets/raindrop.png", renderer);
			l=5;
			n=1;
		}
	}
	else if(temp<=0){
	raintext = TextureLoader("assets/snow.png", renderer);
	l= 4;
	CountOnlySnow=0;
	}
	SDL_QueryTexture(raintext, NULL, NULL, &rainSrc.w, &rainSrc.h);
	rainSrc.x = 0;
	rainSrc.y = 0;
	for(int a=0; a<4864; a++){
		if(rand()%2==1){
			v[a]= 1;
			rainDest[a].x = 2+ 10*(a%128);
			rainDest[a].y = -700 + rand()%5 + 20*a/128;
			rainDest[a].h = rainSrc.h + 6*(rand()%3);
			rainDest[a].w = rainSrc.w*(rainDest[a].h/rainSrc.h);

		}
		else{v[a]= 0;}
	}
}

bool init(const char* title, int height, int width, int flags){
	
	if(SDL_Init(SDL_INIT_EVERYTHING) >= 0){
		g_pWindow = SDL_CreateWindow(title,SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,	height, width, flags);
		if(g_pWindow != 0){
			renderer = SDL_CreateRenderer(g_pWindow, -1, 0);
			rain();
			background(n);
		}
	}
	else{
		return false;
	}
	return true;
}

void render(){
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, m_pTexture, &srcR, &destR);
	for(int a=0; a<4864; a++){
		if(v[a]!=0){
		SDL_RenderCopyEx(renderer, raintext, &rainSrc, &rainDest[a], w, 0, SDL_FLIP_NONE);
		}
	}
	SDL_RenderPresent(renderer);
}

int snowMetric=500;

void update(){
	for(int a=0 ; a<4864; a++){
		if(v[a]==1){
			if(rainDest[a].y< 750){
				rainDest[a].y += l*rainDest[a].h/20;
				if(temp<0){
				rainDest[a].x -= (tan(w*3.14159/180) * l * rainDest[a].h / 20) -1 + (rand()%3);
				}
				else{
				rainDest[a].x -= tan(w*3.14159/180)*l*rainDest[a].h/20;
				}
				if(rainDest[a].x>1280){
						rainDest[a].x = 2;
				}
				else if(rainDest[a].x < 1){
						rainDest[a].x = 1280;
				}
				if(rainDest[a].y >= 700 && rainDest[a].y < 705){
					count++;
					CountOnlySnow++;
					if(temp<=0 && rand()%4==0){
						v[a]=2;
							if(CountOnlySnow>snowMetric && n<5){
							background(++n);
							snowMetric+=3500/n;
							}	
					}

				}
			}
			else{
				rainDest[a].y = rand()%5 -10;
				rainDest[a].x = 10*(a%128);

			}
		}
	}
}


void showconsole(){
	window = FindWindowA("ConsoleWindowClass", NULL);
	ShowWindow(window, 1);
}

void exeToForeground(){
	HWND hWnds = FindWindowA(NULL, "Rain Simulator" );
	SetActiveWindow(hWnds);
}

void writeFile(){
	FILE *fptr;
	fptr = fopen("Raindrops.txt", "w");
	fprintf(fptr, "Number of fallen raindrops in the end is:%d", count);
	fclose(fptr);
}

void event(){
	SDL_Event e;
	while(SDL_PollEvent(&e)){
		if (e.type== SDL_QUIT){
			showconsole();
			ShowWindow(window, 1);
			printf("Number of fallen raindrops in the end is: %d\n",count);
			writeFile();
			running=false;
		}
		else if(e.type == SDL_KEYDOWN) {
			if(e.key.keysym.sym == SDLK_ESCAPE){
					showconsole();
					printf("Number of fallen raindrops in the end is: %d\n",count);
					writeFile();
					running=false;
			}
			else if(e.key.keysym.sym == SDLK_w){
					showconsole();
					printf("Enter wind speed (angle of the rain in clockwise direction):\n");
					scanf("%d",&w);
					ShowWindow(window, 0);
					exeToForeground();
			}
   			else if(e.key.keysym.sym == SDLK_t){
					showconsole();
					printf("Enter temperature: (in degree Celsius)\n");
					scanf("%d",&temp);
					ShowWindow(window, 0);
					rain();
					exeToForeground();
			}
			else if(e.key.keysym.sym == SDLK_r){
					showconsole();
					printf("Number of fallen raindrops fallen until now is: %d\n",count);
					printf("Press any key to continue:\n");
					getch();
					ShowWindow(window, 0);
					exeToForeground();
			}
		}
	}
}

int main(int argc, char* argv[]){
	window = FindWindowA("ConsoleWindowClass", NULL);
	ShowWindow(window, 0);
	if(init("Rain Simulator", 1280, 720, SDL_WINDOW_SHOWN| SDL_WINDOW_ALLOW_HIGHDPI)){
		running = true;
	}
	else{
		return 1;
	}
	while(running){
		frameStart = SDL_GetTicks();
		event();
		update();
		render();
		frameTime = SDL_GetTicks()-frameStart;
		if(frameDelay > frameTime){
			SDL_Delay(frameDelay - frameTime);
		}
	}
	SDL_Quit();
	return 0;
}//*/