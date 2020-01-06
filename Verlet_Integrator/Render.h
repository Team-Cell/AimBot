#ifndef __Render_H__
#define __Render_H__

#include "Physics.h"
#include "SDL/include/SDL.h"
#include"SDL_image/include/SDL_image.h"

struct SDL_Renderer;
struct SDL_Texture;

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

class Render : public Particle
{
public:
	Render();
	~Render();

	void Init();
	void blit_all(fPoint pos_proyectile, fPoint pos_player1, fPoint pos_player2, int actual_weapon);
	void Update(fPoint pos);
	void clearScreen();

public:
	SDL_Window *window;
	//SDL_Renderer *renderer;

	SDL_Renderer* renderer = nullptr;
	SDL_Rect* ball;
	SDL_Texture* tex = nullptr;

	//Players Surfaces & Textures
	SDL_Rect player1_rect;
	SDL_Rect player2_rect;
	SDL_Surface *player1_surface;
	SDL_Texture * player1_tex;
	SDL_Surface *player2_surface;
	SDL_Texture * player2_tex;

	//Weapons Surfaces & Textures
	SDL_Rect bazooka_rect;
	SDL_Surface *bazooka_surface;
	SDL_Texture *bazooka_tex;

	//Particle Surfaces & Textures
	SDL_Rect particle_rect;
	SDL_Surface *particle_surface;
	SDL_Texture *particle_tex;
	SDL_Surface *cell_surface;
	SDL_Texture *cell_tex;
	SDL_Surface *grenade_surface;
	SDL_Texture *grenade_tex;

	//Background Surfaces & Textures
	SDL_Rect backgroundrect;
	SDL_Surface *surfacebackground;
	SDL_Texture * texbackground;
};
#endif // !__Render_H__
