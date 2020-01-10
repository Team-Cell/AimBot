#include "Render.h"
#include "Physics.h"
#include "p2Log.h"


Render::Render() {
	ball = nullptr;
	fps = 30;
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO);

	window = SDL_CreateWindow("Verlet Integrator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT,0);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	particle_rect.x = 0;
	particle_rect.y = 0;
	particle_rect.w = 36;
	particle_rect.h = 46;

	player1_rect.x = 0;
	player1_rect.y = 0;
	player1_rect.w = 62;
	player1_rect.h = 100;

	player2_rect.x = 0;
	player2_rect.y = 0;
	player2_rect.w = 62;
	player2_rect.h = 100;

	bazooka_rect.x = 0;
	bazooka_rect.y = 0;
	bazooka_rect.w = 62;
	bazooka_rect.h = 31;

	center.x = 31;
	center.y = 15;

	/////IDLE ANIMATION//////
	idle_animation.PushBack({ 0, 0, 66, 75 });
	idle_animation.PushBack({ 66, 0, 66, 75 });
	idle_animation.PushBack({ 132, 0, 66, 75 });
	idle_animation.PushBack({ 198, 0, 66, 75 });
	idle_animation.PushBack({ 264, 0, 66, 75 });
	idle_animation.PushBack({ 330, 0, 66, 75 });
	idle_animation.PushBack({ 396, 0, 66, 75 });
	idle_animation.PushBack({ 462, 0, 66, 75 });
	idle_animation.PushBack({ 528, 0, 66, 75 });
	idle_animation.PushBack({ 594, 0, 66, 75 });
	idle_animation.PushBack({ 0, 75, 66, 75 });
	idle_animation.PushBack({ 66, 75, 66, 75 });
	idle_animation.PushBack({ 132, 75, 66, 75 });
	idle_animation.PushBack({ 198, 75, 66, 75 });
	idle_animation.PushBack({ 264, 75, 66, 75 });
	idle_animation.PushBack({ 330, 75, 66, 75 });
	idle_animation.PushBack({ 396, 75, 66, 75 });
	idle_animation.PushBack({ 462, 75, 66, 75 });
	idle_animation.PushBack({ 528, 75, 66, 75 });
	idle_animation.PushBack({ 594, 75, 66, 75 });
	idle_animation.speed = 0.07f;

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
	cell_surface = IMG_Load("Assets/Ball.png");
	cell_tex = SDL_CreateTextureFromSurface(renderer, cell_surface);
	SDL_FreeSurface(cell_surface);
	particle_tex = cell_tex;

	//// PLAYER 1 GRAPHICS INIT ////
	player1_surface = IMG_Load("Assets/NewPlayer1.png");
	player1_tex = SDL_CreateTextureFromSurface(renderer, player1_surface);
	SDL_FreeSurface(player1_surface);

	//// PLAYER 2 GRAPHICS INIT ////
	player2_surface = IMG_Load("Assets/NewPlayer2.png");
	player2_tex = SDL_CreateTextureFromSurface(renderer, player2_surface);
	SDL_FreeSurface(player2_surface);

	//// BAZOOKA GRAPHICS INIT ////
	bazooka_surface = IMG_Load("Assets/Bazooka.png");
	bazooka_tex = SDL_CreateTextureFromSurface(renderer, bazooka_surface);
	SDL_FreeSurface(bazooka_surface);

	//// GRENADE GRAPHICS INIT ////
	grenade_surface = IMG_Load("Assets/Grenade.png");
	grenade_tex = SDL_CreateTextureFromSurface(renderer, grenade_surface);
	SDL_FreeSurface(grenade_surface);

	SDL_RenderPresent(renderer);
}

void Render::blit_all(fPoint pos_proyectile, fPoint pos_player1, fPoint pos_player2, int actualweapon, float angle) {

	////////// PLAYERS BLIT //////////////
	player1_rect.x = pos_player1.x;
	player1_rect.y = SCREEN_HEIGHT - (pos_player1.y+32);

	player2_rect.x = pos_player2.x;
	player2_rect.y = SCREEN_HEIGHT - (pos_player2.y + 32);

	////////// PROYECTILES || WEAPONS BLIT //////////////
	bazooka_rect.x = player1_rect.x+5;
	bazooka_rect.y = player1_rect.y+50;

	particle_rect.x = pos_proyectile.x;
	particle_rect.y = SCREEN_HEIGHT - (pos_proyectile.y + 15);


	SDL_RenderCopyEx(renderer, player1_tex, &(idle_animation.GetCurrentFrame()), &player1_rect, 0, 0, SDL_FLIP_HORIZONTAL);
	SDL_RenderCopyEx(renderer, player2_tex, &(idle_animation.GetCurrentFrame()), &player2_rect, 0, 0, SDL_FLIP_NONE);
	SDL_RenderCopy(renderer, particle_tex, NULL, &particle_rect);

	////////// CHECK WHAT WEAPON IS USING //////////////
	if (actualweapon == 1) {
		particle_tex = grenade_tex;
	}
	if (actualweapon == 2) {
		particle_tex = cell_tex;
		SDL_RenderCopyEx(renderer, bazooka_tex, NULL, &bazooka_rect, -angle,&center, SDL_FLIP_HORIZONTAL);
	}

	////////// RENDER //////////////
	SDL_RenderPresent(renderer);
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
	SDL_RenderCopy(renderer, texbackground, NULL, &backgroundrect);
}