#include "Render.h"
#include "Physics.h"
#include "p2Log.h"


Render::Render() {
	ball = nullptr;
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO);

	window = SDL_CreateWindow("Verlet Integrator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}
Render::~Render() {}

void Render::Init() {

	SDL_RenderClear(renderer);

	//// GRAPHICS BACKGROUND ////
	backgroundrect.x = 0;
	backgroundrect.y = 0;
	backgroundrect.w = SCREEN_WIDTH;
	backgroundrect.h = SCREEN_HEIGHT;
	surfacebackground = IMG_Load("Assets/Background.png");
	texbackground = SDL_CreateTextureFromSurface(renderer, surfacebackground);
	SDL_FreeSurface(surfacebackground);
	SDL_RenderCopy(renderer, texbackground, NULL, &backgroundrect);


	//// BALL GRAPHICS INIT ////
	particle_rect.x = 0;
	particle_rect.y = 0;
	particle_rect.w = 27;
	particle_rect.h = 27;
	particle_surface = IMG_Load("Assets/Ball.png");
	particle_tex = SDL_CreateTextureFromSurface(renderer, particle_surface);
	SDL_FreeSurface(particle_surface);
}

void Render::blit_all(float x_ball, float y_ball) {
	particle_rect.x = x_ball;
	particle_rect.y = y_ball;
	SDL_RenderPresent(renderer);
	SDL_RenderCopy(renderer, particle_tex, NULL, &particle_rect);
}

void Render::Update(fPoint position) {
	
	SDL_RenderCopy(renderer, texbackground, NULL, &backgroundrect);
	particle_rect.x = position.x;
	particle_rect.y = position.y;
	
	SDL_RenderCopy(renderer, particle_tex, NULL, &particle_rect);
	SDL_RenderPresent(renderer);
}

void Render::clearScreen() {
	SDL_RenderClear(renderer);
}