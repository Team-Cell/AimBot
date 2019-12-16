#include <iostream>
#include <stdlib.h>
#include "Physics.h"
#include "Render.h"
#include "p2Log.h"
#include "SDL/include/SDL.h"
#include "SDL_Image/include/SDL_image.h"
using namespace std;

#pragma comment(lib, "SDL_Image/libx86/SDL2_image.lib")
#pragma comment(lib, "SDL/libx86/SDL2.lib")
#pragma comment(lib, "SDL/libx86/SDL2main.lib")

#define RECTANGLE_THICKNESS 200

void HandleInput(int& Montecarlo, fPoint& worm, fPoint& target);

int main(int argc, char* args[]) {

	LOG("Starting Integrator");

	bool exit = false;

	Particle missile;
	fPoint worm;
	fPoint target;
	Render render;

	float dt = 1.0f;
	float fps = 30;
	int Montecarlo = 10;

	//screen limit rectangles
	VRectangle rectangles[4];
	VRectangle top_rectangle(0, -RECTANGLE_THICKNESS, SCREEN_WIDTH + 2 * RECTANGLE_THICKNESS, RECTANGLE_THICKNESS);
	VRectangle left_rectangle(-RECTANGLE_THICKNESS, 0, RECTANGLE_THICKNESS,SCREEN_HEIGHT);
	VRectangle right_rectangle(SCREEN_WIDTH, 0, RECTANGLE_THICKNESS, SCREEN_HEIGHT);
	VRectangle bottom_rectangle(-RECTANGLE_THICKNESS, SCREEN_HEIGHT, SCREEN_WIDTH + 2*RECTANGLE_THICKNESS, RECTANGLE_THICKNESS);

	rectangles[0] = top_rectangle;
	rectangles[1] = left_rectangle;
	rectangles[2] = right_rectangle;
	rectangles[3] = bottom_rectangle;

	render.Init();

	//main loop
	while (SDL_GetTicks() < 10000)
	{
		HandleInput(Montecarlo, worm, target);

		for (int i = 0; i < Montecarlo; i++)
		{
			cout << "Worm: "<< worm.x << "," << worm.y << endl;
			cout << "Target: " << target.x <<","<< target.y << endl;
			cout << i + 1 << endl;
		}
		//for
		//Render
	}
	system("pause");
	return 0;
}

void HandleInput(int& Montecarlo, fPoint& worm_position, fPoint& target_position) {
	cout << "Which is the initial position of the Worm?" << endl;
	cin >> worm_position.x >> worm_position.y;
	cout << "Which is the target position?" << endl;
	cin >> target_position.x >> target_position.y;
	cout << "How many Montecarlo iterations do you want to do? " << endl;
	cin >> Montecarlo;
}
