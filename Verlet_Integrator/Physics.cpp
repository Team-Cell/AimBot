#include "Physics.h"
#include "Render.h"
#include "p2Point.h"
#include "p2Log.h"
#include <math.h>
#include <iostream>
#include "Globals.h"
using namespace std;

Particle::Particle() {
	pos = { 0,0 };
	prev_pos = { 0,0 };
	v = { 0,0 };
	a = { 0,0 };
	area = 1;
	density = 1;
	dt = 1;
	mass = 0;
	drag_coeficient = 0;
	w = 32;
	h = 32;
	gravity = 0;
	tf = 5;
	fps = 30;
	wind = { 10, 0 };
}

Particle::~Particle() {}

#define RECTANGLE_THICKNESS 200

VRectangle rectangles[4];
VRectangle top_rectangle(0, -RECTANGLE_THICKNESS, SCREEN_WIDTH + 2 * RECTANGLE_THICKNESS, RECTANGLE_THICKNESS);
VRectangle left_rectangle(-RECTANGLE_THICKNESS, 0, RECTANGLE_THICKNESS, SCREEN_HEIGHT);
VRectangle right_rectangle(SCREEN_WIDTH, 0, RECTANGLE_THICKNESS, SCREEN_HEIGHT);
VRectangle bottom_rectangle(-RECTANGLE_THICKNESS, SCREEN_HEIGHT, SCREEN_WIDTH + 2 * RECTANGLE_THICKNESS, RECTANGLE_THICKNESS);

//main verlet

fPoint Verlet_Integration(fPoint pos, fPoint prev_pos, fPoint a, float dt) {

	pos = pos + (pos - prev_pos) + a * dt * dt;

	return pos;
}

fPoint Velocity_Verlet(fPoint vi, fPoint ai, fPoint a_new, float dt) {

	fPoint v_new;

	v_new = vi + ((ai + a_new) * 0.5) * dt;

	cout << " vx: " << v_new.x << " vy: " << v_new.y << " ax: " << a_new.x << " ay: " << -a_new.y << endl;

	return v_new;
}

fPoint Stormer_Verlet(fPoint pos, fPoint prev_pos,fPoint a, float dt) {

	fPoint v_new;
	
	if (a.x == 0 && a.y == 0) {
		v_new = (pos - prev_pos) / dt;
	}
	else if (a.x == 0) {
		v_new.y = (pos.y - prev_pos.y - 0.5 * a.y * dt * dt) / dt;
		v_new.x = (pos.x - prev_pos.x) / dt;
	}
	else if (a.y == 0) {
		v_new.x = (pos.x - prev_pos.x - 0.5 * a.x * dt * dt) / dt;
		v_new.y = (pos.y - prev_pos.y) / dt;
	}
	else {
		v_new = (pos - prev_pos - a * dt * dt * 0.5) / dt;
	}
	cout << "px: " << pos.x << " py: " << pos.y << " vx: " << v_new.x << " vy: " << v_new.y << endl;

	return v_new;
}

fPoint Verlet_Acceleration(float m, fPoint total_f) {
	fPoint a_new;

	a_new = total_f / m;

	return a_new;
}

//acceleration and velocity

fPoint DragAcceleration(float density, float drag_coefficient, float area, fPoint speed, float mass) {
	fPoint acceleration;
	acceleration.x = ((0.5 * density * drag_coefficient * area) * (speed.x * speed.x)) / mass;
	acceleration.y = ((0.5 * density * drag_coefficient * area) * (speed.y * speed.y)) / mass;
	return acceleration;
}

fPoint Calculate_Acceleration(fPoint vi, fPoint vf, float dt) {

	fPoint af;

	af.x = (vf.x - vi.x) / dt;
	af.y = (vf.y - vi.y) / dt;

	return af;
}

fPoint Calculate_Acceleration(fPoint force, float mass) {

	fPoint af;

	af.x = force.x / mass;
	af.y = force.y / mass;

	return af;
}

fPoint AccelerationSum(Particle particle) {
	fPoint accelerationSum;
	if (particle.density != 0)
	{
		accelerationSum.x = DragAcceleration(particle.density, particle.drag_coeficient, particle.area, particle.v, particle.mass).x;
		accelerationSum.y = particle.gravity + DragAcceleration(particle.density, particle.drag_coeficient, particle.area, particle.v, particle.mass).y;
		/* +Freefall_Acceleration(particle.gravity, particle.mass, particle.drag_coeficient) /*+ Parachutist_Acceleration(particle.mass, particle.v.y, particle.gravity, particle.k)*/;
		/*To calculate gravitational acceleration we can use the new function of calculate acceleration with the force being the gravity and the mass the bullet's mass (it could
		change depending of the weapon used)*/
	}
	else
	{
		accelerationSum = particle.a;
	}

	return accelerationSum;
}

float Freefall_Speed(float gravity, float m, float air_density, float area, float friction_const) {
	float speed;

	speed = sqrt((2 * m * gravity) / (air_density * area * friction_const));

	return speed;
}

