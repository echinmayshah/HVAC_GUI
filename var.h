/*
 * var.h
 *
 *  Created on: Dec 9, 2025
 *      Author: Chinmay.Shah
 */

#ifndef VAR_H_
#define VAR_H_

#include "Controller.h"

#if 0
	extern uint8_t i_0;
	extern uint16_t tick_0;

	extern float arr_raw[8];
	extern float arr_filt[8];
	extern float arr_speedCmd[8];
	extern float arr_speedFdb[8];
	extern float arr_speedCmd_elec[8];
	extern float arr_speedCmd_mech[8];
#endif

#if 0
	extern uint8_t i_1;
	extern uint16_t tick_1;
	
	extern float arr_i2t[8];
	
	extern volatile uint32_t tick_2;
	//extern uint32_t tick_3;
	extern volatile uint32_t tick_4;
	extern volatile uint32_t tick_5;
	extern volatile uint32_t tick_6;
	extern volatile uint32_t tick_7;
#endif

#if 0
	extern volatile uint8_t i_4;
	extern volatile uint16_t tick_4;

	extern volatile float arr_pot[250];
	extern volatile float arr_mech[250];
	extern volatile float arr_elec[250];
	extern volatile float arr_elec_int[250];
#endif

#if 0
	extern uint8_t k0, k1;
	extern float arr_speedCmd_M0[100];
	extern float arr_speedCmd_M1[100];	
	
	extern uint8_t m0, m1;
	extern float arr_speedFdb_M0[100];
	extern float arr_speedFdb_M1[100];	
#endif

#if 0
	typedef struct
	{
		uint32_t gTick;
		uint32_t currTick;
		uint32_t reqDel;
	}del_t;
	
	extern del_t gDel, del_0, del_1;
			
	bool reqDelay(del_t *p_del);
#endif

#if 1
	typedef enum
	{
		eBrake_Boot = 0,
		eOpen_Loop,
		eOL_to_CL,
		eClosed_Loop,
		eFault
	}isrState_t;
	extern isrState_t isrState[2];
#endif


typedef struct
{
	bool startMotor;
	bool stopMotor;
	bool fullThrotole;

	float speedDesired;	
	float speedCmd;
	//float speedRef;
	float speedInc;
	float speedDec;
	
	uint32_t tick;

	uint8_t gearUp;
}motor_t;
extern motor_t m[2];
	
extern volatile uint32_t speedCmd_M0, speedCmd_M1;
extern uint32_t speedFdb_M0, speedFdb_M1;

extern uint32_t gSysTick;
extern uint16_t gSysSec;

extern bool defautSpeedStart;
extern bool emStop[2];

extern float closedLoop_Speed;

#endif /* VAR_H_ */
