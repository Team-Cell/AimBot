#include "Physics.h"
#include "Render.h"
#include "p2Point.h"
#include "p2Log.h"
#include <math.h>
#include <iostream>
using namespace std;

Particle::Particle() {
	pos = { 0,0 };
	prev_pos = { 0,0 };
	v = { 0,0 };
	a = { 0,0 };
	area = 1;
	density = 1;
	dt = 1;
	mass = 400;
	w = 32;
	h = 32;
	weapon = nullptr;
}

Particle::~Particle() {}

PhysicsEngine::PhysicsEngine()
{
	dt = 1.0f;

	Montecarlo = 10;
	Montecarlo_iterations = 0;
	Max_Montecarlo = 5;
	max_path_iterations = 130;

	min_angle = 0;
	max_angle = 80;
	final_angle = 0;

	min_speed = 10;
	max_speed = 40;
	final_speed = 0;

	wind_acceleration = 0.3f;
}

PhysicsEngine::~PhysicsEngine()
{
}



//Calculate position with drag
fPoint CalculatePos(fPoint pos, fPoint prev_pos, fPoint ai, float dt, float area, float mass, float v_projectile, float angle, bool classical_motion){
	fPoint new_pos = pos;
	if (classical_motion == true) {
		new_pos = Classical_Motion(pos, v_projectile, angle, ai);
	}
	else {
		new_pos = Verlet_Integration(pos, prev_pos, ai, dt);
	}
	fPoint new_v = Stormer_Verlet(new_pos, pos, ai, dt);
	float drag_multiplier = 0.5 * AIR_DENSITY * area * DRAG_COEFFICIENT;
	fPoint Fd = { new_v.x * new_v.x * drag_multiplier,new_v.y * new_v.x * drag_multiplier };
	fPoint a_drag = { Fd.x / mass,Fd.y / mass };
	ai -= a_drag;

	if (classical_motion == true) {
		new_pos = Classical_Motion(pos, v_projectile, angle, ai);
	}
	else {
		new_pos = Verlet_Integration(pos, prev_pos, ai, dt);
	}
	return new_pos;
}

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

	return v_new;
}

fPoint Verlet_Acceleration(float m, fPoint total_f) {
	fPoint a_new;

	a_new = total_f / m;

	return a_new;
}

//acceleration and velocity

fPoint DragAcceleration(float density, float area, fPoint speed, float mass) {
	fPoint acceleration;
	acceleration.x = ((0.5 * density * DRAG_COEFFICIENT * area) * (speed.x * speed.x)) / mass;
	acceleration.y = ((0.5 * density * DRAG_COEFFICIENT * area) * (speed.y * speed.y)) / mass;
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
		accelerationSum.x = DragAcceleration(particle.density, particle.area, particle.v, particle.mass).x;
		accelerationSum.y = particle.gravity + DragAcceleration(particle.density, particle.area, particle.v, particle.mass).y;
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

fPoint Classical_Motion(fPoint position, float initial_velocity, float angle, fPoint acceleration, float dt) {
	fPoint final_position;
	fPoint velocity;

	//CONVERT TO ANGLES
	velocity.x = initial_velocity * cos(angle * PI / 180);
	velocity.y = initial_velocity * sin(angle * PI / 180);

	final_position.x = position.x + velocity.x * dt + 0.5f * acceleration.x * dt * dt;
	final_position.y = position.y + velocity.y * dt + 0.5f * acceleration.y * dt * dt;

	return final_position;
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

// Collsions
bool OnCollision(Particle particle, SDL_Rect rectangle) {
	return (particle.pos.x < rectangle.x + rectangle.w &&
		particle.pos.x + particle.w > rectangle.x &&
		particle.pos.y < rectangle.y + rectangle.h &&
		 particle.pos.y + particle.h > rectangle.y);
}

bool OnCollision(SDL_Rect rect, SDL_Rect rectangle) {
	return (rect.x < rectangle.x + rectangle.w &&
		rect.x + rect.w > rectangle.x &&
		rect.y < rectangle.y + rectangle.h &&
		rect.y + rect.h > rectangle.y);
}

void HandleCollision(Particle& particle, SDL_Rect rect, float dt, float bounce_coefficient) {
	particle.v.x = (particle.pos.x - particle.prev_pos.x) / dt;
	particle.v.y = -(particle.pos.y - particle.prev_pos.y) / dt;
	rect.y = 800 - rect.y;

	//colliding from up
	if ((particle.pos.y - particle.h < rect.y + rect.h) && (particle.pos.x > rect.x) && (particle.pos.y > rect.y)){
			particle.v.y = -particle.v.y * bounce_coefficient;
	}

	//colliding from down
	if ((particle.pos.y + particle.h > rect.y - rect.h) && (particle.pos.y < rect.y) && (particle.pos.x > rect.x)) {
		particle.v.y = -particle.v.y * bounce_coefficient;
	}

	//colliding from left
	if ((particle.pos.x + particle.w > rect.x) && (particle.pos.y < rect.y) && (particle.pos.x < rect.x + rect.w)){
			particle.v.x = -particle.v.x * bounce_coefficient;
	}

	//colliding from right
	if ((particle.pos.x > rect.x) && (particle.pos.y < rect.y) && (particle.pos.x < rect.x + rect.w)){
			particle.v.x = -particle.v.x * bounce_coefficient;
	}

	particle.prev_pos.x = particle.pos.x;
	particle.pos.x = particle.pos.x + particle.v.x * dt;

	particle.prev_pos.y = particle.pos.y;
	particle.pos.y = particle.pos.y - particle.v.y * dt;
}
