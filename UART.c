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
volatile uint8_t state = 0;

uint8_t loopCount = 0;
bool stateInit = 0;

uint32_t read_value;// = CY_SCB_UART_RX_NO_DATA;
uint8_t charReceived = 0;
char getString[100] = {0};
uint8_t charToBeXmitted = 0;
char putString[100] = {0};

uint32_t count;

#if SAFE_0
	uint8_t myIndex = 0;
	char tempString[100] = {0};
#endif

//SAFE_0 1st pretty code - need to work: space as a delimiter, blocking while loop, stringToNumber
//SAFE_1 removed space as a delimiter
//SAFE_2 removed blocking while loop in Xmission
//SAFE_3 SpeedRef updated val cont display
//SAFE_4 display SpeedRef_Xmitted = (SpeedRef_Received + 100); - using sprintf & sscanf

//SAFE_1 to SAFE_4 are raw materials for menu design

void UART_GotoXY(uint8_t row, uint8_t col)
{
    char buffer[20];

    sprintf(buffer, "\x1B[%d;%dH", row, col);
	Cy_SCB_UART_PutString(USER_UART_HW, buffer);
}

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
	
	#if 1
		Cy_SCB_UART_PutString(USER_UART_HW, "line # 1) ************************************************************\r\n");
	    Cy_SCB_UART_PutString(USER_UART_HW, "line # 2) HVAC Motor Control GUI\r\n");
	    Cy_SCB_UART_PutString(USER_UART_HW, "line # 3) ************************************************************\r\n\n");
	    Cy_SCB_UART_PutString(USER_UART_HW, "line # 5) >> Enter 1 to enter parameters, followed by enter \r\n\n");
	#endif

    __enable_irq();

    for(; ;)
    {
		#if SAFE_0//1
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

		#if SAFE_1//1
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
				memset(getString, 0, sizeof(putString));

				loopCount++;
				state = 0;
			}
		#endif

		#if 1//SAFE_4
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
				memset(getString, 0, sizeof(putString));

				loopCount++;
				state = 0;
			}
		#endif
	}
}

/* [] END OF FILE */
