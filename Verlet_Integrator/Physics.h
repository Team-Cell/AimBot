#ifndef _PHYSICS_H_
#define _PHYSICS_H_
#include <math.h>
#include "p2Point.h"

#define PI 3.1416
#define GRAVITY 9.81
#define DRAG_COEFFICIENT 1 

struct Weapon {
	float initial_speed = 0;
	float bounce_coefficient = 0;
	bool linear_trajectory = false;
	bool wind_activated = false;
	Weapon(float init_speed, float bounce_coeff, bool lin_traj, bool wind) {
		initial_speed = init_speed;
		bounce_coefficient = bounce_coeff;
		linear_trajectory = lin_traj;
		wind_activated = wind;
	}
};

struct VRectangle {
	float x = 0;
	float y = 0;
	float w = 0;
	float h = 0;

	VRectangle(){};
	~VRectangle() {};

	VRectangle(float u_x, float u_y, float u_w, float u_h){
		x = u_x;
		y = u_y;
		w = u_w;
		h = u_h;
	}
};

class Particle{
public:
	Particle();
	~Particle();

public:
	fPoint	pos;
	fPoint	prev_pos;
	fPoint	v;
	fPoint	a;
	fPoint  wind;

	float	density;
	float	area;
	float	drag_coeficient;
	float	dt;
	float	mass;
	int		w;
	int		h;
	float	gravity;
	float	tf;
};

class PhysicsEngine
{
public:
	PhysicsEngine();
	~PhysicsEngine();

private:

public:
	float dt;
	int Montecarlo;
	int Montecarlo_iterations;
	int max_path_iterations;
	float final_angle;
	int min_angle;
	int max_angle;
	float wind_acceleration;
};

//main verlet
fPoint Verlet_Integration(fPoint pos, fPoint prev_pos, fPoint ai, float dt);
fPoint Velocity_Verlet(fPoint vi, fPoint ai, fPoint a_new, float dt);
fPoint Stormer_Verlet(fPoint pos, fPoint pos_new, fPoint a, float dt);
fPoint Verlet_Acceleration(float m, fPoint total_f);

//acceleration and velocity
fPoint DragAcceleration(float density, float drag_coefficient, float area, fPoint speed, float mass);
fPoint Calculate_Acceleration(fPoint vi, fPoint vf, float dt);
fPoint Calculate_Acceleration(fPoint force, float mass);
fPoint AccelerationSum(Particle particle);

//position calculators 
fPoint Classical_Motion(fPoint position, float initial_velocity, float angle, fPoint acceleration, bool gravity, float dt = 1.0f);
void CalculatePath(Particle& projectile, Weapon* chosen_weapon, float& angle, VRectangle target, VRectangle rectangles[4], PhysicsEngine physics);

void Physics(float tf, int fps, float dt, float gravity, float mass, fPoint v, fPoint wind, float density);

//collisions
bool OnCollision(Particle particle, VRectangle rectangle);
void HandleCollision(Particle& particle, VRectangle rect, float dt, float bounce_coefficient);

// To be deleted
bool OnCollision(Particle projectile, Particle target);
//

//additional formulas
fPoint Forces_Sum(fPoint f1, fPoint f2);
float Module(fPoint var);
fPoint AddWind(fPoint a, float wind);
fPoint AddGravity(fPoint a);

#endif // !_PHYSICS_H_


