/******************************************************************************
* File Name: main.c
*
* Description: This example demonstrates the UART transmit and receive
*              operation using PDL APIs
*
* Related Document: See Readme.md 
*
*******************************************************************************
* Copyright 2024, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

/*******************************************************************************
* Header Files
*******************************************************************************/
#include "cy_pdl.h"
#include "cybsp.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*******************************************************************************
*            Global Functions
*******************************************************************************/
void handle_error(void);

/*******************************************************************************
* Function Name: handle_error
********************************************************************************
* Summary:
* User defined error handling function.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void handle_error(void)
{
     /* Disable all interrupts. */
    __disable_irq();

    CY_ASSERT(0);
}

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function.
* Reads one byte from the serial terminal and echoes back the read byte.
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/


bool stateInit = 0;

uint32_t read_value;// = CY_SCB_UART_RX_NO_DATA;
uint8_t charReceived = 0;
char getString[100] = {0};
uint8_t charToBeXmitted = 0;
char putString[100] = {0};

uint32_t count;

//SAFE_0 1st pretty code - need to work: space as a delimiter, blocking while loop, stringToNumber
//SAFE_1 removed space as a delimiter
//SAFE_2 removed blocking while loop in Xmission
//SAFE_3 SpeedRef updated val cont display
//SAFE_4 display SpeedRef_Xmitted = (SpeedRef_Received + 100); - using sprintf & sscanf
//SAFE_5 entire multi-level menu
//SAFE_6 beautification(SAFE_5);
//SAFE_7 in-plan
		//-> direct entry to observation_menu & back from observation menu, 
		//-> delay_fun instaed of for(;;) 
		//-> function_calls instead of long instruction cycle
			

//SAFE_1 to SAFE_4 are raw materials for menu design

#define SAFE_0 	0
#define SAFE_1 	0
#define SAFE_2 	0
#define SAFE_3 	0
#define SAFE_4 	0
#define SAFE_5 	0
#define SAFE_6 	0
#define SAFE_7 	1

#if SAFE_0
	uint8_t myIndex = 0;
	char tempString[100] = {0};
#endif

#if SAFE_5 || SAFE_6 || SAFE_7
	uint32_t speedCmd = 100, speedRef = 200, 	speedFdb = 300, mode = 400, temprature = 500, greenLED = 1, redLED = 0, fault = 600;
	
	uint8_t menuLevel = 0;
	bool goAhead = 0;
	uint8_t loopCount = 0;

	//static uint8_t userInput[menuLevel][data] = {0};
	uint8_t userInput[2][1] = {0};

	#define UB_DELAY_TICK_THRESHOLD   (10000UL)
	
	bool Delay_NonBlocking(uint32_t requiredDelay)
	{		
	    static uint32_t iDel = 0U;
	    static uint32_t jDel = 0U;
	
	    if (iDel < requiredDelay)
	    {
	        jDel++;
	        if (jDel >= 10000U)
	        {
	            iDel++;
	            jDel = 0U;
	        }
	    }
	    else
	    {
	        iDel = 0U;
	        jDel = 0U;
	        return true;
	    }
	
	    return false;
	}
#endif

void UART_GotoXY(uint8_t row, uint8_t col)
{
    char buffer[20];

    sprintf(buffer, "\x1B[%d;%dH", row, col);
	Cy_SCB_UART_PutString(USER_UART_HW, buffer);
}

#if SAFE_7
	#define MIN_TEMPERATURE 20
	#define MAX_TEMPERATURE 27
	
	static inline uint32_t random_range(uint32_t min, uint32_t max)
	{
	    return (rand() % (max - min + 1)) + min;
	}
	
	uint32_t getParaVal(uint8_t loopCount)
	{
	    uint32_t getNum = 0U;
	
	    switch (loopCount)
	    {
	        case 0:
	            speedCmd = random_range(1, 1000);
	            getNum = speedCmd;
	            break;
	
	        case 1:
	            speedRef = random_range(1, 1000);
	            if (speedRef > speedCmd)
	                speedRef = speedCmd;
	            getNum = speedRef;
	            break;
	
	        case 2:
	            speedFdb = random_range(1, 1000);
	            getNum = speedFdb;
	            break;
	
	        case 3:
	            mode = random_range(1, 5);
	            getNum = mode;
	            break;
	
	        case 4:
	            uint32_t temp = random_range(1, MAX_TEMPERATURE);
	            temprature = (temp < MIN_TEMPERATURE) ? MIN_TEMPERATURE : temp;
	            getNum = temprature;
	        	break;
	
	        case 5:
	            getNum = greenLED;
	            break;
	
	        case 6:
	            getNum = redLED;
	            break;
	
	        case 7:
	        default:
	            fault = random_range(1, 5);
	            getNum = fault;
	            break;
	    }

		return(getNum);
	}
#endif

