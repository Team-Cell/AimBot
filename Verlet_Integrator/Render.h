#ifndef __ModuleRenderer_H__
#define __ModuleRenderer_H__

#include "Verlet.h"
#include "SDL/include/SDL.h"
#include"SDL_image/include/SDL_image.h"

struct SDL_Renderer;
struct SDL_Texture;

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

class ModuleRender : public Verlet
{
public:
	ModuleRender();
	~ModuleRender();

	bool Render();
	void Init();
	void blit_all(float, float);
	void Update(fPoint pos);
	void clearScreen();

public:
	SDL_Window *window;
	//SDL_Renderer *renderer;

	SDL_Renderer* renderer = nullptr;
	SDL_Rect* ball;
	SDL_Texture* tex = nullptr;

	//All Ball Propeties to Render
	SDL_Rect particle_rect;
	SDL_Surface *particle_surface;
	SDL_Texture * particle_tex;

	SDL_Rect backgroundrect;
	SDL_Surface *surfacebackground;
	SDL_Texture * texbackground;
};
#endif // !__ModuleRenderer_H__
