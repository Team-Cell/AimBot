#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "Physics.h"
#include "Render.h"
#include "p2Log.h"
#include "Globals.h"
using namespace std;

#include "SDL/include/SDL.h"
#include "SDL_Image/include/SDL_image.h"

#pragma comment(lib, "SDL_Image/libx86/SDL2_image.lib")
#pragma comment(lib, "SDL/libx86/SDL2.lib")
#pragma comment(lib, "SDL/libx86/SDL2main.lib")

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

	float wind_acceleration = 0.2;

	float final_angle = 0;

	Weapon* chosen_weapon = nullptr;

	render.Init();
	srand(time(NULL));

	//main loop
	while (exit == false)
	{
		HandleInput(option, Montecarlo, worm, target);
		if (option == 1) chosen_weapon = &Grenade;
		if (option == 2) chosen_weapon = &Bazooka;

		fPoint a = { 0,0 };
		if (chosen_weapon->wind_activated == true) {
			a.x -= wind_acceleration;
		}
		if (chosen_weapon->linear_trajectory == false) {
			a.y -= 0.5;
		}

		float angle;

		while ((final_angle == 0)&&(Montecarlo_iterations < Max_Montecarlos)) {
			Montecarlo_iterations++;
			cout << "Montecarlo n " << Montecarlo_iterations << endl;

			for (int i = 0; i < Montecarlo && (final_angle == 0); i++) {

				cout << "Missile: " << i + 1 << endl;
				projectile.prev_pos.x = worm.x + 5;
				projectile.prev_pos.y = worm.y - 30;
				angle = rand() % max_angle * 100 + 1;
				angle *= 0.01;

				cout << "Angle " << angle << endl;

				CalculateTrajectory(projectile, angle, chosen_weapon);

				render.blit_all(projectile.pos, worm, target.pos, option, angle);
				}
				cout << endl;
			}
			cout << "=====================" << endl << endl;

		if (final_angle == 0)
		{
			final_angle = 90;
		}

		//render bazooka final path
		projectile.prev_pos.x = worm.x + 5;
		projectile.prev_pos.y = worm.y - 30;
		angle = final_angle;

		cout << "Final angle " << angle << endl;

		if (chosen_weapon == &Grenade)
				projectile.pos = Classical_Motion(projectile.prev_pos, chosen_weapon->initial_speed, angle, a, true);
		if (chosen_weapon == &Bazooka)
				projectile.pos = Classical_Motion(projectile.prev_pos, chosen_weapon->initial_speed, angle, a, false);

		for (int i = 0; i < max_path_iterations; i++)
		{
			//add speed calculations
			fPoint temp = projectile.pos;
			projectile.pos = Verlet_Integration(projectile.pos, projectile.prev_pos, a, dt);
			projectile.prev_pos = temp;
			for (int j = 0; j < 4; j++)
			{
				if (OnCollision(projectile, rectangles[j])) {
					if (chosen_weapon->bounce_coefficient != 0)
					{
						HandleCollision(projectile, rectangles[j], dt, chosen_weapon->bounce_coefficient);
					}
				}
			}
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
