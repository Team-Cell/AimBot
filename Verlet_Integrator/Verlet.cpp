#include <iostream>
#include "Verlet.h"
#include "Render.h"
#include "p2Point.h"
#include "p2Log.h"
#include <math.h>
using namespace std;

Verlet::Verlet() {
	pos = { 0,0 };
	prev_pos = { 0,0 };
	v = { 0,0 };
	a = { 0,0 };
	area = 1;
	density = 1;
	dt = 1;
	mass = 0;
	drag_coeficient = 0;
	radius = 0;
	gravity = 0;
}

Verlet::~Verlet() {}

//initial situation of the particle

void InitialSituation(Verlet &particle, float dt) {
	bool ret = true;

	particle.a = AccelerationSum(particle);
	particle.prev_pos.x = particle.pos.x - particle.v.x * dt + particle.a.x * 0.5f * dt * dt;
	particle.prev_pos.y = particle.pos.y - particle.v.y * dt + particle.a.y * 0.5f * dt * dt;
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
	cout << "px: " << pos.x << " py: " << pos.y << " vx: " << v_new.x << " vy: " << v_new.y << endl;

	return v_new;
}

fPoint Verlet_Acceleration(float m, fPoint total_f) {
	fPoint a_new;

	a_new = total_f / m;

	return a_new;
}

//collision related


//we can now if a body is colliding checking all the cases in which it doesn't collide
bool CheckCollision(Verlet particle, VRectangle rect) {
	bool ret = false;
	if (particle.pos.x + particle.radius >= rect.x &&
		particle.pos.x - particle.radius <= rect.x + rect.w
		&& particle.pos.y - particle.radius <= rect.y + rect.h
		&& particle.pos.y + particle.radius >= rect.y) {
		ret = true;
	}
	return ret;
}

void SolveCollision(Verlet particle, VRectangle rect)
{
	float time = 0;
	fPoint aux_pos;
	fPoint a = particle.a * -1;
	aux_pos = particle.prev_pos;
	particle.prev_pos = particle.pos;
	particle.pos = aux_pos;
	while (CheckCollision(particle, rect))
	{
		particle.pos = Verlet_Integration(particle.pos, particle.prev_pos, a, 0.1);
	}
	aux_pos = particle.prev_pos;
	particle.prev_pos = particle.pos;
	particle.pos = aux_pos;
}

float Calculate_Time(float pos_i, float pos_new, float v, float a, float dt) {
	float time, time1, time2, t_pow;

	if (a == 0) {
		time = (pos_new - pos_i) / v;
		return time;
	}
	t_pow = pow((v * v) - ((pos_i - pos_new) * a * 2), 0.5);
	time1 = (-v + t_pow) / a;
	time2 = (-v - t_pow) / a;

	if (time1 > 0 && time1 < dt)time = time1;
	else if (time2 > 0 && time2 < dt)time = time2;
	else time = 0;

	return time;
}

float CalculateCollisionPosition(Verlet& particle, VRectangle rect) {

	float time = 0;
	bool col_x, col_y;
	col_x = col_y = false;

	particle.v = Stormer_Verlet(particle.pos, particle.prev_pos, particle.a, particle.dt);


	//if the particle hits the right collider
	if (particle.prev_pos.x + particle.radius < rect.x) {
		time = Calculate_Time(particle.prev_pos.x, rect.x - particle.radius, particle.v.x, particle.a.x, particle.dt);
		col_x = true;
	}
	//if the particle hits the left collider
	else if (particle.prev_pos.x - particle.radius > rect.x + rect.w) {
		time = Calculate_Time(particle.prev_pos.x, rect.x + rect.w + particle.radius, particle.v.x, particle.a.x, particle.dt);
		col_x = true;
	}
	//if the particle hits the bottom collider
	else if (particle.prev_pos.y + particle.radius < rect.y) {
		time = Calculate_Time(particle.prev_pos.y, rect.y - particle.radius, particle.v.y, particle.a.y, particle.dt);
		col_y = true;
	}
	//if the particle hits the top collider
	else if (particle.prev_pos.y - particle.radius > rect.y + rect.h) {
		time = Calculate_Time(particle.prev_pos.y, rect.y + rect.h + particle.radius, particle.v.y, particle.a.y, particle.dt);
		col_y = true;
	}

	particle.pos = particle.prev_pos + particle.v * time + particle.a * 0.5 * time * time;

	particle.v = Stormer_Verlet(particle.pos, particle.prev_pos, particle.a, time);

	if (col_x == true) {
		particle.v.x = -particle.v.x * 0.9;
		particle.v.y *=0.9;
	}
	else if (col_y == true) {
		particle.v.y = -particle.v.y * 0.9;
		particle.v.x *= 0.9;
	}
	return time;
}

