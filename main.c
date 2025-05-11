//INCLUDE SECTION
#include "stm32f10x.h" 
#include "../func_lib/MRE6183_pwm.h"
#include "../func_lib/MRE6183_usart.h"
#include "../func_lib/MRE6183_delays.h"
#include "../func_lib/MRE6183_Cart_Encoder.h" //including Cart Encdoer header file
#include "../func_lib/MRE6183_Pendulum_Encoder.h"  //including pendulum Encdoer header file
#include "../func_lib/MRE6183_IP_Controls_Utils.h" //including controls utils file
#include <stdio.h>
#include <math.h>

//GLOBAL VARIABLES
#define Samp_Rate 1000
const float Ts = 1.0f / Samp_Rate; //Defining Sample Time from sampling rate

/*FILTER STUFF*/
const float cutoff_hz_cart = 7.0f; //filter cutoff frequency
const float zeta_cart = 0.9f; //filter zeta value
const float cutoff_hz_pend = 7.0f; //filter cutoff frequency
const float zeta_pend = 0.9f; //filter zeta value
Filter2ndOrder cart_filter;
Filter2ndOrder pend_filter;


const float MAX_CART_POS =	0.4f; // Maximum Cart Position in m - maybe 0.43 for swingup controll 
const float MAX_PENDULUM_ANGLE = 0.7f; // Maximum pendulum angle in rad for switching between swingup and balance control



/* Setup a state machine */
typedef enum {
    MODE_STOP = 0,
    MODE_SWINGUP,
    MODE_BALANCE
} ControlMode;
volatile ControlMode controller_mode = MODE_SWINGUP; //Forces the controller mode to start at MODE_STOP

/*Defining State Feedbackk Controller Gains*/
volatile float K1 =  -22;
volatile float K2 = 97.5;
volatile float K3 = -29;
volatile float K4 =  18;



const float Cont_Gain =1.0f;
volatile float control_voltage = 0;
uint16_t pwm_value = 0; //Defining the PWM value


/*Defining swingup controller Gains*/
const float mu = 3.0f;
const float u_max = 4.5f;
float k_pos = 0.0f; //make ZERO to turn of position penalization in swingup controller
float MAX_CART_POS_SWINGUP = 0.8f*MAX_CART_POS;

/*Defining Cart positions and velocities*/
volatile float pos_cart = 0;
volatile float pos_cart_prev = 0;
volatile float vel_cart_raw = 0;
volatile float vel_cart_filtered = 0;
volatile float local_cart_pos = 0;
float cart_pos_for_control = 0;

/*Defining Pendulum positions and velocities*/
volatile float pos_pend = 0;
volatile float pos_pend_prev = 0;
volatile float vel_pend_raw = 0;
volatile float vel_pend_filtered = 0;
volatile float local_pend_pos = 0;
volatile ControlMode loc_controller_mode = MODE_SWINGUP;
/*Defining stuff for sending out data*/

char serial_string_out[500]; 
#define DATA_BUFFER_SIZE 500 // Store 1000 samples in the buffer (1 second)
float loc_cart_pos, loc_pend_pos, loc_cart_vel, loc_pend_vel, loc_voltage;
float time_ms = 0;
float time_copy = 0;

