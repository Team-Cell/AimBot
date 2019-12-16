#ifndef _PHYSICS_H_
#define _PHYSICS_H_
#include <math.h>
#include "p2Point.h"

#define PI 3.1416

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

class Verlet{
public:
	Verlet();
	~Verlet();
public:
	fPoint	pos;
	fPoint	prev_pos;
	fPoint	v;
	fPoint	a;
	float	density;
	float	area;
	float	drag_coeficient;
	float	dt;
	float	mass;
	int		radius;
	float	gravity;
};

//main verlet
fPoint Verlet_Integration(fPoint pos, fPoint prev_pos, fPoint ai, float dt);
fPoint Velocity_Verlet(fPoint vi, fPoint ai, fPoint a_new, float dt);
fPoint Stormer_Verlet(fPoint pos, fPoint pos_new, fPoint a, float dt);
fPoint Verlet_Acceleration(float m, fPoint total_f);


//acceleration and velocity
fPoint DragAcceleration(float density, float drag_coefficient, float area, fPoint speed, float mass);
fPoint Calculate_Acceleration(fPoint vi, fPoint vf, float dt);
fPoint AccelerationSum(Verlet particle);

//position calculators 
fPoint Classical_Motion(fPoint position, fPoint velocity, fPoint acceleration, float dt = 1.0f);

//additional formulas
fPoint Forces_Sum(fPoint f1, fPoint f2);
float Module(fPoint var);

#endif // !_PHYSICS_H_


