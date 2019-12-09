#include <iostream>
#include <stdlib.h>
#include "Verlet.h"
#include "Render.h"
#include "p2Log.h"
#include "SDL/include/SDL.h"
#include "SDL_Image/include/SDL_image.h"
using namespace std;

#pragma comment(lib, "SDL_Image/libx86/SDL2_image.lib")
#pragma comment(lib, "SDL/libx86/SDL2.lib")
#pragma comment(lib, "SDL/libx86/SDL2main.lib")

#define RECTANGLE_THICKNESS 200

void request_data(Verlet& particle, int menu_option, int submenu_option = 0);

int main(int argc, char* args[]) {

	LOG("Starting Integrator");

	int loop_counter = 0;
	int max_loops = 40;
	fPoint temp_pos;
	bool exit = false;
	int last_time;

	Verlet particle;
	ModuleRender render;

	int menu_option = 0;
	int submenu_option = 0;

	float dt = 1.0f;
	float time = 0;
	float current_time = 0;


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
	render.blit_all(0, 0);

	//menu loop
	while ((menu_option != 4) && (menu_option <= 4 || menu_option >= 0)) {
		render.Init();
		//menu
		cout << "MENU" << endl;
		cout << "What do you want to do?" << endl;
		cout << "1. Calculate specific values" << endl;
		cout << "2. Do a graphical representation of the position" << endl;
		cout << "3. Simulate in real time" << endl;
		cout << "4. Quit" << endl;
		cin >> menu_option;
		system("cls");
		switch (menu_option)
		{
		case 1:
			cout << "What do you want to calculate?" << endl;
			cout << "1. Position at a certain time" << endl;
			cout << "2. Time to reach a certain position" << endl;
			cout << "3. Terminal velocity of the particle" << endl;
			cin >> submenu_option;
			system("cls");
			request_data(particle, 1,submenu_option);
			break;
		case 2:
			request_data(particle, 2);
			cout << "Case dt: " << particle.dt << " and a: " << particle.a.x << ", " << -particle.a.y << endl;
			particle.prev_pos = particle.pos;

			//first we calculate the first position of the particle
			particle.pos = Classical_Motion(particle.pos, particle.v, particle.a, dt);

			//main loop
			while (loop_counter < max_loops) {
				//we calculate the sum of the accelerations applied
				particle.a = AccelerationSum(particle);
				//we can now iterate using Verlet
				temp_pos = particle.pos;
				particle.pos = Verlet_Integration(particle.pos, particle.prev_pos, particle.a, dt);
				cout << "Time: " << current_time << "px: " << particle.pos.x << " py: " << SCREEN_HEIGHT - particle.pos.y << " ax: " << particle.a.x << " ay: " << -particle.a.y << endl;
				particle.prev_pos = temp_pos;
				for (int i = 0; i < 4; i++)
				{
					if (CheckCollision(particle, rectangles[i])) {
						cout << "Collision" << endl;
						time += CalculateCollisionPosition(particle, rectangles[i]);
					}
				}
				if (time > 0)
				{
					CalculateCollisionFinalPosition(particle, time);
					time = 0;
				}
				render.blit_all(particle.pos.x, particle.pos.y);
				loop_counter++;
				current_time += particle.dt;
			}
			break;
		case 3:
			//simulation loop
		
			request_data(particle, 3);
			particle.prev_pos = particle.pos;
			particle.pos = Classical_Motion(particle.pos, particle.v, particle.a, dt);
			LOG("particle.v= %f, %f",particle.v.x,particle.v.y);
			last_time = SDL_GetTicks();

			while (exit == false)
			{
				while (1) {
					//dt = (SDL_GetTicks() - last_time) / 1000;
					dt = 0.1;
					last_time = SDL_GetTicks();
					LOG("X: %f, %f", particle.v.x, particle.a.x);
					particle.a = AccelerationSum(particle);
					temp_pos = particle.pos;
					particle.pos = Verlet_Integration(particle.pos, particle.prev_pos, particle.a, dt);
					particle.prev_pos = temp_pos;
					LOG("X: %f, %f", particle.v.x, particle.a.x);
					cout << "px: " << particle.pos.x << " py: " << SCREEN_HEIGHT - particle.pos.y << " ax: " << particle.a.x << " ay: " << -particle.a.y << endl;
					for (int i = 0; i < 4; i++)
					{
						LOG("X: %f, %f", particle.v.x, particle.a.x);
						if (CheckCollision(particle, rectangles[i])) {
							cout << "Collision" << endl;
							time += CalculateCollisionPosition(particle, rectangles[i]);
						}
					}
					LOG("X: %f, %f", particle.v.x, particle.a.x);
					if (time > 0)
					{
						CalculateCollisionFinalPosition(particle, time);
						time = 0;
					}
					render.Update(particle.pos);
					LOG("X: %f, %f", particle.v.x, particle.a.x);

					
					/*while (SDL_PollEvent(&event)) {
						switch (event.type) {
						case SDL_QUIT:
							exit = 1;
							break;
						case SDL_KEYDOWN:
							if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
								exit = 1;
							break;
						}
					}*/
					LOG("X: %f, %f", particle.v.x, particle.a.x);					
				}
			}
		case 4:
			goto END;
			break;
		default:
			break;
		}
		//char_const_acc = 'A';
		//cout << endl << endl;
	}

	END: system("pause");
	return 0;
}

