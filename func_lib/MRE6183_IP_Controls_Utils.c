#include <math.h>
#include <stdint.h>
#include "../func_lib/MRE6183_IP_Controls_Utils.h"


//define pi
static const float PI = 3.1415926f;

//Lets declare some constants for the swingup controller
static const float J_eq = 0.7031;
static const float R_m = 2.6;
static const float r_mp = 0.063;
static const float eta_g = 1;
static const float K_g = 3.71;
static const float eta_m = 1;
static const float K_t = 0.0077;
static const float K_m = 0.0077;
static const float A = J_eq*R_m*r_mp/(eta_g*K_g*eta_m*K_t); //This is just to help in the force to motor voltage calculation
static const float B = K_g*K_m/r_mp; //This is just to help in the force to motor voltage calculation

static const float J_p = 0.0079;
static const float M_p = 0.23;
static const float g = 9.81f;
static const float l_p = 0.3302;

//SWINGUP Parameters
volatile float E = 0;
volatile float u_swingup = 0;
volatile float u_swingup_sat = 0;
volatile int sgn = 0; //Defining sign in swingup controller


void filter_init(Filter2ndOrder* f, float cutoff_hz, float zeta, float Ts){
    float wcf = 2.0f * PI * cutoff_hz;
    float K  = 2.0f / Ts;
    float K2 = K * K;
    float wc2 = wcf * wcf;
    float D = K2 + 2.0f * zeta * wcf * K + wc2;

    f->b0 = wc2 / D;
    f->b1 = 2.0f * wc2 / D;
    f->b2 = wc2 / D;

    f->a1 = (2.0f * wc2 - 2.0f * K2) / D;
    f->a2 = (K2 - 2.0f * zeta * wcf * K + wc2) / D;

    f->x1 = f->x2 = f->y1 = f->y2 = 0.0f;
}




float filter_apply(Filter2ndOrder* f, float x0){
    float y0 = f->b0 * x0
             + f->b1 * f->x1
             + f->b2 * f->x2
             - f->a1 * f->y1
             - f->a2 * f->y2;

    f->x2 = f->x1;
    f->x1 = x0;
    f->y2 = f->y1;
    f->y1 = y0;

    return y0;
}

float compute_control_voltage(
	float pos_cart,
	float pos_pend,
	float vel_cart_filtered,
	float vel_pend_filtered,
	float K1, 
	float K2, 
	float K3, 
	float K4
){
	float u = -(K1*pos_cart+K2*pos_pend+K3*vel_cart_filtered+K4*vel_pend_filtered);
	return fminf( fmaxf(u, -10.0f), 10.0f ); //saturating value
}

uint16_t voltage_to_pwm(float voltage) {

    float pwm_f = (voltage + 10.0f) * 50.0f;
    // Round to nearest integer
    return (uint16_t)(pwm_f + 0.5f);
}


int sgn_prod(float a, float b){
	if (a == 0.0f || b ==0.0f)
			return 0;
	return (a > 0.0f) == (b > 0.0f)? 1: -1;
}

float compute_energy(float alpha, float alpha_dot){
	return 0.5*J_p*alpha_dot*alpha_dot + M_p*g*l_p*(1-cosf(alpha));
}


float swingup_controlforce_to_voltage(float u, float x_c_dot){
	return A*u + B*x_c_dot;
}

float compute_swingup_control_voltage(float k_pos,float pos_cart, float alpha, float alpha_dot, float x_c_dot, float mu, float u_max) {
    // Compute pendulum energy (relative to upright)
    float E = compute_energy(cos(alpha), alpha_dot);

    // Sign of energy injection
    int sgn = sgn_prod(alpha_dot, alpha);

    // Raw swing-up control force
    float u = mu * E * sgn-k_pos*pos_cart;

    // Saturate force to ±u_max
    u = fminf(fmaxf(u, -u_max), u_max);

    // Convert to voltage using your existing function
    return swingup_controlforce_to_voltage(u, x_c_dot);
}



float WrapAngle(float angle) {
    while (angle > 2.0f * PI) {
        angle -= 2.0f * PI;
    }
    while (angle < -2.0f * PI) {
        angle += 2.0f * PI;
    }
    return angle;
}


	