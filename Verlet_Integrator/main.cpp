#include <iostream>
#include <stdlib.h>
#include <time.h>
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

void HandleInput(int& Montecarlo, fPoint& worm, Particle& target);

int main(int argc, char* args[]) {

	LOG("Starting Integrator");

	bool exit = false;

	Particle projectile;
	Particle target;
	fPoint worm;

	Render render;

	int min_angle = 0;
	int max_angle = 80;

	float dt = 1.0f;
	float fps = 30;

	int Montecarlo = 10;
	int max_path_iterations = 50;

	Weapon Bazooka(20, 0, true);
	Weapon Grenade(5, 0.8, false);

	//screen limit rectangles
	VRectangle rectangles[4];
	VRectangle top_rectangle(0, -RECTANGLE_THICKNESS, SCREEN_WIDTH + 2 * RECTANGLE_THICKNESS, RECTANGLE_THICKNESS);
	VRectangle left_rectangle(-RECTANGLE_THICKNESS, 0, RECTANGLE_THICKNESS,SCREEN_HEIGHT);
	VRectangle right_rectangle(SCREEN_WIDTH, 0, RECTANGLE_THICKNESS, SCREEN_HEIGHT);
	VRectangle bottom_rectangle(-RECTANGLE_THICKNESS, SCREEN_HEIGHT, SCREEN_WIDTH + 2*RECTANGLE_THICKNESS, RECTANGLE_THICKNESS);
	//VRectangle target;

	rectangles[0] = top_rectangle;
	rectangles[1] = left_rectangle;
	rectangles[2] = right_rectangle;
	rectangles[3] = bottom_rectangle;

	render.Init();
	srand(time(NULL));

	//main loop
	while (exit == false)
	{
		HandleInput(Montecarlo, worm, target);
		float angle;
		for (int i = 0; i < Montecarlo; i++)
		{
			NEXT_MISSILE: cout << "Missile: " << i + 1 << endl;
			projectile.prev_pos = worm;
			angle = rand() % 8001;
			angle *= 0.01;
			cout << "Angle " << angle << endl;

			//Test projectile is a bazooka so no gravity is applied to it
			projectile.pos = Classical_Motion(projectile.prev_pos, Bazooka.initial_speed, angle, {0, 0});
			
			for (int i = 0; i < max_path_iterations; i++)
			{
				//add speed calculations
				fPoint temp = projectile.pos;
				projectile.pos = Verlet_Integration(projectile.pos, projectile.prev_pos, { 0, 0 }, dt);
				cout << "Position x:" << projectile.pos.x << " y: " << projectile.pos.y << endl;
				projectile.prev_pos = temp;

				for (int j = 0; j < 4; j++)
				{
					if (OnCollision(projectile, rectangles[i])) {
						cout << "Collision" << endl;
						i++;
						goto NEXT_MISSILE;
					}
				}

				if (OnCollision(projectile, target)) {
					cout << "Collision" << endl;
					i++;
					goto NEXT_MISSILE;
				}
			}
			cout << endl;
		}

		//for of the final path
		//Render
	}
	system("pause");
	return 0;
}

void HandleInput(int& Montecarlo, fPoint& worm_position, Particle& target) {
	cout << "Which is the initial position of the Worm?" << endl;
	cin >> worm_position.x >> worm_position.y;
	cout << "Which is the target position?" << endl;
	cin >> target.pos.x >> target.pos.y;
	cout << "How many Montecarlo iterations do you want to do? " << endl;
	cin >> Montecarlo;
}