void request_data(Verlet& particle, int menu_option, int submenu_option) {
	float time = 0;
	float dt = 0.1;
	float final_position;
	char axis;
	int axis_option = 0;

	LOOP: if (menu_option == 1)
	{
		switch (submenu_option)
		{
		case 1:
			cout << "Which is the position of the particle? " << endl << "x: ";
			cin >> particle.pos.x;
			cout << "y: ";
			cin >> particle.pos.y;
			cout << "Which is the initial speed of the particle?" << endl << "x: ";
			cin >> particle.v.x;
			cout << "y: ";
			cin >> particle.v.y;
			cout << "Which is the acceleration?" << endl << "x: ";
			cin >> particle.a.x;
			cout << "y: ";
			cin >> particle.a.y;
			cout << "At which time do you want to know the position?" << endl;
			cin >> time;
			particle.pos = Position_at_Time(particle.pos, particle.v, particle.a, time);
			cout << "Final position: (" << particle.pos.x << ", " << particle.pos.y << ")" << " m" <<endl;
			system("pause");
			break;
		case 2: 
			dt = 1;
			cout << "Which is the position of the particle?" << endl << "x: ";
			cin >> particle.pos.x;
			cout << "y: ";
			cin >> particle.pos.y;
			cout << "Which is the initial speed of the particle?" << endl << "x: ";
			cin >> particle.v.x;
			cout << "y: ";
			cin >> particle.v.y;
			cout << "Which is the acceleration of the particle?" << endl << "x: ";
			cin >> particle.a.x;
			cout << "y: ";
			cin >> particle.a.y;
			// We ask which axis and which is the final position, and it returns us the time to reach that position
			cout << "On which axis is the position you want to reach? <x> or <y> ";
			cin >> axis;
			if ((axis == 'x')||(axis =='X'))axis_option = 1;
			if ((axis == 'y')||(axis =='Y'))axis_option = 2;
			cout << "Which is the final position in that axis?"<< endl;
			cin >> final_position;
			cout << "Time: " << Time_To_Position(particle.pos, particle.v,particle.a,dt,final_position, axis_option) << " s" <<endl;
			system("pause");
			break;
		case 3:
			//ask for values
			cout << "Which is the value of the gravity?" << endl;
			cin >> particle.gravity;
			cout << "Which is mass of the particle?" << endl;
			cin >> particle.mass;
			cout << "Which is the density of the air? " << endl;
			cin >> particle.density;
			cout << "Which is the drag coeficient of the air? " << endl;
			cin >> particle.drag_coeficient;
			cout << "Which is the area of the particle?" << endl;
			cin >> particle.area;
			cout << "Terminal velocity: " << Terminal_Velocity(particle.gravity, particle.mass, particle.density, particle.drag_coeficient, particle.area) << " m/s"<< endl;
			system("pause");
			break;
		default:
			goto LOOP;
			break;
		}
		system("cls");
	}else if (menu_option == 2)
	{
		char char_const_acc = 'A';
		cout << "Which is the initial position?:" << endl << "x: ";
		cin >> particle.pos.x;
		cout << "y: ";
		cin >> particle.pos.y;
		particle.pos.y = SCREEN_HEIGHT - particle.pos.y;
		cout << "Which is the initial speed?:" << endl << "x: ";
		cin >> particle.v.x;
		cout << "y: ";
		cin >> particle.v.y;
		particle.v.y = -particle.v.y;
		while ((char_const_acc != 'y') && (char_const_acc != 'Y') && (char_const_acc != 'n') && (char_const_acc != 'N')) {
			cout << "Do you want gravity to be the only acceleration?: " << endl << "(Yes: Y / No: N)" << endl;
			cin >> char_const_acc;
			if ((char_const_acc == 'y') || (char_const_acc == 'Y')) {
				//cout << "Which is the initial acceleration?: " << endl;
				//cin >> particle.a.x >> particle.a.y;
				//particle.a.y = -particle.a.y;
				cout << "Which is the gravity?" << endl;
				cin >> particle.gravity;
				particle.gravity = -particle.gravity;
				particle.a.y = particle.gravity;
			}
			if ((char_const_acc == 'n') || (char_const_acc == 'N')) {
				cout << "Which is the gravity?" << endl;
				cin >> particle.gravity;
				particle.gravity = -particle.gravity;
				cout << "Which is the aerodynamic drag coeficient?: " << endl;
				cin >> particle.drag_coeficient;
				cout << "Which is the density of the air?: " << endl;
				cin >> particle.density;
				cout << "Which is the area?: " << endl;
				cin >> particle.area;
			}
			if ((char_const_acc != 'y') && (char_const_acc != 'Y') && (char_const_acc != 'n') && (char_const_acc != 'N')) {
				cout << endl << "You entered an invalid answer." << endl;
			}
			//breaks if particle.gravity is not a number
		}
		cout << "Which is the mass?:" << endl;
		cin >> particle.mass;
		system("cls");

	}else if (menu_option == 3) {
		char char_const_acc = 'A';
		cout << "Which is the initial position?:" << endl << "x: ";
		cin >> particle.pos.x;
		cout << "y: ";
		cin >> particle.pos.y;
		particle.pos.y = SCREEN_HEIGHT - particle.pos.y;
		cout << "Which is the initial speed?:" << endl << "x: ";
		cin >> particle.v.x;
		cout << "y: "; 
		cin >> particle.v.y;
		particle.v.y = -particle.v.y;
		while ((char_const_acc != 'y') && (char_const_acc != 'Y') && (char_const_acc != 'n') && (char_const_acc != 'N')) {
			cout << "Do you want gravity to be the only acceleration?: " << endl << "(Yes: Y / No: N)" << endl;
			cin >> char_const_acc;
			if ((char_const_acc == 'y') || (char_const_acc == 'Y')) {
				//cout << "Which is the initial acceleration?: " << endl;
				//cin >> particle.a.x >> particle.a.y;
				//particle.a.y = -particle.a.y;
				cout << "Which is the gravity?" << endl;
				cin >> particle.gravity;
				particle.gravity = -particle.gravity;
				particle.a.y = particle.gravity;
			}
			if ((char_const_acc == 'n') || (char_const_acc == 'N')) {
				cout << "Which is the gravity?" << endl;
				cin >> particle.gravity;
				particle.gravity = -particle.gravity;
				cout << "Which is the aerodynamic drag coeficient?: " << endl;
				cin >> particle.drag_coeficient;
				cout << "Which is the density of the air?: " << endl;
				cin >> particle.density;
				cout << "Which is the area?: " << endl;
				cin >> particle.area;
			}
			if ((char_const_acc != 'y') && (char_const_acc != 'Y') && (char_const_acc != 'n') && (char_const_acc != 'N')) {
				
					cout << endl << "You entered an invalid answer." << endl;
				
			}
		}
		cout << "Which is the mass?:" << endl;
		cin >> particle.mass;
		system("cls");
	}
}