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

void HandleInput(int& option, int& Montecarlo, fPoint& worm, Particle& target);

int main(int argc, char* args[]) {

	LOG("Starting Integrator");

	bool exit = false;

	Particle projectile;
	Particle target;
	fPoint worm;

	Render render;
	int option = 2;

	int min_angle = 0;
	int max_angle = 80;

	float final_angle = 0;

	float dt = 1.0f;
	float fps = 30;

	int Montecarlo = 10;
	int Montecarlo_iterations = 0;
	int max_path_iterations = 50;

	Weapon Grenade(40, 0.6, false);
	Weapon Bazooka(20, 0, true);
	Weapon* chosen_weapon = nullptr;

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
	srand(time(NULL));

	//main loop
	while (exit == false)
	{
		HandleInput(option, Montecarlo, worm, target);
		if (option == 1) chosen_weapon = &Grenade;
		if (option == 2) chosen_weapon = &Bazooka;

		float angle;

		while (final_angle == 0) {
			Montecarlo_iterations++;
			cout << "Montecarlo n " << Montecarlo_iterations << endl;

			for (int i = 0; i < Montecarlo && (final_angle == 0); i++) {

				cout << "Missile: " << i + 1 << endl;
				projectile.prev_pos.x = worm.x + 5;
				projectile.prev_pos.y = worm.y - 30;
				angle = rand() % max_angle * 100 + 1;
				angle *= 0.01;

				cout << "Angle " << angle << endl;

				//Test projectile is a bazooka so no gravity is applied to it
				projectile.pos = Classical_Motion(projectile.prev_pos, chosen_weapon->initial_speed, angle, { 0, 0 });

				for (int i = 0; i < max_path_iterations; i++)
				{
					//add speed calculations
					fPoint temp = projectile.pos;
					projectile.pos = Verlet_Integration(projectile.pos, projectile.prev_pos, { 0, 0 }, dt);
					//cout << "Position x:" << projectile.pos.x << " y: " << projectile.pos.y << endl;
					projectile.prev_pos = temp;

					for (int j = 0; j < 4; j++)
					{
						if (OnCollision(projectile, rectangles[j])) {
							if (chosen_weapon->bounce_coefficient == 0)
							{

							}
							else
							{
								HandleCollision(projectile, rectangles[j], dt, chosen_weapon->bounce_coefficient);
							}
						}
					}

					if (OnCollision(projectile, target)) {
						cout << "Target hit" << endl;
						final_angle = angle;
						i++;
						break;
					}

					render.blit_all(projectile.pos, worm, target.pos, option, angle);
				}
				cout << endl;
			}
			cout << "=====================" << endl << endl;
		}

		//render bazooka final path
		projectile.prev_pos = worm;
		angle = final_angle;

		cout << "Final angle " << angle << endl;

		projectile.pos = Classical_Motion(projectile.prev_pos, chosen_weapon->initial_speed, angle, { 0, 0 });

		for (int i = 0; i < max_path_iterations; i++)
		{
			//add speed calculations
			fPoint temp = projectile.pos;
			projectile.pos = Verlet_Integration(projectile.pos, projectile.prev_pos, { 0, 0 }, dt);
			projectile.prev_pos = temp;

			if (OnCollision(projectile, target)) {
				break;
			}

			render.blit_all(projectile.pos, worm, target.pos, option,final_angle);
		}
		//
		cout << endl;

		final_angle = 0;
		option = 0;

		render.clearScreen();
	}
	
	system("pause");
	return 0;
}

void HandleInput(int& option, int& Montecarlo, fPoint& worm_position, Particle& target) {

	cout << "Which weapon do you prefer? "<< endl <<"1. Grenade " << endl << "2. Bazooka: " << endl;
	cin >> option;
	cout << "Which is the initial position of the Worm?" << endl;
	cin >> worm_position.x >> worm_position.y;
	cout << "Which is the target position?" << endl;
	cin >> target.pos.x >> target.pos.y;
	cout << "How many Montecarlo iterations do you want to do? " << endl;
	cin >> Montecarlo;
	cout << endl;
	cout << "=====================" << endl << endl;
}