int main(void)
{
    cy_en_scb_uart_status_t init_status;
    cy_stc_scb_uart_context_t USER_UART_context;

    /* Initialize the device and board peripherals */
    cybsp_init();

    /* Start UART operation */
    init_status = Cy_SCB_UART_Init(USER_UART_HW, &USER_UART_config, &USER_UART_context);
    if (init_status!=CY_SCB_UART_SUCCESS)
       {
            handle_error();
       }
    Cy_SCB_UART_Enable(USER_UART_HW);

    /* Transmit header to the terminal */
    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    Cy_SCB_UART_PutString(USER_UART_HW, "\x1b[2J\x1b[;H");
	
	#if (SAFE_0 || SAFE_1 || SAFE_2 || SAFE_3 || SAFE_4)
		Cy_SCB_UART_PutString(USER_UART_HW, "line # 1) ************************************************************\r\n");
	    Cy_SCB_UART_PutString(USER_UART_HW, "line # 2) HVAC Motor Control GUI\r\n");
	    Cy_SCB_UART_PutString(USER_UART_HW, "line # 3) ************************************************************\r\n\n");
	    //Cy_SCB_UART_PutString(USER_UART_HW, "line # 5) >> Enter 1 to enter parameters, followed by enter \r\n\n");
	#endif

    __enable_irq();

    for(; ;)
    {
		#if SAFE_0
			if(!state)	//printing
			{
				if(stateInit == 0)	
				{
					if(loopCount == 0)
					{
						UART_GotoXY(10, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "line # 10) Enter your first name: \r\n");
						UART_GotoXY(10, 40);
					}
					else if(loopCount == 1)
					{
						UART_GotoXY(11, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "line # 11) Enter your second name: \r\n");	//40th onwards
						UART_GotoXY(11, 40);
					}
					else if(loopCount == 2)
					{
						UART_GotoXY(12, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "line # 12) Enter your last name: \r\n");
						UART_GotoXY(12, 40);
					}
					
					stateInit = 1;
					state = 1;
				}
			}
			else if(state == 1)	//receiving
			{	
				#if 1
					read_value = Cy_SCB_UART_Get(USER_UART_HW);
					if(read_value != CY_SCB_UART_RX_NO_DATA)
					{
						if((char)read_value == ' ')
						{
							//Cy_SCB_UART_ClearRxFifo(USER_UART_HW);
							//SCB_RX_FIFO_CTRL(USER_UART_HW) |= (uint32_t)  SCB_RX_FIFO_CTRL_CLEAR_Msk;
							
							reg = 1;
							charReceived = myIndex;// - 1;
						}
						else if( ( (char)read_value == '\n') || ((char)read_value == '\r') )
						{
							state = 2;
						}
						else if( ( (char)read_value == 0x0A) || ((char)read_value == 0x0D) )
						{
							state = 2;
						}
						else if( (read_value == '\n') || (read_value == '\r') )
						{
							state = 2;
						}
						else if( (read_value == 0x0A) || (read_value == 0x0D) )
						{
							state = 2;
						}
						else 
						{
							tempString[myIndex++] = (char)read_value;
						}
						tempString_1[myIndex_1++] = (char)read_value;
					}
				#endif
			}
			else if(state == 2)	
			{
				uint8_t i = 0;
				while(i < charReceived)
				{
					getString[i] = tempString[i];
					i++;
				}
				state = 3;
			}
			else if(state == 3)	//received & echo-back
			{
				uint8_t j = 0;
				while(j < charReceived)	
				{
					count = Cy_SCB_UART_Put(USER_UART_HW, getString[j++]);
			        if(count == 0ul)
			        {
			          	handle_error();
			        }
				}
				
				state = 4;
			}
			else if(state == 4)	//reset state
			{
				myIndex 		= 0;
				charReceived 	= 0;
				stateInit 		= 0;
				memset(getString, 0, sizeof(getString));

				//Cy_SCB_UART_ClearRxFifo(USER_UART_HW);
    			//SCB_RX_FIFO_CTRL(USER_UART_HW) &= (uint32_t) ~SCB_RX_FIFO_CTRL_CLEAR_Msk;

				loopCount++;

				state = 0;
			}
		#endif

		#if SAFE_1
			if(!state)	//printing
			{
				if(stateInit == 0)	
				{
					if(loopCount == 0)
					{
						UART_GotoXY(10, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "line # 10) Enter your first name: \r\n");
						UART_GotoXY(10, 40);
					}
					else if(loopCount == 1)
					{
						UART_GotoXY(11, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "line # 11) Enter your second name: \r\n");	//40th onwards
						UART_GotoXY(11, 40);
					}
					else if(loopCount == 2)
					{
						UART_GotoXY(12, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "line # 12) Enter your last name: \r\n");
						UART_GotoXY(12, 40);
					}
					
					stateInit = 1;
					state = 1;
				}
			}
			else if(state == 1)	//receiving
			{	
				#if 1
					read_value = Cy_SCB_UART_Get(USER_UART_HW);
					if(read_value != CY_SCB_UART_RX_NO_DATA)
					{
						if((char)read_value == '\n')
						{
							state = 2;
						}
						else 
						{
							getString[charReceived++] = (char)read_value;
						}
					}
				#endif
			}
			else if(state == 2)	//received & echo-back
			{
				uint8_t i = 0;
				while(i < charReceived)	
				{
					count = Cy_SCB_UART_Put(USER_UART_HW, getString[i++]);
			        if(count == 0ul)
			        {
			          	handle_error();
			        }
				}
				
				state = 3;
			}
			else if(state == 3)	//reset state
			{
				charReceived 	= 0;
				stateInit 		= 0;
				memset(getString, 0, sizeof(getString));

				loopCount++;
				state = 0;
			}
		#endif

		#if SAFE_2
			if(!state)	//printing
			{
				if(stateInit == 0)	
				{
					if(loopCount == 0)
					{
						UART_GotoXY(10, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "line # 10) Enter your first name: \r\n");
						UART_GotoXY(10, 40);
					}
					else if(loopCount == 1)
					{
						UART_GotoXY(11, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "line # 11) Enter your second name: \r\n");	//40th onwards
						UART_GotoXY(11, 40);
					}
					else if(loopCount == 2)
					{
						UART_GotoXY(12, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "line # 12) Enter your last name: \r\n");
						UART_GotoXY(12, 40);
					}
					
					stateInit = 1;
					state = 1;
				}
			}
			else if(state == 1)	//receiving
			{	
				read_value = Cy_SCB_UART_Get(USER_UART_HW);
				if(read_value != CY_SCB_UART_RX_NO_DATA)
				{
					if((char)read_value == '\n')
					{
						state = 2;
					}
					else 
					{
						getString[charReceived++] = (char)read_value;
					}
				}
			}
			else if(state == 2)	//received & echo-back
			{
				static uint8_t charXmitted = 0;
				if(charXmitted < charReceived)	
				{
					count = Cy_SCB_UART_Put(USER_UART_HW, getString[charXmitted++]);
			        if(count == 0ul)
			        {
			          	handle_error();
			        }
				}
				else 
				{
					charXmitted = 0;
					state = 3;
				}
			}
			else if(state == 3)	//reset state
			{
				charReceived 	= 0;
				stateInit 		= 0;
				memset(getString, 0, sizeof(getString));

				loopCount++;
				state = 0;
			}
		#endif

		#if SAFE_3
			uint16_t SpeedRef = 50;

			if(!state)	//printing
			{
				UART_GotoXY(10, 1);
				Cy_SCB_UART_PutString(USER_UART_HW, "line # 10) SpeedRef: \r\n");
				UART_GotoXY(10, 40);
				
				state = 1;
			}
			else if(state == 1)	//receiving
			{	
				static uint32_t tick = 0;
				//char chSpeedRef[20];

				// Seed the random number generator using the current time
			    // This should only be called once in the program
			    //srand(time(NULL));
				
				if(++tick >= 1e7)	
				{
					SpeedRef = (rand() % 1000) + 1;

					sprintf(getString, "%d", SpeedRef);
					charReceived = strlen(getString);

					tick = 0;
					state = 2;
				}
			}
			else if(state == 2)	//received & echo-back
			{
				static uint8_t charXmitted = 0;
				if(charXmitted < charReceived)	
				{
					count = Cy_SCB_UART_Put(USER_UART_HW, getString[charXmitted++]);
			        if(count == 0ul)
			        {
			          	handle_error();
			        }
				}
				else 
				{
					charXmitted = 0;
					state = 3;
				}
			}
			else if(state == 3)	//reset state
			{
				charReceived 	= 0;
				stateInit 		= 0;
				memset(getString, 0, sizeof(getString));

				loopCount++;
				state = 0;
			}
		#endif

		#if SAFE_4
			if(!state)	//printing
			{
				UART_GotoXY(10, 1);
				Cy_SCB_UART_PutString(USER_UART_HW, "line # 10) Enter SpeedRef: \r\n");
				UART_GotoXY(10, 40);
					
				state = 1;
			}
			else if(state == 1)	//receiving
			{	
				read_value = Cy_SCB_UART_Get(USER_UART_HW);
				if(read_value != CY_SCB_UART_RX_NO_DATA)
				{
					if((char)read_value == '\n')
					{
						uint16_t getSpeedRef, putSpeedRef;		
						
						sscanf(getString, "%hd", &getSpeedRef);			//str to int	
						putSpeedRef = getSpeedRef + 100;	

						sprintf(putString, "%d", putSpeedRef);			//int to str
						charToBeXmitted = strlen(putString);

						state = 2;
					}
					else 
					{
						getString[charReceived++] = (char)read_value;
					}
				}
			}
			else if(state == 2)	//received & echo-back
			{
				static uint8_t charXmitted = 0;
				if(charXmitted < charToBeXmitted)	
				{
					count = Cy_SCB_UART_Put(USER_UART_HW, putString[charXmitted++]);
			        if(count == 0ul)
			        {
			          	handle_error();
			        }
				}
				else 
				{
					charXmitted = 0;
					state = 3;
				}
			}
			else if(state == 3)	//reset state
			{
				charReceived 	= 0;
				charToBeXmitted = 0;
				stateInit 		= 0;
				memset(getString, 0, sizeof(getString));
				memset(putString, 0, sizeof(putString));

				loopCount++;
				state = 0;
			}
		#endif

		#if SAFE_5
			if(menuLevel == 0)
			{
				static uint8_t state = 0;

				if(!state)	//printing
				{
					UART_GotoXY(1, 1);
					Cy_SCB_UART_PutString(USER_UART_HW, "# 1) ************************************************************\r\n");
				    Cy_SCB_UART_PutString(USER_UART_HW, "# 2) HVAC Motor Control GUI\r\n");
				    Cy_SCB_UART_PutString(USER_UART_HW, "# 3) ************************************************************\r\n");
					Cy_SCB_UART_PutString(USER_UART_HW, "# 4) \r\n");
					Cy_SCB_UART_PutString(USER_UART_HW, "# 5) \r\n");
					Cy_SCB_UART_PutString(USER_UART_HW, "# 6) \r\n");
					//UART_GotoXY(7, 1);
					Cy_SCB_UART_PutString(USER_UART_HW, "# 7) \r\n");
					//UART_GotoXY(8, 1);
					Cy_SCB_UART_PutString(USER_UART_HW, "# 8) Main-Menu \r\n");
					//UART_GotoXY(9, 1);
					Cy_SCB_UART_PutString(USER_UART_HW, "# 9) Enter you choice (followed by enter)\r\n");
					//UART_GotoXY(10, 1);
					Cy_SCB_UART_PutString(USER_UART_HW, "# 10) Enter 1 to start/stop motor \r\n");
					//UART_GotoXY(11, 1);
					Cy_SCB_UART_PutString(USER_UART_HW, "# 11) Enter 2 for speedCmd \r\n");
					//UART_GotoXY(12, 1);
					Cy_SCB_UART_PutString(USER_UART_HW, "# 12) your input: \r\n");
					UART_GotoXY(12, 45);
		
					state = 1;
				}
				else if(state == 1)	//receiving
				{	
					read_value = Cy_SCB_UART_Get(USER_UART_HW);
					if(read_value != CY_SCB_UART_RX_NO_DATA)
					{
						if((char)read_value == '\n')
						{
							uint32_t getNum, putNum;		
						
							sscanf(getString, "%lu", &getNum);			//str to int (getString to getNum)
							
							if(getNum == 1 || getNum == 2)
							{
								putNum = getNum;
								sprintf(putString, "%lu", putNum);		//int to str (putNum to putString)
								//userInput[menuLevel][0] = putNum;		//userInput[menuLevel][0] = putNum;
								userInput[0][0] = putNum;				//userInput[menuLevel][0] = putNum;
								goAhead = 1;
							}
							else 
							{
								strcpy(putString, "invalid input");
								goAhead = 0;
							}
							
							charToBeXmitted = strlen(putString);
							state = 2;
						}
						else 
						{
							getString[charReceived++] = (char)read_value;
						}
					}
				}
				else if(state == 2)	//received & echo-back
				{
					static uint8_t charXmitted = 0;
					if(charXmitted < charToBeXmitted)	
					{
						count = Cy_SCB_UART_Put(USER_UART_HW, putString[charXmitted++]);
				        if(count == 0ul)
				        {
				          	handle_error();
				        }
					}
					else 
					{
						charXmitted = 0;
						state = 3;
					}
				}
				else if(state == 3)	//reset state
				{
					charReceived 	= 0;
					charToBeXmitted = 0;
					memset(getString, 0, sizeof(getString));
					memset(putString, 0, sizeof(putString));
	
					if(goAhead == false)
					{
						menuLevel = 0;
					}
					else 
					{
						menuLevel++;
					}
					
					for(uint32_t i=0; i<10000; i++)
						for(uint32_t j=0; j<10000; j++);
					
					Cy_SCB_UART_PutString(USER_UART_HW, "\x1b[2J\x1b[;H");	//clear screen
				}
			}
			else if(menuLevel == 1)
			{
				static uint8_t state = 0;

				if(!state)	//printing
				{
					if(userInput[0][0] == 1)		//start/stop
					{
						UART_GotoXY(1, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "line # 1) ************************************************************\r\n");
					    Cy_SCB_UART_PutString(USER_UART_HW, "line # 2) HVAC Motor Control GUI\r\n");
					    Cy_SCB_UART_PutString(USER_UART_HW, "line # 3) ************************************************************\r\n");
						
						UART_GotoXY(7, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "\r\n");
						UART_GotoXY(8, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "line # 8) 	Menu level 1 - start/stop \r\n");
						UART_GotoXY(10, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "line # 10) Enter 1 to start motor \r\n");
						UART_GotoXY(11, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "line # 11) Enter 2 to stop motor \r\n");
						UART_GotoXY(10, 40);	//go to line # 10 to take input from user
					}
					else if(userInput[0][0] == 2)	//speedCmd
					{
						UART_GotoXY(8, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "line # 8) 	Menu level 1 - speedCmd \r\n");
						UART_GotoXY(10, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "line # 10) Enter speedCmd:  \r\n");
						UART_GotoXY(10, 40);	//go to line # 10 to take input from user
					}
						
					state = 1;
				}
				else if(state == 1)	//receiving
				{	
					read_value = Cy_SCB_UART_Get(USER_UART_HW);
					if(read_value != CY_SCB_UART_RX_NO_DATA)
					{
						if((char)read_value == '\n')
						{
							uint32_t getNum, putNum;		
						
							sscanf(getString, "%lu", &getNum);				//str to int (getString to getNum)
							
							if(userInput[0][0] == 1)	//start/stop
							{
								if(getNum == 1 || getNum == 2)
								{
									putNum = getNum;
									sprintf(putString, "%lu", putNum);		//int to str (putNum to putString)
									userInput[1][0] = getNum;				//userInput[menuLevel][0] = getNum;
									goAhead = 1;
								}
								else 
								{
									strcpy(putString, "invalid input");
									goAhead = 0;
								}
							}
							else if(userInput[0][0] == 2)	//speedRef 
							{
								if(getNum)	//if non-Zero
								{
									putNum = getNum;
									sprintf(putString, "%lu", putNum);		//int to str (putNum to putString)
									userInput[1][0] = getNum;				//userInput[menuLevel][0] = getNum;
									goAhead = 1;
								}
								else 
								{
									strcpy(putString, "invalid input");
									goAhead = 0;
								}
							}
							
							charToBeXmitted = strlen(putString);
							state = 2;
						}
						else 
						{
							getString[charReceived++] = (char)read_value;
						}
					}
				}
				else if(state == 2)	//received & echo-back
				{
					static uint8_t charXmitted = 0;
					if(charXmitted < charToBeXmitted)	
					{
						count = Cy_SCB_UART_Put(USER_UART_HW, putString[charXmitted++]);
				        if(count == 0ul)
				        {
				          	handle_error();
				        }
					}
					else 
					{
						charXmitted = 0;
						state = 3;
					}
				}
				else if(state == 3)	//reset state
				{
					charReceived 	= 0;
					charToBeXmitted = 0;
					memset(getString, 0, sizeof(getString));
					memset(putString, 0, sizeof(putString));
	
					if(goAhead == false)
					{
						
						menuLevel = 0;
					}
					else 
					{
						menuLevel++;
					}
		
					#if 1
						for(uint32_t i=0; i<10000; i++)
							for(uint32_t j=0; j<10000; j++);
					
						Cy_SCB_UART_PutString(USER_UART_HW, "\x1b[2J\x1b[;H");	//clear screen	//TODO
						Cy_SCB_UART_PutString(USER_UART_HW, "\r\n");
					#endif
				}
			}
			else if(menuLevel == 2)	//run-time menu
			{
				static uint8_t state = 0;

				if(!state)	//printing
				{
					static uint8_t init = 0;
					
					if(!init)
					{
						if(userInput[0][0] == 1)			//start/stop
						{
							if(userInput[1][0] == 1)		//start
							{
								speedCmd = 500;
							}
							else if(userInput[1][0] == 2)	//stop
							{
								speedCmd = 0;
							}
						}
						else if(userInput[0][0] == 2)		//speedCmd
						{
							speedCmd = userInput[1][0];
						}

						init += 1;
					}
					else if(init == 1)
					{
						UART_GotoXY(1, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# 1) ************************************************************\r\n");
					    Cy_SCB_UART_PutString(USER_UART_HW, "# 2) HVAC Motor Control GUI\r\n");
					    Cy_SCB_UART_PutString(USER_UART_HW, "# 3) ************************************************************\r\n");
						
						//UART_GotoXY(4, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# 4) \r\n");
						//UART_GotoXY(5, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# 5) \r\n");
						//UART_GotoXY(6, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# 6) \r\n");
						//UART_GotoXY(7, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# 7) \r\n");
						//UART_GotoXY(8, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# 8) \r\n");
						//UART_GotoXY(9, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# 9) \r\n");

						//UART_GotoXY(10, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# 10) speedCmd: \r\n");
						//UART_GotoXY(11, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# 11) speedRef: \r\n");
						//UART_GotoXY(12, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# 12) speedFdb: \r\n");
						//UART_GotoXY(13, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# 13) mode: \r\n");
						//UART_GotoXY(14, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# 14) temprature: \r\n");
						//UART_GotoXY(15, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# 15) greenLED: \r\n");
						//UART_GotoXY(16, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# 16) redLED: \r\n");
						//UART_GotoXY(17, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# 17) fault: \r\n");

						for(uint32_t i=0; i<10000; i++)
							for(uint32_t i=0; i<10000; i++);
						
						loopCount = 0;
						init++;
					}
					else 
					{
						//Cy_SCB_UART_ClearTxFifo(USER_UART_HW);
						//Cy_SCB_UART_ClearRxFifo(USER_UART_HW);
						
						UART_GotoXY((loopCount + 10), 35);
						state = 1;
					}
				}
				else if(state == 1)	//inputing
				{	
					#if 1
					uint32_t getNum, putNum;	
					
					if(loopCount == 0)
					{
						speedCmd = (rand() % 1000) + 1;
						getNum = speedCmd;
					}
					else if(loopCount == 1)
					{
						speedRef = (rand() % 1000) + 1;
						if(speedRef >= speedCmd)
							speedRef = speedCmd;
						getNum = speedRef;
					}
					else if(loopCount == 2)
					{
						speedFdb = (rand() % 1000) + 1;
						getNum = speedFdb;
					}
					else if(loopCount == 3)
					{
						mode = (rand() % 5) + 1;
						getNum = mode;
					}
					else if(loopCount == 4)
					{
						temprature = (rand() % 27) + 1;
						if(temprature <= 20)
							temprature = 20;
						getNum = temprature;
					}
					else if(loopCount == 5)
					{
						getNum = greenLED;
					}
					else if(loopCount == 6)
					{
						getNum = redLED;
					}
					else //if(loopCount == 7)
					{
						fault = (rand() % 5) + 1;
						getNum = fault;
					}
					#endif
					
					//strcpy(getString, "12345");
					//sscanf(getString, "%lu", &getNum);		//str to int (getString to getNum)		
					putNum = getNum;
					sprintf(putString, "%lu", putNum);			//int to str (putNum to putString)						
					
					charToBeXmitted = strlen(putString);

					state = 2;
				}
				else if(state == 2)	//received & echo-back
				{
					static uint8_t charXmitted = 0;
					if(charXmitted < charToBeXmitted)	
					{
						count = Cy_SCB_UART_Put(USER_UART_HW, putString[charXmitted++]);
				        if(count == 0ul)
				        {
				          	handle_error();
				        }
					}
					else 
					{
						charXmitted = 0;
						state = 3;
					}
				}
				else if(state == 3)	//reset state
				{
					charReceived 	= 0;
					charToBeXmitted = 0;
					memset(getString, 0, sizeof(getString));
					memset(putString, 0, sizeof(putString));
				

					#if 0
					for(uint32_t i=0; i<10000; i++)
						for(uint32_t i=0; i<10000; i++);
					#endif

					for(uint32_t i=0; i<1000; i++)
						for(uint32_t i=0; i<10000; i++);

					loopCount += 1;
					if(loopCount >= 8)
					{
						loopCount = 0;
					}
					
					state = 0;
				}
			}
		#endif

		#if SAFE_6
			if(menuLevel == 0)
			{
				static uint8_t state = 0;

				if(!state)	//printing
				{
					UART_GotoXY(1, 1);
					Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************ \r\n");
				    Cy_SCB_UART_PutString(USER_UART_HW, "# HVAC Motor Control GUI \r\n");
				    Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************ \r\n");

					UART_GotoXY(5, 1);
					Cy_SCB_UART_PutString(USER_UART_HW, "# Menu_M0 - Main Menu \r\n");
					Cy_SCB_UART_PutString(USER_UART_HW, "# Enter you choice (followed by enter) \r\n");
					
					UART_GotoXY(8, 1);
					Cy_SCB_UART_PutString(USER_UART_HW, "# Enter 1 to start/stop motor \r\n");
					Cy_SCB_UART_PutString(USER_UART_HW, "# Enter 2 for speedCmd \r\n");
					
					UART_GotoXY(11, 1);
					Cy_SCB_UART_PutString(USER_UART_HW, "# Your input?: \r\n");
					UART_GotoXY(11, 16);
		
					state = 1;
				}
				else if(state == 1)	//receiving
				{	
					read_value = Cy_SCB_UART_Get(USER_UART_HW);
					if(read_value != CY_SCB_UART_RX_NO_DATA)
					{
						if((char)read_value == '\n')
						{
							uint32_t getNum, putNum;		
						
							sscanf(getString, "%lu", &getNum);			//str to int (getString to getNum)
							
							if(getNum == 1 || getNum == 2)
							{
								putNum = getNum;
								sprintf(putString, "%lu", putNum);		//int to str (putNum to putString)
								//userInput[menuLevel][0] = putNum;		//userInput[menuLevel][0] = putNum;
								userInput[0][0] = putNum;				//userInput[menuLevel][0] = putNum;
								goAhead = 1;
							}
							else 
							{
								strcpy(putString, "invalid input");
								goAhead = 0;
							}
							
							charToBeXmitted = strlen(putString);
							state = 2;
						}
						else 
						{
							getString[charReceived++] = (char)read_value;
						}
					}
				}
				else if(state == 2)	//received & echo-back
				{
					static uint8_t charXmitted = 0;
					if(charXmitted < charToBeXmitted)	
					{
						count = Cy_SCB_UART_Put(USER_UART_HW, putString[charXmitted++]);
				        if(count == 0ul)
				        {
				          	handle_error();
				        }
					}
					else 
					{
						charXmitted = 0;
						state = 3;
					}
				}
				else if(state == 3)	//reset state
				{
					charReceived 	= 0;
					charToBeXmitted = 0;
					memset(getString, 0, sizeof(getString));
					memset(putString, 0, sizeof(putString));
	
					if(goAhead == false)
					{
						menuLevel = 0;
					}
					else 
					{
						menuLevel++;
					}
					
					for(uint32_t i=0; i<10000; i++)
						for(uint32_t j=0; j<10000; j++);
					
					Cy_SCB_UART_PutString(USER_UART_HW, "\x1b[2J\x1b[;H");	//clear screen
				}
			}
			else if(menuLevel == 1)
			{
				static uint8_t state = 0;

				if(!state)	//printing
				{
					if(userInput[0][0] == 1)		//start/stop
					{
						UART_GotoXY(1, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************ \r\n");			//line # 1
					    Cy_SCB_UART_PutString(USER_UART_HW, "# HVAC Motor Control GUI \r\n");
					    Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************ \r\n");

						UART_GotoXY(5, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# Menu_M0.0 - Start/Stop Motor Menu \r\n");
						
						UART_GotoXY(7, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# Enter 1 to start motor \r\n");
						Cy_SCB_UART_PutString(USER_UART_HW, "# Enter 2 to stop motor \r\n");
						
						UART_GotoXY(10, 1);	
						Cy_SCB_UART_PutString(USER_UART_HW, "# Your input?: \r\n");
						UART_GotoXY(10, 16);	//go to line # 10 to take input from user
					}
					else if(userInput[0][0] == 2)	//speedCmd
					{
						UART_GotoXY(1, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************\r\n");			//line # 1
					    Cy_SCB_UART_PutString(USER_UART_HW, "# HVAC Motor Control GUI\r\n");
					    Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************\r\n");

						UART_GotoXY(5, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# Menu_M0.1 - speedCmd Menu \r\n");

						UART_GotoXY(7, 1);	
						Cy_SCB_UART_PutString(USER_UART_HW, "# Enter speedCmd \r\n");
						
						UART_GotoXY(10, 1);	
						Cy_SCB_UART_PutString(USER_UART_HW, "# Your input?: \r\n");
						UART_GotoXY(10, 16);	//go to line # 9 to take input from user
					}
						
					state = 1;
				}
				else if(state == 1)	//receiving
				{	
					read_value = Cy_SCB_UART_Get(USER_UART_HW);
					if(read_value != CY_SCB_UART_RX_NO_DATA)
					{
						if((char)read_value == '\n')
						{
							uint32_t getNum, putNum;		
						
							sscanf(getString, "%lu", &getNum);				//str to int (getString to getNum)
							
							if(userInput[0][0] == 1)	//start/stop
							{
								if(getNum == 1 || getNum == 2)
								{
									putNum = getNum;
									sprintf(putString, "%lu", putNum);		//int to str (putNum to putString)
									userInput[1][0] = getNum;				//userInput[menuLevel][0] = getNum;
									goAhead = 1;
								}
								else 
								{
									strcpy(putString, "invalid input");
									goAhead = 0;
								}
							}
							else if(userInput[0][0] == 2)	//speedRef 
							{
								if(getNum)	//if non-Zero
								{
									putNum = getNum;
									sprintf(putString, "%lu", putNum);		//int to str (putNum to putString)
									userInput[1][0] = getNum;				//userInput[menuLevel][0] = getNum;
									goAhead = 1;
								}
								else 
								{
									strcpy(putString, "invalid input");
									goAhead = 0;
								}
							}
							
							charToBeXmitted = strlen(putString);
							state = 2;
						}
						else 
						{
							getString[charReceived++] = (char)read_value;
						}
					}
				}
				else if(state == 2)	//received & echo-back
				{
					static uint8_t charXmitted = 0;
					if(charXmitted < charToBeXmitted)	
					{
						count = Cy_SCB_UART_Put(USER_UART_HW, putString[charXmitted++]);
				        if(count == 0ul)
				        {
				          	handle_error();
				        }
					}
					else 
					{
						charXmitted = 0;
						state = 3;
					}
				}
				else if(state == 3)	//reset state
				{
					charReceived 	= 0;
					charToBeXmitted = 0;
					memset(getString, 0, sizeof(getString));
					memset(putString, 0, sizeof(putString));
	
					if(goAhead == false)
					{
						menuLevel = 0;
					}
					else 
					{
						menuLevel++;
					}
		
					#if 1
						for(uint32_t i=0; i<10000; i++)
							for(uint32_t j=0; j<10000; j++);
					
						Cy_SCB_UART_PutString(USER_UART_HW, "\x1b[2J\x1b[;H");	//clear screen	//TODO
						Cy_SCB_UART_PutString(USER_UART_HW, "\r\n");
					#endif
				}
			}
			else if(menuLevel == 2)	//run-time menu
			{
				static uint8_t state = 0;

				if(!state)	//printing
				{
					static uint8_t init = 0;
					
					if(!init)
					{
						if(userInput[0][0] == 1)			//start/stop
						{
							if(userInput[1][0] == 1)		//start
							{
								speedCmd = 500;
							}
							else if(userInput[1][0] == 2)	//stop
							{
								speedCmd = 0;
							}
						}
						else if(userInput[0][0] == 2)		//speedCmd
						{
							speedCmd = userInput[1][0];
						}

						init += 1;
					}
					else if(init == 1)
					{
						UART_GotoXY(1, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************\r\n");
					    Cy_SCB_UART_PutString(USER_UART_HW, "# HVAC Motor Control GUI\r\n");
					    Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************\r\n");

						UART_GotoXY(8, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# Parameters \r\n");
						UART_GotoXY(8, 35);
						Cy_SCB_UART_PutString(USER_UART_HW, "Value \r\n");

						UART_GotoXY(10, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# speedCmd: \r\n");
						//UART_GotoXY(11, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# speedRef: \r\n");
						//UART_GotoXY(12, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# speedFdb: \r\n");
						//UART_GotoXY(13, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# mode: \r\n");
						//UART_GotoXY(14, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# temprature: \r\n");
						//UART_GotoXY(15, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# greenLED: \r\n");
						//UART_GotoXY(16, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# redLED: \r\n");
						//UART_GotoXY(17, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# fault: \r\n");

						for(uint32_t i=0; i<10000; i++)
							for(uint32_t i=0; i<10000; i++);
						
						loopCount = 0;
						init++;
					}
					else 
					{
						//Cy_SCB_UART_ClearTxFifo(USER_UART_HW);
						//Cy_SCB_UART_ClearRxFifo(USER_UART_HW);
						
						UART_GotoXY((loopCount + 10), 35);
						state = 1;
					}
				}
				else if(state == 1)	//inputing
				{	
					#if 1
					uint32_t getNum, putNum;	
					
					if(loopCount == 0)
					{
						speedCmd = (rand() % 1000) + 1;
						getNum = speedCmd;
					}
					else if(loopCount == 1)
					{
						speedRef = (rand() % 1000) + 1;
						if(speedRef >= speedCmd)
							speedRef = speedCmd;
						getNum = speedRef;
					}
					else if(loopCount == 2)
					{
						speedFdb = (rand() % 1000) + 1;
						getNum = speedFdb;
					}
					else if(loopCount == 3)
					{
						mode = (rand() % 5) + 1;
						getNum = mode;
					}
					else if(loopCount == 4)
					{
						temprature = (rand() % 27) + 1;
						if(temprature <= 20)
							temprature = 20;
						getNum = temprature;
					}
					else if(loopCount == 5)
					{
						getNum = greenLED;
					}
					else if(loopCount == 6)
					{
						getNum = redLED;
					}
					else //if(loopCount == 7)
					{
						fault = (rand() % 5) + 1;
						getNum = fault;
					}
					#endif
					
					//strcpy(getString, "12345");
					//sscanf(getString, "%lu", &getNum);		//str to int (getString to getNum)		
					putNum = getNum;
					sprintf(putString, "%lu", putNum);			//int to str (putNum to putString)						
					
					charToBeXmitted = strlen(putString);

					state = 2;
				}
				else if(state == 2)	//received & echo-back
				{
					static uint8_t charXmitted = 0;
					if(charXmitted < charToBeXmitted)	
					{
						count = Cy_SCB_UART_Put(USER_UART_HW, putString[charXmitted++]);
				        if(count == 0ul)
				        {
				          	handle_error();
				        }
					}
					else 
					{
						charXmitted = 0;
						state = 3;
					}
				}
				else if(state == 3)	//reset state
				{
					charReceived 	= 0;
					charToBeXmitted = 0;
					memset(getString, 0, sizeof(getString));
					memset(putString, 0, sizeof(putString));
				
					#if 0
					for(uint32_t i=0; i<10000; i++)
						for(uint32_t i=0; i<10000; i++);
					#endif

					for(uint32_t i=0; i<1000; i++)
						for(uint32_t i=0; i<10000; i++);

					loopCount += 1;
					if(loopCount >= 8)
					{
						loopCount = 0;
					}
					
					state = 0;
				}
			}
		#endif

		#if 0//SAFE_7-safe copy
			if(menuLevel == 0)
			{
				static uint8_t state = 0;

				if(!state)	//menu-printing (Menu_M0)
				{
					UART_GotoXY(1, 1);
					Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************ \r\n");
				    Cy_SCB_UART_PutString(USER_UART_HW, "# HVAC Motor Control GUI \r\n");
				    Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************ \r\n");

					UART_GotoXY(5, 1);
					Cy_SCB_UART_PutString(USER_UART_HW, "# Menu_M0 - Main Menu \r\n");
					Cy_SCB_UART_PutString(USER_UART_HW, "# Enter you choice (followed by enter) \r\n");
					
					UART_GotoXY(8, 1);
					Cy_SCB_UART_PutString(USER_UART_HW, "# Enter 1 to start/stop motor \r\n");
					Cy_SCB_UART_PutString(USER_UART_HW, "# Enter 2 for speedCmd \r\n");
					
					UART_GotoXY(11, 1);
					Cy_SCB_UART_PutString(USER_UART_HW, "# Your input?: \r\n");
					UART_GotoXY(11, 16);
		
					state = 1;
				}
				else if(state == 1)	//receiving
				{	
					read_value = Cy_SCB_UART_Get(USER_UART_HW);
					if(read_value != CY_SCB_UART_RX_NO_DATA)
					{
						if((char)read_value == '\n')
						{
							uint32_t getNum, putNum;		
						
							sscanf(getString, "%lu", &getNum);			//str to int (getString to getNum)
							
							if(getNum == 1 || getNum == 2)
							{
								putNum = getNum;
								sprintf(putString, "%lu", putNum);		//int to str (putNum to putString)
								//userInput[menuLevel][0] = putNum;		//userInput[menuLevel][0] = putNum;
								userInput[0][0] = putNum;				//userInput[menuLevel][0] = putNum;
								goAhead = 1;
							}
							else 
							{
								strcpy(putString, "invalid input");
								goAhead = 0;
							}
							
							charToBeXmitted = strlen(putString);
							
							state = 2;
						}
						else 
						{
							getString[charReceived++] = (char)read_value;
						}
					}
				}
				else if(state == 2)	//response received-echo-back
				{
					static uint8_t charXmitted = 0;
					if(charXmitted < charToBeXmitted)	
					{
						count = Cy_SCB_UART_Put(USER_UART_HW, putString[charXmitted++]);
				        if(count == 0ul)
				        {
				          	handle_error();
				        }
					}
					else 
					{
						charXmitted = 0;
						state = 3;
					}
				}
				else if(state == 3)	//reset state
				{
					charReceived 	= 0;
					charToBeXmitted = 0;
					memset(getString, 0, sizeof(getString));
					memset(putString, 0, sizeof(putString));
	
					if(goAhead == false)
					{
						menuLevel = 0;
					}
					else 
					{
						menuLevel++;
					}
					
					Delay_NonBlocking(1e3);
					//for(uint32_t i=0; i<10000; i++)
						//for(uint32_t j=0; j<10000; j++);
					
					Cy_SCB_UART_PutString(USER_UART_HW, "\x1b[2J\x1b[;H");	//clear screen
				}
			}
			else if(menuLevel == 1)
			{
				static uint8_t state = 0;

				if(!state)	//menu-printing (Menu_M0.0/M0.1)
				{
					if(userInput[0][0] == 1)		//start/stop
					{
						UART_GotoXY(1, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************ \r\n");			//line # 1
					    Cy_SCB_UART_PutString(USER_UART_HW, "# HVAC Motor Control GUI \r\n");
					    Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************ \r\n");

						UART_GotoXY(5, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# Menu_M0.0 - Start/Stop Motor Menu \r\n");
						
						UART_GotoXY(7, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# Enter 1 to start motor \r\n");
						Cy_SCB_UART_PutString(USER_UART_HW, "# Enter 2 to stop motor \r\n");
						
						UART_GotoXY(10, 1);	
						Cy_SCB_UART_PutString(USER_UART_HW, "# Your input?: \r\n");
						UART_GotoXY(10, 16);	//go to line # 10 to take input from user
					}
					else if(userInput[0][0] == 2)	//speedCmd
					{
						UART_GotoXY(1, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************\r\n");			//line # 1
					    Cy_SCB_UART_PutString(USER_UART_HW, "# HVAC Motor Control GUI\r\n");
					    Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************\r\n");

						UART_GotoXY(5, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# Menu_M0.1 - speedCmd Menu \r\n");

						UART_GotoXY(7, 1);	
						Cy_SCB_UART_PutString(USER_UART_HW, "# Enter speedCmd \r\n");
						
						UART_GotoXY(10, 1);	
						Cy_SCB_UART_PutString(USER_UART_HW, "# Your input?: \r\n");
						UART_GotoXY(10, 16);	//go to line # 9 to take input from user
					}
						
					state = 1;
				}
				else if(state == 1)	//receiving
				{	
					read_value = Cy_SCB_UART_Get(USER_UART_HW);
					if(read_value != CY_SCB_UART_RX_NO_DATA)
					{
						if((char)read_value == '\n')
						{
							uint32_t getNum, putNum;		
						
							sscanf(getString, "%lu", &getNum);				//str to int (getString to getNum)
							
							if(userInput[0][0] == 1)	//start/stop
							{
								if(getNum == 1 || getNum == 2)
								{
									putNum = getNum;
									sprintf(putString, "%lu", putNum);		//int to str (putNum to putString)
									userInput[1][0] = getNum;				//userInput[menuLevel][0] = getNum;
									goAhead = 1;
								}
								else 
								{
									strcpy(putString, "invalid input");
									goAhead = 0;
								}
							}
							else if(userInput[0][0] == 2)	//speedRef 
							{
								if(getNum)	//if non-Zero
								{
									putNum = getNum;
									sprintf(putString, "%lu", putNum);		//int to str (putNum to putString)
									userInput[1][0] = getNum;				//userInput[menuLevel][0] = getNum;
									goAhead = 1;
								}
								else 
								{
									strcpy(putString, "invalid input");
									goAhead = 0;
								}
							}
							
							charToBeXmitted = strlen(putString);
							state = 2;
						}
						else 
						{
							getString[charReceived++] = (char)read_value;
						}
					}
				}
				else if(state == 2)	//response received-echo-back
				{
					static uint8_t charXmitted = 0;
					if(charXmitted < charToBeXmitted)	
					{
						count = Cy_SCB_UART_Put(USER_UART_HW, putString[charXmitted++]);
				        if(count == 0ul)
				        {
				          	handle_error();
				        }
					}
					else 
					{
						charXmitted = 0;
						state = 3;
					}
				}
				else if(state == 3)	//reset state
				{
					charReceived 	= 0;
					charToBeXmitted = 0;
					memset(getString, 0, sizeof(getString));
					memset(putString, 0, sizeof(putString));
	
					if(goAhead == false)
					{
						menuLevel = 0;
					}
					else 
					{
						menuLevel++;
					}
		
					Delay_NonBlocking(1e3);
					//for(uint32_t i=0; i<10000; i++)
						//for(uint32_t j=0; j<10000; j++);
				
					Cy_SCB_UART_PutString(USER_UART_HW, "\x1b[2J\x1b[;H");	//clear screen
					//Cy_SCB_UART_PutString(USER_UART_HW, "\r\n");
				}
			}
			else if(menuLevel == 2)	//run-time menu
			{
				static uint8_t state = 0;

				if(!state)	//menu-printing (Observation Menu)
				{
					static uint8_t init = 0;
					
					if(!init)
					{
						if(userInput[0][0] == 1)			//start/stop
						{
							if(userInput[1][0] == 1)		//start
							{
								speedCmd = 500;
							}
							else if(userInput[1][0] == 2)	//stop
							{
								speedCmd = 0;
							}
						}
						else if(userInput[0][0] == 2)		//speedCmd
						{
							speedCmd = userInput[1][0];
						}

						init += 1;
					}
					else if(init == 1)
					{
						UART_GotoXY(1, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************\r\n");
					    Cy_SCB_UART_PutString(USER_UART_HW, "# HVAC Motor Control GUI\r\n");
					    Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************\r\n");

						UART_GotoXY(8, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# Parameters \r\n");
						UART_GotoXY(8, 35);
						Cy_SCB_UART_PutString(USER_UART_HW, "Value \r\n");

						UART_GotoXY(10, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# speedCmd: \r\n");
						//UART_GotoXY(11, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# speedRef: \r\n");
						//UART_GotoXY(12, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# speedFdb: \r\n");
						//UART_GotoXY(13, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# mode: \r\n");
						//UART_GotoXY(14, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# temprature: \r\n");
						//UART_GotoXY(15, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# greenLED: \r\n");
						//UART_GotoXY(16, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# redLED: \r\n");
						//UART_GotoXY(17, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# fault: \r\n");

						Delay_NonBlocking(1e3);
						//for(uint32_t i=0; i<10000; i++)
							//for(uint32_t j=0; j<10000; j++);
						
						loopCount = 0;
						init++;
					}
					else 
					{
						UART_GotoXY((loopCount + 10), 35);	//go-for parameters-print
						state = 1;
					}
				}
				else if(state == 1)	//inputing
				{	
					#if 1
					uint32_t getNum, putNum;	
					
					if(loopCount == 0)
					{
						speedCmd = (rand() % 1000) + 1;
						getNum = speedCmd;
					}
					else if(loopCount == 1)
					{
						speedRef = (rand() % 1000) + 1;
						if(speedRef >= speedCmd)
							speedRef = speedCmd;
						getNum = speedRef;
					}
					else if(loopCount == 2)
					{
						speedFdb = (rand() % 1000) + 1;
						getNum = speedFdb;
					}
					else if(loopCount == 3)
					{
						mode = (rand() % 5) + 1;
						getNum = mode;
					}
					else if(loopCount == 4)
					{
						temprature = (rand() % 27) + 1;
						if(temprature <= 20)
							temprature = 20;
						getNum = temprature;
					}
					else if(loopCount == 5)
					{
						getNum = greenLED;
					}
					else if(loopCount == 6)
					{
						getNum = redLED;
					}
					else //if(loopCount == 7)
					{
						fault = (rand() % 5) + 1;
						getNum = fault;
					}
					#endif
					
					//strcpy(getString, "12345");
					//sscanf(getString, "%lu", &getNum);		//str to int (getString to getNum)		
					putNum = getNum;
					sprintf(putString, "%lu", putNum);			//int to str (putNum to putString)						
					
					charToBeXmitted = strlen(putString);

					state = 2;
				}
				else if(state == 2)	//received & echo-back
				{
					static uint8_t charXmitted = 0;
					if(charXmitted < charToBeXmitted)	
					{
						count = Cy_SCB_UART_Put(USER_UART_HW, putString[charXmitted++]);
				        if(count == 0ul)
				        {
				          	handle_error();
				        }
					}
					else 
					{
						charXmitted = 0;
						state = 3;
					}
				}
				else if(state == 3)	//reset state
				{
					charReceived 	= 0;
					charToBeXmitted = 0;
					memset(getString, 0, sizeof(getString));
					memset(putString, 0, sizeof(putString));
				
					Delay_NonBlocking(1e3);
					//for(uint32_t i=0; i<1000; i++)
						//for(uint32_t j=0; j<10000; j++);

					loopCount += 1;
					if(loopCount >= 8)
					{
						loopCount = 0;
					}
					
					state = 0;
				}
			}
		#endif

		#if SAFE_7
			if(menuLevel == 0)
			{
				static uint8_t state = 0;

				if(!state)	//menu-printing (Menu_M0)
				{
					UART_GotoXY(1, 1);
					Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************ \r\n");
				    Cy_SCB_UART_PutString(USER_UART_HW, "# HVAC Motor Control GUI \r\n");
				    Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************ \r\n");

					UART_GotoXY(5, 1);
					Cy_SCB_UART_PutString(USER_UART_HW, "# Menu_M0 - Main Menu \r\n");
					Cy_SCB_UART_PutString(USER_UART_HW, "# Enter you choice (followed by enter) \r\n");
					
					UART_GotoXY(8, 1);
					Cy_SCB_UART_PutString(USER_UART_HW, "# Enter 1 to start/stop motor \r\n");
					Cy_SCB_UART_PutString(USER_UART_HW, "# Enter 2 for speedCmd \r\n");
					
					UART_GotoXY(11, 1);
					Cy_SCB_UART_PutString(USER_UART_HW, "# Your input?: \r\n");
					UART_GotoXY(11, 16);
		
					state = 1;
				}
				else if(state == 1)	//receiving
				{	
					read_value = Cy_SCB_UART_Get(USER_UART_HW);

					if (read_value != CY_SCB_UART_RX_NO_DATA)
					{
					    char rx = (char)read_value;
					
					    if (rx == '\n')
					    {
					        uint32_t getNum = 0U;
					        uint32_t putNum = 0U;
					
					        getString[charReceived] = '\0';        /* terminate safely */
					        charReceived = 0U;
					
					        /* Convert string to integer */
					        if (sscanf(getString, "%lu", &getNum) == 1)
					        {
					            if ((getNum == 1U) || (getNum == 2U))
					            {
					                putNum = getNum;
					                (void)sprintf(putString, "%lu", putNum);
					
					                userInput[0][0] = putNum;
					
					                goAhead = 1U;
					            }
					            else
					            {
					                (void)strcpy(putString, "invalid input");
					                goAhead = 0U;
					            }
					        }
					        else
					        {
					            (void)strcpy(putString, "invalid input");
					            goAhead = 0U;
					        }
					
					        charToBeXmitted = (uint8_t)strlen(putString);
					        state = 2U;
					    }
					    else
					    {
					        /* Protect against buffer overflow */
					        if (charReceived < (sizeof(getString) - 1U))
					        {
					            getString[charReceived++] = rx;
					        }
					    }
					}
				}
				else if(state == 2)	//response received-echo-back
				{
					static uint8_t charXmitted = 0;
					if(charXmitted < charToBeXmitted)	
					{
						count = Cy_SCB_UART_Put(USER_UART_HW, putString[charXmitted++]);
				        if(count == 0ul)
				        {
				          	handle_error();
				        }
					}
					else 
					{
						charXmitted = 0;
						state = 3;
					}
				}
				else if(state == 3)	//reset state
				{					
					if(Delay_NonBlocking(1000U))
					{
						charReceived 	= 0;
						charToBeXmitted = 0;
						memset(getString, 0, sizeof(getString));
						memset(putString, 0, sizeof(putString));
		
						// Update menu level
        				menuLevel = (goAhead) ? (menuLevel + 1) : 0;
					
						// Clear terminal screen
						Cy_SCB_UART_PutString(USER_UART_HW, "\x1b[2J\x1b[;H");	
					}
				}
			}
			else if(menuLevel == 1)
			{
				static uint8_t state = 0;

				if(!state)	//menu-printing (Menu_M0.0/M0.1)
				{
					if(userInput[0][0] == 1)		//start/stop
					{
						UART_GotoXY(1, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************ \r\n");			//line # 1
					    Cy_SCB_UART_PutString(USER_UART_HW, "# HVAC Motor Control GUI \r\n");
					    Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************ \r\n");

						UART_GotoXY(5, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# Menu_M0.0 - Start/Stop Motor Menu \r\n");
						
						UART_GotoXY(7, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# Enter 1 to start motor \r\n");
						Cy_SCB_UART_PutString(USER_UART_HW, "# Enter 2 to stop motor \r\n");
						
						UART_GotoXY(10, 1);	
						Cy_SCB_UART_PutString(USER_UART_HW, "# Your input?: \r\n");
						UART_GotoXY(10, 16);	//go to line # 10 to take input from user
					}
					else if(userInput[0][0] == 2)	//speedCmd
					{
						UART_GotoXY(1, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************\r\n");			//line # 1
					    Cy_SCB_UART_PutString(USER_UART_HW, "# HVAC Motor Control GUI\r\n");
					    Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************\r\n");

						UART_GotoXY(5, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# Menu_M0.1 - speedCmd Menu \r\n");

						UART_GotoXY(7, 1);	
						Cy_SCB_UART_PutString(USER_UART_HW, "# Enter speedCmd \r\n");
						
						UART_GotoXY(10, 1);	
						Cy_SCB_UART_PutString(USER_UART_HW, "# Your input?: \r\n");
						UART_GotoXY(10, 16);	//go to line # 9 to take input from user
					}
						
					state = 1;
				}
				else if(state == 1)	//receiving
				{	
					read_value = Cy_SCB_UART_Get(USER_UART_HW);
					if(read_value != CY_SCB_UART_RX_NO_DATA)
					{
						if((char)read_value == '\n')
						{
							uint32_t getNum, putNum;		
						
							sscanf(getString, "%lu", &getNum);				//str to int (getString to getNum)
							
							if(userInput[0][0] == 1)	//start/stop
							{
								if(getNum == 1 || getNum == 2)
								{
									putNum = getNum;
									sprintf(putString, "%lu", putNum);		//int to str (putNum to putString)
									userInput[1][0] = getNum;				//userInput[menuLevel][0] = getNum;
									goAhead = 1;
								}
								else 
								{
									strcpy(putString, "invalid input");
									goAhead = 0;
								}
							}
							else if(userInput[0][0] == 2)	//speedRef 
							{
								if(getNum)	//if non-Zero
								{
									putNum = getNum;
									sprintf(putString, "%lu", putNum);		//int to str (putNum to putString)
									userInput[1][0] = getNum;				//userInput[menuLevel][0] = getNum;
									goAhead = 1;
								}
								else 
								{
									strcpy(putString, "invalid input");
									goAhead = 0;
								}
							}
							
							charToBeXmitted = strlen(putString);
							state = 2;
						}
						else 
						{
							getString[charReceived++] = (char)read_value;
						}
					}
				}
				else if(state == 2)	//response received-echo-back
				{
					static uint8_t charXmitted = 0;
					if(charXmitted < charToBeXmitted)	
					{
						count = Cy_SCB_UART_Put(USER_UART_HW, putString[charXmitted++]);
				        if(count == 0ul)
				        {
				          	handle_error();
				        }
					}
					else 
					{
						charXmitted = 0;
						state = 3;
					}
				}
				else if(state == 3)	//reset state
				{
					if(Delay_NonBlocking(1000U))
					{
						charReceived 	= 0;
						charToBeXmitted = 0;
						memset(getString, 0, sizeof(getString));
						memset(putString, 0, sizeof(putString));
		
						#if 0
						if(goAhead == false)
						{
							menuLevel = 0;
						}
						else 
						{
							menuLevel++;
						}
						#endif
						//menuLevel += goAhead ? 1: (menuLevel = 0);
						goAhead ? (menuLevel += 1) : (menuLevel = 0);
					
						Cy_SCB_UART_PutString(USER_UART_HW, "\x1b[2J\x1b[;H");	//clear screen
					}
				}
			}
			else if(menuLevel == 2)	//run-time menu
			{
				static uint8_t state = 0;

				if(!state)	//menu-printing (Observation Menu)
				{
					static uint8_t init = 0;
					
					if(!init)
					{
						if(userInput[0][0] == 1)			//start/stop
						{
							if(userInput[1][0] == 1)		//start
							{
								speedCmd = 500;
							}
							else if(userInput[1][0] == 2)	//stop
							{
								speedCmd = 0;
							}
						}
						else if(userInput[0][0] == 2)		//speedCmd
						{
							speedCmd = userInput[1][0];
						}

						init++;
					}
					else if(init == 1)
					{
						UART_GotoXY(1, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************\r\n");
					    Cy_SCB_UART_PutString(USER_UART_HW, "# HVAC Motor Control GUI\r\n");
					    Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************\r\n");

						UART_GotoXY(8, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# Parameters \r\n");
						UART_GotoXY(8, 35);
						Cy_SCB_UART_PutString(USER_UART_HW, "Value \r\n");

						UART_GotoXY(10, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# speedCmd: \r\n");
						//UART_GotoXY(11, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# speedRef: \r\n");
						//UART_GotoXY(12, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# speedFdb: \r\n");
						//UART_GotoXY(13, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# mode: \r\n");
						//UART_GotoXY(14, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# temprature: \r\n");
						//UART_GotoXY(15, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# greenLED: \r\n");
						//UART_GotoXY(16, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# redLED: \r\n");
						//UART_GotoXY(17, 1);
						Cy_SCB_UART_PutString(USER_UART_HW, "# fault: \r\n");

						init++;
					}
					else if(init == 2)
					{
						if(Delay_NonBlocking(100U))
						{
							init++;
						}
					}
					else 
					{
						UART_GotoXY((loopCount + 10), 35);	//goToXY for parameters-print
						state = 1;
					}
				}
				else if(state == 1)	//inputing
				{	
					uint32_t putNum = getParaVal(loopCount);;
					//sprintf(putString, "%lu", putNum);			//int to str (putNum to putString)						
					//charToBeXmitted = strlen(putString);
					charToBeXmitted = (uint8_t)sprintf(putString, "%lu", putNum);

					state = 2;
				}
				else if(state == 2)	//received & echo-back
				{
					static uint8_t charXmitted = 0;
					if(charXmitted < charToBeXmitted)	
					{
						count = Cy_SCB_UART_Put(USER_UART_HW, putString[charXmitted++]);
				        if(count == 0ul)
				        {
				          	handle_error();
				        }
					}
					else 
					{
						charXmitted = 0;
						state = 3;
					}
				}
				else if(state == 3)	//reset state
				{
					if(Delay_NonBlocking(100U))
					{
						charReceived 	= 0;
						charToBeXmitted = 0;
						memset(getString, 0, sizeof(getString));
						memset(putString, 0, sizeof(putString));
	
						if(++loopCount >= 8)
						{
							loopCount = 0;
						}
						
						state = 0;
					}
				}
			}
		#endif
	}
}

/* [] END OF FILE */