int main(void){
	/*PWM CONFIG*/
	PWM_Configuration(); //calling the PWM_Configuration function
	PWM_Enable();
	PWM_SetValue(500); //Setting pwm value to 500 = 0 V (ish) control voltage
	
	/*PENDULUM ENCODER CONFIG */
	Pendulum_Encoder_Configuration();
	Pendulum_Enable();
	Pendulum_ResetValue();
	
	/*CART ENCODER CONFIG */
	Cart_Encoder_Configuration();
	Cart_Enable();
	Cart_ResetValue();
	__enable_irq();
	
	/*FILTER CONFIG*/
	filter_init(&cart_filter, cutoff_hz_cart, zeta_cart, Ts);
	filter_init(&pend_filter, cutoff_hz_pend, zeta_pend, Ts);

		/*DELAY CONFIG*/
	TimerDelay_Init();  // Needed for delay_ms() to work properly	
	
	/*UART CONFIG*/
	UART_Init();
	// Clear screen + scrollback + move cursor to top-left
	sprintf(serial_string_out, "\033[2J\033[3J\033[H");
	UART_StringOut(serial_string_out);

	
	/*Control Loop - TIMER INITIALIZATION*/
	TimerInterrupt_Init(Samp_Rate);
	
	
		/* INFINITE LOOP */
    while(1){
			
			///*
			loc_cart_pos = pos_cart;
			loc_pend_pos = pos_pend;
			loc_cart_vel = vel_cart_filtered;
			loc_pend_vel = vel_pend_filtered;
			loc_voltage  = control_voltage;
			time_copy = time_ms;
			loc_controller_mode = controller_mode;
			
			sprintf(serial_string_out, "%.1f,%.3f,%.3f,%.3f,%.3f,%.3f,%d\r\n",
        time_copy,loc_cart_pos, loc_pend_pos, loc_cart_vel, loc_pend_vel, loc_voltage,loc_controller_mode);
			UART_StringOut(serial_string_out);
			//*/
		
			
	} //Infinite loop
} //Main Function 

/* CONTROL LOOP */
void TIM6_DAC_IRQHandler(void) {
	TIM_ClearITPendingBit(TIM6, TIM_IT_Update); //Lets clear the flag before doing anything
		time_ms++ ;

		/* READING PEND AND CART POSITIONS */
		pos_cart = Cart_ReadValueRad();	
		pos_pend = Pendulum_ReadValueRad();
		pos_pend = WrapAngle(pos_pend); //wrapping from -2pi to 2pi - UNCOMMENT IF YOU DONT WANT THAT
		
		/*PEND AND CART VELOCITIES*/
		vel_cart_raw = (pos_cart - pos_cart_prev)*Samp_Rate;
		vel_pend_raw = (pos_pend - pos_pend_prev)*Samp_Rate;

		vel_cart_filtered = filter_apply(&cart_filter, vel_cart_raw);
		vel_pend_filtered = filter_apply(&pend_filter, vel_pend_raw);
	
		/*Save previos values*/
		pos_cart_prev = pos_cart;
		pos_pend_prev = pos_pend;
			
		/* STATE MACHINE */
		/* STATE MACHINE */
	switch (controller_mode) {
    case MODE_STOP:
        // remain in STOP until reset externally if needed
        break;

    case MODE_SWINGUP:
        if (fabsf(pos_cart) > MAX_CART_POS) {
            controller_mode = MODE_STOP;
        } else if (fabsf(pos_pend) < MAX_PENDULUM_ANGLE) {
            controller_mode = MODE_BALANCE;
        }
        break;

    case MODE_BALANCE:
        if (fabsf(pos_cart) > MAX_CART_POS) {
            controller_mode = MODE_STOP;
        }
        // Do NOT go back to swingup
        break;
}

	
	/*STATE MACHINE - switch between different controler modes*/
	switch (controller_mode) {
		case MODE_STOP:
				control_voltage = 0;
				pwm_value = voltage_to_pwm(control_voltage);
				PWM_SetValue(pwm_value);// cut PWM to zero so the motor coasts/brakes
				break;
		
		case MODE_SWINGUP:
				control_voltage = -compute_swingup_control_voltage(k_pos,pos_cart,pos_pend,vel_pend_filtered,vel_cart_filtered,mu,u_max);

				pwm_value = voltage_to_pwm(control_voltage);
				//pwm_value = 0;
				PWM_SetValue(pwm_value); // cut PWM to zero so the motor coasts/brakes
				break;
		
		case MODE_BALANCE:
				control_voltage = Cont_Gain*compute_control_voltage(
											pos_cart,pos_pend,vel_cart_filtered,vel_pend_filtered,
											K1,K2,K3,K4);

				pwm_value = voltage_to_pwm(control_voltage);
				PWM_SetValue(pwm_value); 
				
		
	}
	
	/* Preparing Data to Send Out */
	
}
