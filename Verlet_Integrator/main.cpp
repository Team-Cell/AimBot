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

void HandleInput(int& option, int& Montecarlo, fPoint& worm, SDL_Rect& target);

int main(int argc, char* args[]) {

	LOG("Starting Integrator");

	bool exit = false;

	Particle projectile;
	SDL_Rect target = { 0,0,66,75 };
	fPoint worm;

	Render render;
	PhysicsEngine physics;
	Audio audio;

	int option = 1;
	float timer = 0;
	float dt = 0;

	Weapon Grenade(20, 0.6, false, true);
	Weapon Bazooka(20, 0, true, false);

	//screen limit rectangles
	SDL_Rect rectangles[7];
	SDL_Rect top_rectangle = { 0, -RECTANGLE_THICKNESS, SCREEN_WIDTH + 2 * RECTANGLE_THICKNESS, RECTANGLE_THICKNESS };
	SDL_Rect left_rectangle = {-RECTANGLE_THICKNESS, 0, RECTANGLE_THICKNESS, SCREEN_HEIGHT};
	SDL_Rect right_rectangle = { SCREEN_WIDTH, 0, RECTANGLE_THICKNESS, SCREEN_HEIGHT };
	SDL_Rect bottom_rectangle = { -RECTANGLE_THICKNESS, SCREEN_HEIGHT, SCREEN_WIDTH + 2 * RECTANGLE_THICKNESS, RECTANGLE_THICKNESS };
	SDL_Rect explosion_rect = {-1000,-1000, 125, 125};

	rectangles[0] = top_rectangle;
	rectangles[1] = left_rectangle;
	rectangles[2] = right_rectangle;
	rectangles[3] = bottom_rectangle;
	rectangles[6] = explosion_rect;

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
		int option = 1;
		HandleInput(option, physics.Montecarlo, worm, target);

		//worm platform
		rectangles[4] = {(int) worm.x - 50,800 - (int)worm.y + 70, 168, 45 };
		//target platform
		rectangles[5] = { (int)target.x - 50, 800- (int)target.y + target.h, 168, 45 };

		if (option == 1) projectile.weapon = &Grenade;
		if (option == 2) projectile.weapon = &Bazooka;

		if (projectile.weapon->wind_activated == true) {
			projectile.a += {0, physics.wind_acceleration};
		}
		if (projectile.weapon->linear_trajectory == false) {
			projectile.a += { 0, -GRAVITY };
		}
		float angle;

		//find projectile path to hit the target ================================================================================

		while ((physics.final_angle == 0) && (physics.Montecarlo_iterations < physics.Max_Montecarlo)) {
			physics.Montecarlo_iterations++;
			cout << "Montecarlo n " << physics.Montecarlo_iterations << endl;

			for (int i = 0; i < physics.Montecarlo && (physics.final_angle == 0); i++) {

				cout << "Missile: " << i + 1 << endl;

				angle = rand() % (physics.max_angle - physics.min_angle) * 100 + 1;
				angle += physics.min_angle;
				angle *= 0.01;
				cout << "Angle: " << angle << endl;

				if (projectile.weapon == &Grenade)
				{
					float speed_difference = rand() % (physics.max_speed - physics.min_speed);
					projectile.weapon->initial_speed = physics.min_speed + speed_difference;
					cout << "Speed: " << projectile.weapon->initial_speed << endl;
				}

				projectile.prev_pos.x = worm.x + 5;
				projectile.prev_pos.y = worm.y - 30;

				if (projectile.weapon == &Grenade)
					projectile.pos = Classical_Motion(projectile.prev_pos, projectile.weapon->initial_speed, angle, projectile.a, true);
				if (projectile.weapon == &Bazooka)
					projectile.pos = Classical_Motion(projectile.prev_pos, projectile.weapon->initial_speed, angle, projectile.a, false);
				timer = SDL_GetTicks();

				for (int i = 0; i < physics.max_path_iterations; i++)
				{
					fPoint temp = projectile.pos;
					projectile.pos = Verlet_Integration(projectile.pos, projectile.prev_pos, projectile.a, physics.dt, DRAG_COEFFICIENT);
					projectile.prev_pos = temp;

					for (int j = 0; j < 6; j++)
					{
						if (OnCollision(projectile, rectangles[j])) {
							if (projectile.weapon->bounce_coefficient != 0)
							{
								HandleCollision(projectile, rectangles[j], physics.dt, projectile.weapon->bounce_coefficient);
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
						physics.final_speed = projectile.weapon->initial_speed;
						i++;
						explosion_rect.x = projectile.pos.x - 45;
						explosion_rect.y = 790 - projectile.pos.y - 50;
						render.DrawQuad(explosion_rect, 0, 0, 255, 100);
						render.printExplosion(explosion_rect);
						SDL_RenderPresent(render.renderer);
						break;
					}

					//debug draw
					dt = SDL_GetTicks();
					dt -= timer;
					if (dt > 0)
					{
						render.delay = render.cap_miliseconds - dt;
						SDL_Delay(render.delay);
					}
					render.blit_all(projectile.pos, worm, { (float)target.x,(float)target.y }, option, angle);
					render.DrawQuad(rectangles[4], 0, 0, 255, 255);
					render.DrawQuad(rectangles[5], 0, 0, 255, 255);
					SDL_RenderPresent(render.renderer);
					timer = SDL_GetTicks();
				}

				//colateral explosion
				if (physics.final_angle == 0)
				{
					explosion_rect.x = projectile.pos.x - 45;
					explosion_rect.y = 790 - projectile.pos.y - 50;
					render.printExplosion(explosion_rect);
					SDL_RenderPresent(render.renderer);

					if (OnCollision(target, explosion_rect))
					{
						cout << "Target hit by explosion" << endl;
						physics.final_angle = angle;
						physics.final_speed = projectile.weapon->initial_speed;
					}
				}
				cout << endl;
			}
			cout << "=====================" << endl << endl;
		}

		//render final path =====================================================================================================

		angle = physics.final_angle;
		if (physics.final_angle == 0) angle = 90;

		cout << "Final angle: " << angle << endl;
		cout << "Final speed: " << physics.final_speed << endl;

		projectile.prev_pos.x = worm.x + 5;
		projectile.prev_pos.y = worm.y - 30;

		if (projectile.weapon == &Grenade)
				projectile.pos = Classical_Motion(projectile.prev_pos, projectile.weapon->initial_speed, angle, projectile.a, true);
		if (projectile.weapon == &Bazooka)
				projectile.pos = Classical_Motion(projectile.prev_pos, projectile.weapon->initial_speed, angle, projectile.a, false);

		timer = SDL_GetTicks();
		for (int i = 0; i < 300; i++)
		{
			fPoint temp = projectile.pos;
			projectile.pos = Verlet_Integration(projectile.pos, projectile.prev_pos, projectile.a, physics.dt, DRAG_COEFFICIENT);
			projectile.prev_pos = temp;

			for (int j = 0; j < 6; j++)
			{
				if (OnCollision(projectile, rectangles[j])) {
					if (projectile.weapon->bounce_coefficient != 0)
					{
						HandleCollision(projectile, rectangles[j], physics.dt, projectile.weapon->bounce_coefficient);
					}
					else
					{
						i++;
						break;
					}
				}
			}
			if (OnCollision(projectile, target)) {
				render.printExplosion(explosion_rect);
				SDL_RenderPresent(render.renderer);
				audio.PlayFx(1);
				break;
			}

			dt = SDL_GetTicks();
			dt -= timer;
			if (dt > 0)
			{
				render.delay = render.cap_miliseconds - dt;
				SDL_Delay(render.delay);
			}
			render.blit_all(projectile.pos, worm, {(float)target.x, (float)target.y}, option, physics.final_angle);
			timer = SDL_GetTicks();
		}

		cout << endl;
		physics.Montecarlo_iterations = 0;
		physics.final_angle = 0;
		option = 0;
		render.clearScreen();
	}

	system("pause");
	return 0;
}

void HandleInput(int& option, int& Montecarlo, fPoint& worm_position, SDL_Rect& target) {

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
