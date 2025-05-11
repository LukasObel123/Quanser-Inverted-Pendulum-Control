#ifndef MRE6183_IP_Controls_Utils_H
#define MRE6183_IP_Controls_Utils_H
#include <stdint.h>


// Define filter struct
typedef struct {
    float b0, b1, b2;
    float a1, a2;
    float x1, x2;
    float y1, y2;
} Filter2ndOrder;


/* 
 * Initialize the 2nd-order low-pass filter.
 *   cutoff_hz = desired cutoff frequency in Hz (e.g. 10.0)
 *   zeta      = damping ratio      (e.g. 0.9)
 *   Ts        = sample period in seconds (e.g. 0.005 for 200 Hz)
 */
// Instance-based filter init and apply
void filter_init(Filter2ndOrder* f, float cutoff_hz, float zeta, float Ts);
float filter_apply(Filter2ndOrder* f, float x0);


/*COMPUTE BALANCE CONTROL VOLTAGE*/
float compute_control_voltage(
	float pos_cart,
	float pos_pend,
	float vel_cart_filtered,
	float vel_pend_filtered,
	float K1, 
	float K2, 
	float K3, 
	float K4
);

/*CONVERT VOLTAGE TO PWM*/	
uint16_t voltage_to_pwm(float voltage);
	
/*Get Sign of 2 variables - Needed for swingup controller */
int sgn_prod(float a, float b);

/*Convert energy control and x_c_dot to motor voltage*/
float swingup_controlforce_to_voltage(float u, float x_c_dot);

/*Compute pendulum energy*/
float compute_energy(float alpha, float alpha_dot);
	
/*Compute the control voltage for the swingup controller*/
float compute_swingup_control_voltage(float k_pos,float pos_cart, float alpha, float alpha_dot, float x_c_dot, float mu, float u_max);


/*WRAPS ANGLE FROM -2PI TO 2PI */
float WrapAngle(float angle);

#endif /* MRE6183_IP_Controls_Utils_H */