#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "Physics.h"
#include "Render.h"
#include "Audio.h"
#include "p2Log.h"
#include "SDL/include/SDL.h"
#include "SDL_Image/include/SDL_image.h"
using namespace std;

#pragma comment(lib, "SDL_Image/libx86/SDL2_image.lib")
#pragma comment(lib, "SDL/libx86/SDL2.lib")
#pragma comment(lib, "SDL/libx86/SDL2main.lib")

#define RECTANGLE_THICKNESS 200

void HandleInput(int& option, int& Montecarlo, fPoint& worm, Collider& target);

int main(int argc, char* args[]) {

	LOG("Starting Integrator");

	bool exit = false;

	Particle projectile;
	Collider target = {0,0,66,75};
	fPoint worm;

	Render render;
	PhysicsEngine physics;
	Audio audio;
	
	int option = 1;

	Weapon Grenade(20, 0.6, false, true);
	Weapon Bazooka(20, 0, true, false);
	Weapon* chosen_weapon = nullptr;

	//screen limit rectangles
	Collider rectangles[4];
	Collider top_rectangle(0, -RECTANGLE_THICKNESS, SCREEN_WIDTH + 2 * RECTANGLE_THICKNESS, RECTANGLE_THICKNESS);
	Collider left_rectangle(-RECTANGLE_THICKNESS, 0, RECTANGLE_THICKNESS,SCREEN_HEIGHT);
	Collider right_rectangle(SCREEN_WIDTH, 0, RECTANGLE_THICKNESS, SCREEN_HEIGHT);
	Collider bottom_rectangle(-RECTANGLE_THICKNESS, SCREEN_HEIGHT, SCREEN_WIDTH + 2*RECTANGLE_THICKNESS, RECTANGLE_THICKNESS);

	rectangles[0] = top_rectangle;
	rectangles[1] = left_rectangle;
	rectangles[2] = right_rectangle;
	rectangles[3] = bottom_rectangle;

	render.Init();
	audio.Init();

	srand(time(NULL));
	//audio.PlayMusic("Music/worms.ogg");
	audio.LoadFx("Music/Explosion.wav");
	audio.LoadFx("Music/Grenade_throwing.wav");
	audio.LoadFx("Music/Bazooka_throwing.wav");

	//main loop
	while (exit == false)
	{
		HandleInput(option, physics.Montecarlo, worm, target);

		if (option == 1) chosen_weapon = &Grenade;
		if (option == 2) chosen_weapon = &Bazooka;

		if (chosen_weapon->wind_activated == true) {
			projectile.a += {0, physics.wind_acceleration};
		}
		if (chosen_weapon->linear_trajectory == false) {
			projectile.a += { 0, -GRAVITY };
		}
		float angle;

		//find projectile path to hit the target
		while ((physics.final_angle == 0) && (physics.Montecarlo_iterations < physics.Max_Montecarlo)) {
			physics.Montecarlo_iterations++;
			cout << "Montecarlo n " << physics.Montecarlo_iterations << endl;

			for (int i = 0; i < physics.Montecarlo && (physics.final_angle == 0); i++) {

				cout << "Missile: " << i + 1 << endl;

				projectile.prev_pos.x = worm.x + 5;
				projectile.prev_pos.y = worm.y - 30;

				angle = rand() % (physics.max_angle - physics.min_angle) * 100 + 1;
				angle += physics.min_angle;
				angle *= 0.01;
				cout << "Angle " << angle << endl;

				if (chosen_weapon == &Grenade)
					projectile.pos = Classical_Motion(projectile.prev_pos, chosen_weapon->initial_speed, angle, projectile.a, false);
														
				if (chosen_weapon == &Bazooka)
					projectile.pos = Classical_Motion(projectile.prev_pos, chosen_weapon->initial_speed, angle, projectile.a, false);
				
				for (int i = 0; i < physics.max_path_iterations; i++)
				{
					fPoint temp = projectile.pos;
					projectile.pos = Verlet_Integration(projectile.pos, projectile.prev_pos, projectile.a, physics.dt);
					projectile.prev_pos = temp;

					for (int j = 0; j < 4; j++)
					{
						if (OnCollision(projectile, rectangles[j])) {
							if (chosen_weapon->bounce_coefficient != 0)
							{
								HandleCollision(projectile, rectangles[j], physics.dt, chosen_weapon->bounce_coefficient);
							}
							else
							{
								i++;
								break;
							}
						}
					}

					if (OnCollision(projectile, target)) {
						cout << "Target hit" << endl;
						physics.final_angle = angle;
						i++;
						break;
					}

					//debug draw
					render.blit_all(projectile.pos, worm, { target.x, target.y }, option, angle);
				}
				cout << endl;
			}
			cout << "=====================" << endl << endl;
		}

		//render final path

		projectile.prev_pos.x = worm.x + 5;
		projectile.prev_pos.y = worm.y - 30;
		angle = physics.final_angle;
		if (physics.final_angle == 0) angle = 90;

		cout << "Final angle " << angle << endl;

		if (chosen_weapon == &Grenade)
				projectile.pos = Classical_Motion(projectile.prev_pos, chosen_weapon->initial_speed, angle, projectile.a, true);
			
		if (chosen_weapon == &Bazooka)
				projectile.pos = Classical_Motion(projectile.prev_pos, chosen_weapon->initial_speed, angle, projectile.a, false);
						

		for (int i = 0; i < 300; i++)
		{
			fPoint temp = projectile.pos;
			projectile.pos = Verlet_Integration(projectile.pos, projectile.prev_pos, projectile.a, physics.dt);
			projectile.prev_pos = temp;
			for (int j = 0; j < 4; j++)
			{
				if (OnCollision(projectile, rectangles[j])) {
					if (chosen_weapon->bounce_coefficient != 0)
					{
						HandleCollision(projectile, rectangles[j], physics.dt, chosen_weapon->bounce_coefficient);
					}
					else
					{
						i++;
						break;
					}
				}
			}
			if (OnCollision(projectile, target)) {
				audio.PlayFx(1);
				break;
			}
			render.blit_all(projectile.pos, worm, {target.x, target.y}, option, physics.final_angle);
		}
		//
		cout << endl;

		physics.Montecarlo_iterations = 0;
		physics.final_angle = 0;
		option = 0;

		render.clearScreen();
	}

	system("pause");
	return 0;
}

void HandleInput(int& option, int& Montecarlo, fPoint& worm_position, Collider& target) {

	cout << "Which weapon do you prefer? "<< endl <<"1. Grenade " << endl << "2. Bazooka: " << endl;
	cin >> option;
	cout << "Which is the initial position of the Worm?" << endl;
	cin >> worm_position.x >> worm_position.y;
	cout << "Which is the target position?" << endl;
	cin >> target.x >> target.y;
	cout << "How many Montecarlo iterations do you want to do? " << endl;
	cin >> Montecarlo;
	cout << endl;
	cout << "=====================" << endl << endl;
}
