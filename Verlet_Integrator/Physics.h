#ifndef _PHYSICS_H_
#define _PHYSICS_H_
#include <math.h>
#include "p2Point.h"

#define PI 3.1416
#define GRAVITY 0.5
#define DRAG_COEFFICIENT 1 
#define AIR_DENSITY 1.22

#include "SDL_image/include/SDL_image.h"

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

class Particle{
public:
	Particle();
	~Particle();

public:
	fPoint	pos;
	fPoint	prev_pos;
	fPoint	v;
	fPoint	a;

	float	density;
	float	area;
	float	dt;
	float	mass;
	int		w;
	int		h;
	float	gravity;
	Weapon* weapon;
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
	int Max_Montecarlo;
	int max_path_iterations;

	int min_angle;
	int max_angle;
	float final_angle;

	int max_speed;
	int min_speed;
	float final_speed;

	float wind_acceleration;
};

//Calculate position with drag
fPoint CalculatePos(fPoint pos, fPoint prev_pos, fPoint ai, float dt, float area, float mass, float v_projectile = 0.0f, float angle = 0.0f, bool classical_motion = false);

//main verlet
fPoint Verlet_Integration(fPoint pos, fPoint prev_pos, fPoint ai, float dt);
fPoint Velocity_Verlet(fPoint vi, fPoint ai, fPoint a_new, float dt);
fPoint Stormer_Verlet(fPoint pos, fPoint pos_new, fPoint a, float dt);
fPoint Verlet_Acceleration(float m, fPoint total_f);

//acceleration and velocity
fPoint Calculate_Acceleration(fPoint vi, fPoint vf, float dt);
fPoint Calculate_Acceleration(fPoint force, float mass);
fPoint AccelerationSum(Particle particle);

//position calculators 
fPoint Classical_Motion(fPoint position, float initial_velocity, float angle, fPoint acceleration, float dt = 1.0f);

//collisions
bool OnCollision(Particle particle, SDL_Rect rectangle, bool flip_y = true);
void HandleCollision(Particle& particle, SDL_Rect rect, float dt, float bounce_coefficient);
bool OnCollision(SDL_Rect projectile, SDL_Rect target);

//additional formulas
fPoint Forces_Sum(fPoint f1, fPoint f2);

#endif // !_PHYSICS_H_


