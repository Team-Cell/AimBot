#ifndef _VERLET_H_
#define _VERLET_H_
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
	fPoint pos;
	fPoint prev_pos;
	fPoint v;
	fPoint a;
	float density;
	float area;
	float drag_coeficient;
	float dt;
	float mass;
	int radius;
	float gravity;
};

void InitialSituation(Verlet &particle, float dt);

//main verlet
fPoint Verlet_Integration(fPoint pos, fPoint prev_pos, fPoint ai, float dt);
fPoint Velocity_Verlet(fPoint vi, fPoint ai, fPoint a_new, float dt);
fPoint Stormer_Verlet(fPoint pos, fPoint pos_new, fPoint a, float dt);
fPoint Verlet_Acceleration(float m, fPoint total_f);

//collision related 
bool CheckCollision(Verlet particle, VRectangle rect);
void SolveCollision(Verlet particle, VRectangle rect);
float Calculate_Time(float pos_i, float pos_new, float v, float a, float dt);
float CalculateCollisionPosition(Verlet& particle, VRectangle rect);
void CalculateCollisionFinalPosition(Verlet& particle, float time);

//acceleration and velocity
fPoint DragAcceleration(float density, float drag_coefficient, float area, fPoint speed, float mass);
fPoint Calculate_Acceleration(fPoint vi, fPoint vf, float dt);
fPoint AccelerationSum(Verlet particle);
float Terminal_Velocity(float gravity, float mass, float density, float drag_coefficient, float area);
float Parachutist_Acceleration(float m, float v_y, float gravity, float k);
float Freefall_Speed(float gravity, float m, float air_density, float area, float friction_const);
float Freefall_Acceleration(float gravity, float m, float friction_const);

//position calculators 
float Time_To_Position(fPoint initial_position, fPoint velocity, fPoint acceleration, float dt, float final_position, int axis_option);
fPoint Position_at_Time(fPoint pos, fPoint velocity, fPoint acceleration, float time);
fPoint Classical_Motion(fPoint position, fPoint velocity, fPoint acceleration, float dt = 1.0f);
float Flight_Time(float vi, float gravity, float angle);
float Flight_Time(fPoint vi, float gravity);

//additional formulas

fPoint Forces_Sum(fPoint f1, fPoint f2);
float Module(fPoint var);

#endif // !_VERLET_H_


