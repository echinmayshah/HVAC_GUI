/*
 * GUI.h
 *
 *  Created on: Dec 11, 2025
 *      Author: Chinmay.Shah
 */

#ifndef GUI_H_
#define GUI_H_


#include "HardwareIface.h"
#include "ParamConfig.h"
#include "Controller.h"
#include "var.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//SAFE_9 in-plan
		//menuLevel=2 will has dual motor			
		//menuLevel=0,1 will also prompt for motor_0/motor_1
#define GUI 					(1U)
#define SAFE_9 					(1U)

/* 8MHz IMO clock with 8000000 reload value to generate 1s interrupt */
#define SYSTICK_RELOAD_VAL   	(8000UL)	//1 mSec

#define MIN_TEMPERATURE 20
#define MAX_TEMPERATURE 27

void handle_error(void);
void GUI_Call(void);
bool guiStart_Call(void);



void toggle_led_on_systick_handler(void);

#endif /* GUI_H_ */