void CalculateCollisionFinalPosition(Verlet& particle, float time) {
	time = particle.dt - time;
	particle.pos = particle.prev_pos + particle.v * time + particle.a * 0.5 * (time * time);

	particle.prev_pos = particle.pos - particle.v * particle.dt - particle.a * 0.5 * particle.dt * particle.dt;
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

fPoint AccelerationSum(Verlet particle) {
	fPoint accelerationSum;
	//accelerationSum.x = accelerationSum.y = 0;
	//accelerationSum = particle.a;
	//accelerationSum += DragAcceleration(particle.density, particle.drag_coeficient, particle.area, particle.v, particle.mass);
	//accelerationSum.x += DragAcceleration(particle.density, particle.drag_coeficient, particle.area, particle.v, particle.mass).x + Calculate_Acceleration(particle.vi, particle.vf, particle.dt) +
		//Parachutist_Acceleration(particle.mass, particle.v.y, particle.gravity, particle.k) + Freefall_Acceleration(particle.gravity, particle.mass, particle.drag_coeficient);*/
	if (particle.density != 0)
	{
		accelerationSum.x = DragAcceleration(particle.density, particle.drag_coeficient, particle.area, particle.v, particle.mass).x;
		accelerationSum.y = particle.gravity + DragAcceleration(particle.density, particle.drag_coeficient, particle.area, particle.v, particle.mass).y;/* +Freefall_Acceleration(particle.gravity, particle.mass, particle.drag_coeficient) /*+ Parachutist_Acceleration(particle.mass, particle.v.y, particle.gravity, particle.k)*/;

	}
	else
	{
		accelerationSum = particle.a;
	}

	return accelerationSum;
}

float Terminal_Velocity(float gravity, float mass, float density, float drag_coefficient, float area) {
	return sqrt((2 * mass * abs(gravity)) / (density * drag_coefficient * area));
}

float Parachutist_Acceleration(float m, fPoint v, float gravity, float k) {
	float acceleration;

	// k: constant based on falling object form

	acceleration = (-m * gravity + k * v.y * v.y) / m;

	return acceleration;
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

float Time_To_Position(fPoint initial_position, fPoint velocity, fPoint acceleration, float dt, float final_position, int axis_option) {
	float time = 0;
	Verlet particle;
	int max_iterations = 100;
	int current_iterations = 0;
	fPoint temp_pos;

	particle.a = acceleration;
	particle.prev_pos = initial_position;
	particle.pos = Classical_Motion(initial_position, velocity, particle.a, dt);

	//the final position is on the x axis
	if (axis_option == 1)
	{
		if (final_position > 0)
		{
			while (particle.pos.x < final_position)
			{
				temp_pos = particle.pos;
				particle.pos = Verlet_Integration(particle.pos, particle.prev_pos, particle.a, dt);
				particle.prev_pos = temp_pos;

				current_iterations++;
				time += dt;
				cout << "Time: " << time << " px: " << particle.pos.x << " py: " << particle.pos.y << " ax: " << acceleration.x << " ay: " << acceleration.y << endl;
			}
		}
		else
		{
			while (particle.pos.x > final_position)
			{
				temp_pos = particle.pos;
				particle.pos = Verlet_Integration(particle.pos, particle.prev_pos, particle.a, dt);
				particle.prev_pos = temp_pos;

				current_iterations++;
				time += dt;

				cout << "Time: "<< time << " px: " << particle.pos.x << " py: " << particle.pos.y << " ax: " << acceleration.x << " ay: " << acceleration.y << endl;
			}
		}
	}

	//the final position is on the y axis
	if (axis_option == 2)
	{
		if (final_position > 0)
		{
			while (particle.pos.y < final_position)
			{
				temp_pos = particle.pos;
				particle.pos = Verlet_Integration(particle.pos, particle.prev_pos, particle.a, dt);
				particle.prev_pos = temp_pos;

				current_iterations++;
				time += dt;

				cout << "Time: " << time << " px: " << particle.pos.x << " py: " << particle.pos.y << " ax: " << acceleration.x << " ay: " << acceleration.y << endl;
			}
		}
		else
		{
			while (particle.pos.y > final_position)
			{
				temp_pos = particle.pos;
				particle.pos = Verlet_Integration(particle.pos, particle.prev_pos, particle.a, dt);
				particle.prev_pos = temp_pos;

				current_iterations++;
				time += dt;

				cout << "Time: " << time << " px: " << particle.pos.x << " py: " << particle.pos.y << " ax: " << acceleration.x << " ay: " << acceleration.y << endl;
			}
		}
	}
	return time;
}

fPoint Position_at_Time(fPoint pos, fPoint velocity, fPoint acceleration, float time)
{
	fPoint prev_pos,aux_pos;
	float dt = 1.0f;
	float time_passed = dt;
	prev_pos = pos;
	pos = Classical_Motion(pos, velocity, acceleration, dt);

	while (time_passed < time)
	{
		cout << "Time passed: " << time_passed << " px: " << pos.x << " py: " << pos.y << " ax: " << acceleration.x << " ay: " << acceleration.y << endl;
		aux_pos = pos;
		pos = Verlet_Integration(pos, prev_pos, acceleration, dt);
		prev_pos = aux_pos;
		time_passed += dt;

	}

	return pos;
}


fPoint Classical_Motion(fPoint position, fPoint velocity, fPoint acceleration, float dt) {
	fPoint final_position;

	final_position.x = position.x + velocity.x * dt + 0.5f * acceleration.x * dt * dt;
	final_position.y = position.y + velocity.y * dt + 0.5f * acceleration.y * dt * dt;

	return final_position;
}

float Flight_Time(float vi, float gravity, float angle) {
	float time;

	time = (2 * vi * sin(angle * PI / 180)) / gravity;

	return time;
}

float Flight_Time(fPoint vi, float gravity) {
	float time;

	time = (2 * vi.y) / gravity;

	return time;
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