float Freefall_Acceleration(float gravity, float m, float friction_const) {
	float acceleration;

	acceleration = (m * gravity - m * gravity * friction_const) / m;

	return acceleration;
}

//position calculators

fPoint Classical_Motion(fPoint position, float initial_velocity, float angle, fPoint acceleration, bool gravity, float dt) {
	fPoint final_position;
	fPoint velocity;

	//CONVERT TO ANGLES
	velocity.x = initial_velocity * cos(angle * PI / 180);
	velocity.y = initial_velocity * sin(angle * PI / 180);

	if (gravity) velocity.y -= GRAVITY;

	final_position.x = position.x + velocity.x * dt + 0.5f * acceleration.x * dt * dt;
	final_position.y = position.y + velocity.y * dt + 0.5f * acceleration.y * dt * dt;

	return final_position;
}

float CalculateTrajectory(Particle projectile, Particle target, float angle, Weapon* chosen_weapon) {
	rectangles[0] = top_rectangle;
	rectangles[1] = left_rectangle;
	rectangles[2] = right_rectangle;
	rectangles[3] = bottom_rectangle;

	float final_angle = 0;

	if (chosen_weapon == &Grenade)
		projectile.pos = Classical_Motion(projectile.prev_pos, chosen_weapon->initial_speed, angle, a, false);
	if (chosen_weapon == &Bazooka)
		projectile.pos = Classical_Motion(projectile.prev_pos, chosen_weapon->initial_speed, angle, a, false);
	for (int i = 0; i < max_path_iterations; i++)
	{
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
			cout << "Target hit" << endl;
			final_angle = angle;
			i++;
			return  final_angle;
			break;
		}
	}
}

//additional formulas

fPoint Forces_Sum(fPoint f1, fPoint f2) {
	fPoint total_f;

	total_f = f1 + f2;

	return total_f;
}

float Module(fPoint var) {

	return sqrt(var.x * var.x + var.y * var.y);

}

//This while could be used to calculate a number of forces before sending to the Verlet_Acceleration function
/*p2Point<float> Calculate_Total_Forces(int number_forces) {
	while (number_forces > 1) {
		Calculate_Two_Forces()
		number_forces--;
	}
}*/

//here we calculate inicialize the inicial variables in orther to calculate physics later
void Physics(float tf, int fps, float dt, float gravity, float mass, fPoint v, fPoint wind, float density)
{
	/*
	float ff = tf * fps;
	
	for ( int i = 1; i < ff; i++)
	{
		//compute frame time step
		dt = 1.0 / fps;

		float Fg = mass * gravity;

		fPoint vw = v - wind;
		fPoint vu = vw / sqrt(pow(vw.x, 2), pow(vw.y, 2));
		float Fd = 0.5 * density * vw.x * vw.y *

	}
	*/
}

// Collsions
bool OnCollision(Particle particle, VRectangle rectangle) {
	return (particle.pos.x < rectangle.x + rectangle.w &&
		particle.pos.x + particle.w > rectangle.x &&
		particle.pos.y < rectangle.y + rectangle.h &&
		 particle.pos.y + particle.h > rectangle.y);
}

bool OnCollision(Particle projectile, Particle target) {
	return (projectile.pos.x < target.pos.x + target.w &&
		projectile.pos.x + projectile.w > target.pos.x &&
		projectile.pos.y < target.pos.y + target.h &&
		projectile.pos.y + projectile.h > target.pos.y);
}

void HandleCollision(Particle& particle, VRectangle rect, float dt, float bounce_coefficient) {
	int COLLIDER_MARGIN = 10;
	particle.v.x =    (particle.pos.x - particle.prev_pos.x) / dt;
	particle.v.y =  - (particle.pos.y - particle.prev_pos.y) / dt;

	//going down
	if (particle.v.y < 0)
	{
		//colliding with the floor
		if (particle.pos.y < rect.y + COLLIDER_MARGIN)
		{
			particle.v.y = -particle.v.y * bounce_coefficient;
		}
		//colliding with one of the two walls
		else
		{
			particle.v.x = -particle.v.x * bounce_coefficient;
		}
	}

	//going up
	else
	{
		//colliding with ceiling
		if (particle.pos.y < rect.y + rect.h - COLLIDER_MARGIN)
		{
			particle.v.y = -particle.v.y * bounce_coefficient;
		}
		//colliding with one of the two walls
		else
		{
			particle.v.x = -particle.v.x * bounce_coefficient;
		}
	}

	particle.prev_pos.x = particle.pos.x;
	particle.pos.x = particle.pos.x + particle.v.x * dt;

	particle.prev_pos.y = particle.pos.y;
	particle.pos.y = particle.pos.y - particle.v.y * dt;
}

/*
if ((particle.v.y < 0) && (particle.pos.x > rect.x) && particle.pos.x < rect.x + rect.w)
{
	particle.v.y = -particle.v.y;
}
	//coming from up

	//coming from down

	//coming from left

	//coming from right
*/
fPoint AddWind(fPoint a, float wind) {
	a.x -= wind;
	return a;
}
fPoint AddGravity(fPoint a) {
	a.y -= 0.5;
	return a;
}