/*
 * var.c
 *
 *  Created on: Dec 9, 2025
 *      Author: Chinmay.Shah
 */

#include "var.h"

#if 0
	uint8_t i_0 = 0;
	uint16_t tick_0 = 0;

	float arr_raw[8]	= {0.0};
	float arr_filt[8] 	= {0.0};
	float arr_speedCmd[8] = {0.0};
	float arr_speedFdb[8] = {0.0};
	float arr_speedCmd_elec[8] = {0.0};
	float arr_speedCmd_mech[8] = {0.0};
#endif

#if 0
	uint8_t i_1 = 0;
	uint16_t tick_1 = 0;
	
	float arr_i2t[8] 			= {0.0};
	
	volatile uint32_t tick_2 = 0;
	//uint32_t tick_3 = 0;
	volatile uint32_t tick_4 = 0;
	volatile uint32_t tick_5 = 0;
	volatile uint32_t tick_6 = 0;
	volatile uint32_t tick_7 = 0;
#endif

#if 0
	volatile uint8_t i_4 = 0;
	volatile uint16_t tick_4 = 0;

	volatile float arr_pot[250] = {0.0};
	volatile float arr_mech[250] = {0.0};
	volatile float arr_elec[250] = {0.0};
	volatile float arr_elec_int[250] = {0.0};
#endif

#if 0
	uint8_t k0 = 0, k1 = 0;
	float arr_speedCmd_M0[100] = {0.0};
	float arr_speedCmd_M1[100] = {0.0};	
	
	uint8_t m0 = 0, m1 = 0;
	float arr_speedFdb_M0[100] = {0.0};
	float arr_speedFdb_M1[100] = {0.0};	
#endif

#if 0
	del_t gDel, del_0, del_1;
			
	bool reqDelay(del_t *p_del)
	{
		if( (p_del->gTick - p_del->currTick) >= p_del->reqDel )
		{
			return true;
		}
				
		return false;
	}
#endif


#if 1
	isrState_t isrState[2] = {eBrake_Boot};
#endif

motor_t m[2];

volatile uint32_t speedCmd_M0 = 0, speedCmd_M1 = 0;
uint32_t speedFdb_M0, speedFdb_M1;

uint32_t gSysTick = 0;
uint16_t gSysSec = 0;

bool defautSpeedStart = false;
bool emStop[2] = { 0 };

float closedLoop_Speed = 544.0f;//460.0f;




