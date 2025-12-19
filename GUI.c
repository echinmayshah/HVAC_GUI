/*
 * GUI.c
 *
 *  Created on: Dec 11, 2025
 *      Author: Chinmay.Shah
 */

#include "GUI.h"
#include "mtb_hal_timer.h"
#include "var.h"

#define CLEAR_SCREEN() Cy_SCB_UART_PutString(USER_UART_HW, "\x1b[2J\x1b[;H");	

#if GUI
	bool stateInit = 0;

	uint32_t read_value;// = CY_SCB_UART_RX_NO_DATA;
	uint8_t charReceived = 0;
	char getString[100] = {0};
	uint8_t charToBeXmitted = 0;
	char putString[100] = {0};
	
	uint32_t count;
	bool goBackMainMenu = false;

	bool motorInstance = 0;
	
	uint32_t mode = 0;
		
	uint8_t menuLevel = 0;
	bool goAhead = 0;
	uint8_t loopCount = 0;
	
	uint16_t userDataInput[2][1] = {0};
	float speedStart = 0.5f;
#endif

static inline uint32_t random_range(uint32_t , uint32_t );
static uint32_t getParaVal(uint8_t );
static bool Delay_NonBlocking(uint32_t );
static void UART_GotoXY(uint8_t , uint8_t );


static inline uint32_t random_range(uint32_t min, uint32_t max)
{
    return (rand() % (max - min + 1)) + min;
}

static inline uint8_t popcount_u32(uint32_t v)
{
    //return (uint8_t)__builtin_popcount(v);
    
    uint8_t count = 0;

    while (v)
    {
        v &= (v - 1U);  // clears the lowest set bit
        count++;
    }

    return count;
}
								
static uint32_t getParaVal(uint8_t loopCount)
{
    uint32_t getNum = 0U;

    switch (loopCount)
    {
        case 0U:	//speedCmd
            //speedCmd = random_range(1, 1000);
            //getNum = speedCmd;
           
			if(motorInstance == false)
				 getNum = speedCmd_M0;
			else if(motorInstance == true)
				 getNum = speedCmd_M1;
            break;

		#if 0
	        case 1:	//speedRef
	            m[motorInstance].speedRef = m[motorInstance].speedDesired;//random_range(1, 1000);
	            //if (speedRef > speedCmd)
	                //speedRef = speedCmd;
	            getNum = m[motorInstance].speedRef;
	            break;
        #endif

        case 1U:	//speedFdb
            //speedFdb = random_range(1, 1000);
            //getNum = speedFdb;
            if(motorInstance == false)
            {
				if(isrState[motorInstance] == eClosed_Loop)
				{
					getNum = speedFdb_M0;
				}
				else 
				{
					getNum = 0U;
				}
			}
			else if(motorInstance == true)
			{
				if(isrState[motorInstance] == eClosed_Loop)
				{
					getNum = speedFdb_M1;
				}
				else 
				{
					getNum = 0U;
				}
			}
            break;

        case 2U:	//mode
            mode = (uint32_t)isrState[motorInstance];	//random_range(0, 3);
            getNum = mode;
            break;

        case 3U:	//temp
            //uint32_t temp = random_range(1, MAX_TEMPERATURE);
            //temprature = (temp < MIN_TEMPERATURE) ? MIN_TEMPERATURE : temp;
            getNum = motor[motorInstance].vars_ptr->temp_ps;
        	break;

		#if 0
        case 4U:	//greenLED
            //getNum = greenLED;
            if(motor[i].faults_ptr->flags.sw.reg == 0U)
            {
				getNum = false;
			}
			else 
			{
				getNum = true;
			}
            break;
        #endif
            
 		case 4U:	//greenLED
        case 5U:	//redLED
            //getNum = redLED;
            if(motor[motorInstance].faults_ptr->flags.sw.reg == 0U)
            {
				if(loopCount == 4U)			//greenLED
					getNum = 1U;
				else if(loopCount == 5U)	//redLED
					getNum = 0U;
			}
			else 
			{
				if(loopCount == 4U)			//greenLED
					getNum = 0U;
				else if(loopCount == 5U)	//redLED
					getNum = 1U;
			}
            break;

        case 6U:	//fault
        default:
            //fault = random_range(0, 5);
            //faults_ptr->flags.sw.oc;
            getNum = motor[motorInstance].faults_ptr->flags.sw.reg;
            break;
    }

	return(getNum);
}

static void UART_GotoXY(uint8_t row, uint8_t col)
{
    char buffer[20];

    sprintf(buffer, "\x1B[%d;%dH", row, col);
	Cy_SCB_UART_PutString(USER_UART_HW, buffer);
}

static bool Delay_NonBlocking(uint32_t requiredDelay_mSec)
{			
	static int32_t startTick = 0U;
	
	if (startTick == 0U)
	{
		startTick = gSysTick;          
	}
	else if ((gSysTick - startTick) >= requiredDelay_mSec)
	{
		startTick = 0;
		
		return(true);
	}

	return(false);
}

void handle_error(void)
{
     /* Disable all interrupts. */
    __disable_irq();

    CY_ASSERT(0);
}

#if 0
	bool guiStart_Call(void)
	{
		if(m[0].sysTick >= 1000U)
		{			
			Cy_GPIO_Write(DIR_LED_PORT, DIR_LED_NUM, m[0].toggle);
			Cy_GPIO_Write(FAULT_LED_ALL_PORT, FAULT_LED_ALL_NUM, m[0].toggle);
			
			m[0].toggle ^= 1U;
			
			m[0].sysSec++;
			if(m[0].sysSec >= 5U)
			{		
				Cy_GPIO_Write(DIR_LED_PORT, DIR_LED_NUM, 0);
				Cy_GPIO_Write(FAULT_LED_ALL_PORT, FAULT_LED_ALL_NUM, 0);
				
				m[0].sysTick = 0U;
				m[0].sysSec = 0U;
				
				return true;
			}
	
			m[0].sysTick = 0U;
		}
		
		return false;
	}
#endif

#if 1
	bool guiStart_Call(void)
	{
		static bool init = false;
		static uint32_t lastTick = 0U;
		static uint8_t secCnt  = 0U;
		static bool toggle = false;
	
		if(init == false)
		{
			lastTick = gSysTick;
			init = true;
		}
		else //if(init == true)
		{
			if ( (gSysTick - lastTick) >= 1000U )
		    {
		        lastTick += 1000U;      /* Maintain precise period */
		     
				secCnt ++;
				if(secCnt  >= 5U)
				{
					secCnt  = 0U;
					
					Cy_GPIO_Write(DIR_LED_PORT, DIR_LED_NUM, 0);
					Cy_GPIO_Write(FAULT_LED_ALL_PORT, FAULT_LED_ALL_NUM, 0);
	
					return true;
				}
				
				toggle ^= 1U;
				Cy_GPIO_Write(DIR_LED_PORT, DIR_LED_NUM, toggle);
				Cy_GPIO_Write(FAULT_LED_ALL_PORT, FAULT_LED_ALL_NUM, toggle);	
			}
		}
	
		return false;
	}
#endif
void GUI_Call(void)
{
	if(menuLevel == 0U)			//motor_0/motor_1?
	{
		static uint8_t state = 0U;

		if(state == 0U)	//title dash-board
		{
			UART_GotoXY(1, 1);
			Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************ \r\n");
		    Cy_SCB_UART_PutString(USER_UART_HW, "# HVAC Motor Control GUI \r\n");
		    Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************ \r\n");
			
			UART_GotoXY(5, 1);
			Cy_SCB_UART_PutString(USER_UART_HW, "# Page_0 - Main Page \r\n");
			Cy_SCB_UART_PutString(USER_UART_HW, "# Enter you choice (followed by enter) \r\n");
			Cy_SCB_UART_PutString(USER_UART_HW, "# but, don't enter until it prompt \r\n");
			
			UART_GotoXY(9, 1);
			Cy_SCB_UART_PutString(USER_UART_HW, "# Enter 0 for motor_0 \r\n");
			Cy_SCB_UART_PutString(USER_UART_HW, "# Enter 1 for motor_1 \r\n");
			
			UART_GotoXY(12, 1);
			Cy_SCB_UART_PutString(USER_UART_HW, "# Your input?: ");
			UART_GotoXY(12, 16);

			goAhead = false;
			state = 1U;
		}
		else if(state == 1U)	//receiving
		{
			read_value = Cy_SCB_UART_Get(USER_UART_HW);

			if (read_value != CY_SCB_UART_RX_NO_DATA)
			{
			    char rx = (char)read_value;
			
			    if (rx == '\n')
			    {
			        uint32_t getNum = 0U;
			        uint32_t putNum = 0U;
			
			        getString[charReceived] = '\0'; 	//safely termination
			        charReceived = 0U;
			
			        if (sscanf(getString, "%lu", &getNum) == 1U)	//string to integer
			        {
			            if ((getNum == 0U) || (getNum == 1U))
			            {
			                putNum = getNum;
			                (void)sprintf(putString, "%lu", putNum);
			                userDataInput[0][0] = putNum;
			                goAhead = true;
			            }
			            else
			            {
			                (void)strcpy(putString, "invalid input");
			            }
			        }
			        else
			        {
			            (void)strcpy(putString, "invalid input");
			        }

			        charToBeXmitted = (uint8_t)strlen(putString);
			        state = 2U;
			    }
			    else
			    {
			        if (charReceived < (sizeof(getString) - 1U))	 // avoid buffer overflow
			        {
			            getString[charReceived++] = rx;
			        }
			    }
			}
			else
			{
			    if(Delay_NonBlocking(5000U))
			    {
		            charReceived 	= 0U;
					charToBeXmitted = 0U;
					memset(getString, 0U, sizeof(getString));
					memset(putString, 0U, sizeof(putString));
					
					state = 0U;
					CLEAR_SCREEN()
					
					defautSpeedStart = true;
					menuLevel = 3U;
			    }
			}
		}
		else if(state == 2U)	//response received-echo-back
		{
			static uint8_t charXmitted = 0U;

			if(charXmitted < charToBeXmitted)	
			{
				count = Cy_SCB_UART_Put(USER_UART_HW, putString[charXmitted++]);
		        if(count == 0U)
		        {
		          	handle_error();
		        }
			}
			else 
			{
				charXmitted = 0U;
				state = 3U;
			}
		}
		else if(state == 3U)	//reset state
		{
			if(Delay_NonBlocking(2500U))	//delay 1) to display user-input 2) mandatory-del after Xmission
			{
				charReceived 	= 0U;
				charToBeXmitted = 0U;
				memset(getString, 0U, sizeof(getString));
				memset(putString, 0U, sizeof(putString));

				// Update menu level
				goAhead ? (menuLevel += 1U) : (menuLevel = 0U);
			
				// Clear terminal screen
				CLEAR_SCREEN()	
				state = 0U;
			}
		}
	}
	else if(menuLevel == 1U)	//start/stop, speedCmd
	{
		static uint8_t state = 0U;

		if(state == 0U)
		{
			UART_GotoXY(1, 1);
			Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************ \r\n");
		    Cy_SCB_UART_PutString(USER_UART_HW, "# HVAC Motor Control GUI \r\n");
		    Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************ \r\n");
			
			UART_GotoXY(5, 1);
			Cy_SCB_UART_PutString(USER_UART_HW, "# Page_1 \r\n");
			
			UART_GotoXY(8, 1);
			Cy_SCB_UART_PutString(USER_UART_HW, "# Enter 1 to start/stop motor \r\n");
			Cy_SCB_UART_PutString(USER_UART_HW, "# Enter 2 for speedCmd \r\n");
			
			UART_GotoXY(11, 1);
			Cy_SCB_UART_PutString(USER_UART_HW, "# Your input?: ");	//\r\n");
			UART_GotoXY(11, 16);

			goAhead = false;
			state = 1U;
		}
		else if(state == 1U)	//receiving
		{
			read_value = Cy_SCB_UART_Get(USER_UART_HW);

			if (read_value != CY_SCB_UART_RX_NO_DATA)
			{
			    char rx = (char)read_value;
			
			    if (rx == '\n')
			    {
			        uint32_t getNum = 0U;
			        uint32_t putNum = 0U;
			
			        getString[charReceived] = '\0'; 	//safely termination
			        charReceived = 0U;
			        
			        if (sscanf(getString, "%lu", &getNum) == 1U)	//string to integer
			        {
			            if ((getNum == 1U) || (getNum == 2U))
			            {
			                putNum = getNum;
			                (void)sprintf(putString, "%lu", putNum);
			                userDataInput[0][0] = putNum;
			                goAhead = true;
			            }
			            else
			            {
			                (void)strcpy(putString, "invalid input");
			            }
			        }
			        else
			        {
			            (void)strcpy(putString, "invalid input");
			        }

			        charToBeXmitted = (uint8_t)strlen(putString);
			        state = 2U;
			    }
			    else
			    {
			        if (charReceived < (sizeof(getString) - 1U))	 // avoid buffer overflow
			        {
			            getString[charReceived++] = rx;
			        }
			    }
			}
			else
			{
			    if(Delay_NonBlocking(5000U))
			    {
		            charReceived 	= 0U;
					charToBeXmitted = 0U;
					memset(getString, 0U, sizeof(getString));
					memset(putString, 0U, sizeof(putString));
					
					state = 0U;
					CLEAR_SCREEN()
					
					defautSpeedStart = true;
					menuLevel = 3U;
			    }
			}
		}
		else if(state == 2U)	//response received-echo-back
		{
			static uint8_t charXmitted = 0U;

			if(charXmitted < charToBeXmitted)	
			{
				count = Cy_SCB_UART_Put(USER_UART_HW, putString[charXmitted++]);
		        if(count == 0U)
		        {
		          	handle_error();
		        }
			}
			else 
			{
				charXmitted = 0U;
				state = 3U;
			}
		}
		else if(state == 3U)	//reset state
		{
			if(Delay_NonBlocking(2500U))	//delay 1) to display user-input 2) mandatory-del after Xmission
			{
				charReceived 	= 0U;
				charToBeXmitted = 0U;
				memset(getString, 0U, sizeof(getString));
				memset(putString, 0U, sizeof(putString));

				// Update menu level
				goAhead ? (menuLevel += 1U) : (menuLevel = 0U);
			
				// Clear terminal screen
				CLEAR_SCREEN()	
				state = 0U;
			}
		}
	}
	else if(menuLevel == 2U)	//speedCmd
	{
		static uint8_t state = 0U;

		if(state == 0U)
		{
			if(userDataInput[0][0] == 1U)		//start/stop
			{
				UART_GotoXY(1, 1);
				Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************ \r\n");			//line # 1
			    Cy_SCB_UART_PutString(USER_UART_HW, "# HVAC Motor Control GUI \r\n");
			    Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************ \r\n");

				UART_GotoXY(5, 1);
				Cy_SCB_UART_PutString(USER_UART_HW, "# Page_1.0 - Start/Stop Motor \r\n");
				
				UART_GotoXY(7, 1);
				Cy_SCB_UART_PutString(USER_UART_HW, "# Enter 1 to start motor \r\n");
				Cy_SCB_UART_PutString(USER_UART_HW, "# Enter 2 to stop motor \r\n");
				
				UART_GotoXY(10, 1);	
				Cy_SCB_UART_PutString(USER_UART_HW, "# Your input?: ");
				UART_GotoXY(10, 16);	//go to line # 10 to take input from user
			}
			else if(userDataInput[0][0] == 2U)	//speedCmd
			{
				UART_GotoXY(1, 1);
				Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************\r\n");			//line # 1
			    Cy_SCB_UART_PutString(USER_UART_HW, "# HVAC Motor Control GUI\r\n");
			    Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************\r\n");

				UART_GotoXY(5, 1);
				Cy_SCB_UART_PutString(USER_UART_HW, "# Page_1.1 - speedCmd \r\n");

				UART_GotoXY(7, 1);	
				Cy_SCB_UART_PutString(USER_UART_HW, "# Enter speedCmd \r\n");
				
				UART_GotoXY(10, 1);		//go to line # 10 to take input from the user
				Cy_SCB_UART_PutString(USER_UART_HW, "# Your input?: \r\n");
				UART_GotoXY(10, 16);	
			}
				
			goAhead = false;
			state = 1U;
		}
		else if(state == 1U)	//receiving
		{	
			read_value = Cy_SCB_UART_Get(USER_UART_HW);
			if(read_value != CY_SCB_UART_RX_NO_DATA)
			{
				if((char)read_value == '\n')
				{
					uint32_t getNum, putNum;		
				
					sscanf(getString, "%lu", &getNum);				//str to int (getString to getNum)
					
					if(userDataInput[0][0] == 1U)	//start/stop
					{
						if(getNum == 1U || getNum == 2U)
						{
							putNum = getNum;
							sprintf(putString, "%lu", putNum);		//int to str (putNum to putString)
							userDataInput[1][0] = getNum;			//userDataInput[menuLevel][0] = getNum;
							goAhead = true;
						}
						else 
						{
							strcpy(putString, "invalid input");
						}
					}
					else if(userDataInput[0][0] == 2U)	//speedRef 
					{
						if(getNum)	//if non-Zero
						{
							putNum = getNum;
							sprintf(putString, "%lu", putNum);		//int to str (putNum to putString)
							userDataInput[1][0] = getNum;			//userDataInput[menuLevel][0] = getNum;
							goAhead = true;
						}
						else 
						{
							strcpy(putString, "invalid input");
						}
					}
					
					charToBeXmitted = strlen(putString);
					state = 2U;
				}
				else 
				{
					getString[charReceived++] = (char)read_value;
				}
			}
		}
		else if(state == 2U)	//response received-echo-back
		{
			static uint8_t charXmitted = 0;

			if(charXmitted < charToBeXmitted)	
			{
				count = Cy_SCB_UART_Put(USER_UART_HW, putString[charXmitted++]);
		        if(count == 0U)
		        {
		          	handle_error();
		        }
			}
			else 
			{
				charXmitted = 0U;
				state = 3U;
			}
		}
		else if(state == 3U)	//reset state
		{
			if(Delay_NonBlocking(2500U))	//delay 1)to display use-input & 2)mandatory-del after Xmission
			{
				charReceived 	= 0U;
				charToBeXmitted = 0U;
				memset(getString, 0U, sizeof(getString));
				memset(putString, 0U, sizeof(putString));

				goAhead ? (menuLevel += 1U) : (menuLevel = 0U);
			
				CLEAR_SCREEN()	//clear screen
				state = 0U;
			}
		}
	}
	else if(menuLevel == 3U)	//run-time menu
	{
		static uint8_t state = 0U;
		static uint8_t init = 0U;
		static bool userInput = false;

		if(state == 0U)	//dash board-printing
		{
			if(init == 0U)	//menu specific
			{
				if(userDataInput[0][0] == 1U)			//start/stop
				{
					if(userDataInput[1][0] == 1U)		//start
					{
						m[0].speedDesired 	= 0.9f;
						m[0].speedCmd 		= speedStart;
						m[0].speedInc		= 0.05f;
						m[0].speedDec		= 0.05f;
						m[0].fullThrotole 	= false;
						m[0].startMotor 	= true;
						
						m[1].speedDesired 	= 0.9f;
						m[1].speedCmd 		= speedStart;
						m[1].speedInc		= 0.025f;
						m[1].speedDec		= 0.075f;
						m[1].fullThrotole 	= false;
						m[1].startMotor 	= true;
						
						emStop[0] = emStop[1] = false;
					}
					else if(userDataInput[1][0] == 2U)	//stop
					{
						//m[0].speedDesired = 0.0f;
						//m[1].speedDesired = 0.0f;
						m[0].startMotor 	= false;
						m[1].startMotor 	= false;
					}
				}
				else if(userDataInput[0][0] == 2U)		//speedCmd
				{
					if(userDataInput[1][0] >= 1507U)
						userDataInput[1][0] = 1507U;
					
					m[0].speedDesired 	= 0.9f;
					m[0].speedCmd 		= (float)userDataInput[1][0]/1675.00;
					m[0].speedInc		= 0.075f;
					m[0].speedDec		= 0.1f;
					m[0].fullThrotole 	= false;
					m[0].startMotor 	= true;
					
					m[1].speedDesired 	= 0.9f;
					m[1].speedCmd 		= (float)userDataInput[1][0]/1675.00;
					m[1].speedInc		= 0.1f;
					m[1].speedDec		= 0.075f;
					m[1].fullThrotole 	= false;
					m[1].startMotor 	= true;
					
					emStop[0] = emStop[1] = false;
				}

				m[0].gearUp = 0U;	
				init = 1U;
			}
			else if(init == 1U)	//para-disp
			{
				UART_GotoXY(1, 1);
				Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************\r\n");
			    Cy_SCB_UART_PutString(USER_UART_HW, "# HVAC Motor Control GUI\r\n");
			    Cy_SCB_UART_PutString(USER_UART_HW, "# ************************************************************\r\n");

				UART_GotoXY(8, 1);
				Cy_SCB_UART_PutString(USER_UART_HW, "# Parameters \r\n");
				UART_GotoXY(8, 35);
				Cy_SCB_UART_PutString(USER_UART_HW, "Motor_0");
				UART_GotoXY(8, 70);
				Cy_SCB_UART_PutString(USER_UART_HW, "Motor_1");

				UART_GotoXY(10, 1);
				Cy_SCB_UART_PutString(USER_UART_HW, "# speedCmd: \r\n");
				//UART_GotoXY(11, 1);
				//Cy_SCB_UART_PutString(USER_UART_HW, "# speedRef: \r\n");
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

				UART_GotoXY(20, 1);
				Cy_SCB_UART_PutString(USER_UART_HW, "# enter 0 for Emergency Stop - Motor_0: \r\n");
				Cy_SCB_UART_PutString(USER_UART_HW, "# enter 1 for Emergency Stop - Motor_1: \r\n");
				Cy_SCB_UART_PutString(USER_UART_HW, "# enter 2 for Page_0: ");
				UART_GotoXY(22, 35);

				init = 2U;
			}
			else if(init == 2U)	//delay
			{
				if(Delay_NonBlocking(1000U))
				{
					init = 3U;
				}
			}
			else if(init == 3U) 	
			{
				loopCount 	= 0U;
				goAhead 	= false;
				init 		= 0U;
				state 		= 1U;
				userInput	= false;
			}
		}
		else if(state == 1U)	//getting user-input
		{
			read_value = Cy_SCB_UART_Get(USER_UART_HW);

			if(read_value != CY_SCB_UART_RX_NO_DATA)	//check-resp
			{
				char rx = (char)read_value;
				
				if (rx == '\n')
				{
					uint32_t getNum = 0xFFU;
					
					getString[charReceived] = '\0'; //safe termination
					charReceived = 0U;
					
					if (sscanf(getString, "%lu", &getNum) == 1)	//string to integer 
					{
						if (getNum == 0U)
						{
							strcpy(putString, "Emergency-Stop req ");
							charToBeXmitted = (uint8_t)strlen(putString);
							m[0].speedCmd = 0.0f;
							emStop[0] = true;
							goBackMainMenu = true;
							goAhead = 1U;
						}
						else if (getNum == 1U)
						{
							strcpy(putString, "Emergency-Stop req ");
							charToBeXmitted = (uint8_t)strlen(putString);
							m[1].speedCmd = 0.0f;
							emStop[1] = true;
							goBackMainMenu = true;
							goAhead = 1U;
						}
						else if (getNum == 2U)
						{
							strcpy(putString, "Page_0 req ");
							charToBeXmitted = (uint8_t)strlen(putString);
							goBackMainMenu = true;
							goAhead = 1U;
						}
						else
						{
							strcpy(putString, "invalid input ");
							charToBeXmitted = (uint8_t)strlen(putString);
						}
					}
					else
					{
						(void)strcpy(putString, "invalid input ");
						charToBeXmitted = (uint8_t)strlen(putString);
					}

					state = 2U;
					userInput = true;
				}
				else //if (rx != '\n')
				{
					if (charReceived < (sizeof(getString) - 1U))	// avoid buffer overflow
					{
						getString[charReceived++] = rx;
					}
				}
			}
			else	//para-round-robin loop
			{
				if(Delay_NonBlocking(100U))	//(1000U))
				{
					uint32_t putNum = getParaVal(loopCount);

					switch(loopCount)
					{
						case 2U:	//mode
						{
							if(putNum == 0U)
							{	
								strcpy(putString, 	"Brake Boot             ");
												   //1234567890123456789012345
							}
							else if(putNum == 1U)
							{
								strcpy(putString, 	"Open Loop              ");
												   //1234567890123456789012345
							}
							else if(putNum == 2U)
							{
								strcpy(putString, 	"OL_to_CL               ");
												   //1234567890123456789012345
							}
							else if(putNum == 3U)
							{
								strcpy(putString, 	"Closed Loop            ");
												   //1234567890123456789012345
							}
							else if(putNum == 4U)
							{
								strcpy(putString, 	"Fault                  ");
												   //1234567890123456789012345
							}
							charToBeXmitted = (uint8_t)strlen(putString);

							break;
						}
						
						//if to LEDs are needed
						case 4U:	//greenLED
						case 5U:	//redLED
						{
							if(putNum == 1U)
							{	
								strcpy(putString, "On ");
							}
							else 
							{
								strcpy(putString, "Off");
							}
							charToBeXmitted = (uint8_t)strlen(putString);

							break;
						}
						
						case 6U:	//fault
						{
							if(putNum == 0U)
							{	
								strcpy(putString, 	"No Fault               ");
												   //1234567890123456789012345
							}
							else 
							{
								uint8_t noOfCountSet;
								
								noOfCountSet = popcount_u32(putNum);
								
								if(noOfCountSet > 1U)
								{
									#define OC (1 << 0)
									#define OT (1 << 1)
									#define OV (1 << 2)
									#define UV (1 << 3)
									
									//strcpy(putString, 	"Multiple Fault         ");
													   ////1234567890123456789012345
									
									if( (putNum & OC) && (putNum & OT) && (putNum & UV) )
									{
										strcpy(putString, 	"OC_OT_UV               ");
														   //1234567890123456789012345
									}
									else if( (putNum & OC) && (putNum & OT) )
									{
										strcpy(putString, 	"OC_OT                  ");
														   //1234567890123456789012345
									}
									else if( (putNum & OC) && (putNum & UV) )
									{
										strcpy(putString, 	"OC_UV                  ");
														   //1234567890123456789012345
									}	
									else if( (putNum & OT) && (putNum & UV) )
									{
										strcpy(putString, 	"OT_UV                  ");
														   //1234567890123456789012345
									}	
									else 
									{
										strcpy(putString, 	"Other                  ");
														   //1234567890123456789012345
									}
								}
								else 
								{
									if(putNum & OC)
									{
										strcpy(putString, 	"OverCurr               ");
														   //1234567890123456789012345
									}
									else if(putNum & OT)
									{
										strcpy(putString, 	"OverTemp               ");
														   //1234567890123456789012345
									}	
									else if(putNum & OV)
									{
										strcpy(putString, 	"OverVolt               ");
														   //1234567890123456789012345
									}	
									else if(putNum & UV)
									{
										strcpy(putString, 	"UnderVolt              ");
														   //1234567890123456789012345
									}	
									else 
									{
										strcpy(putString, 	"Other           		");
														   //1234567890123456789012345
									}
								}
							}
							charToBeXmitted = (uint8_t)strlen(putString);
							
							break;
						}
						
						default:
						{
							if(loopCount == 1U)
							{
								if(putNum == 0U)
								{	
									strcpy(putString, "Out of Closed Loop       ");
													   //1234567890123456789012345
									charToBeXmitted = (uint8_t)strlen(putString);
								}
								else
								{
									charToBeXmitted = (uint8_t)sprintf(putString, "%-20lu", putNum);
								}
							}
							else 
							{
								charToBeXmitted = (uint8_t)sprintf(putString, "%-4lu", putNum);
							}
							
							break;
						}
					}

					if(motorInstance == false)
						UART_GotoXY((loopCount + 10), 35);	//goToXY for parameters-print
					else if(motorInstance == true)
						UART_GotoXY((loopCount + 10), 70);	//goToXY for parameters-print

					if(++loopCount >= 7U)
					{
						loopCount = 0U;
						motorInstance ^= 1;
						
						if(motorInstance == 0U)
						{
							if(m[0].gearUp == 1U)
								m[0].gearUp = 2U;
							
							if(m[1].gearUp == 1U)
								m[1].gearUp = 2U;
						}
					}						

					state = 2U;	//7U;
				}
			}
		}
		else if(state == 2U)	//wait-state
		{
			if(Delay_NonBlocking(100U))	//mandatory del for UART_GotoXY();
			{
				state = 3U;
			}
		}
		else if(state == 3U)	//received & echo-back
		{
			static uint8_t charXmitted = 0U;

			if(charXmitted < charToBeXmitted)	
			{
				count = Cy_SCB_UART_Put(USER_UART_HW, putString[charXmitted++]);
		        if(count == 0U)
		        {
		          	handle_error();
		        }
			}
			else 
			{
				charXmitted = 0U;
				state = 4U;
			}
		}
		else if(state == 4U)	//received & echo-back
		{
			if(userInput == true)
			{
				if(Delay_NonBlocking(1200U))
				{
					if(goAhead == 0U)
					{
						UART_GotoXY(20, 35); //to erase invalid-input, so start from (20, 35)
						Cy_SCB_UART_PutString(USER_UART_HW, "                       ");
										   				   //1234567890123456789012345
					}
						
					UART_GotoXY(20, 35); //for next iteration
					state = 5U;
				}
			}
			else 
			{
				UART_GotoXY(20, 35); //for next iteration
				state = 5U;
			}
		}
		else if(state == 5U)	//reset state
		{
			if(Delay_NonBlocking(100U))	//mandatory del for UART_GotoXY();
			{
				charReceived 	= 0U;
				charToBeXmitted = 0U;
				memset(getString, 0U, sizeof(getString));
				memset(putString, 0U, sizeof(putString));

				goAhead = false;
				userInput = false;
				state = 1U;

				if(goBackMainMenu == true)
				{
					CLEAR_SCREEN()
				
					menuLevel = 0U;
					loopCount = 0U;
					goBackMainMenu = false;
					state = 0U;
				}
			}
		}
	}
}

#define LED 0U

#if LED
	#define LED_ON     (0UL)
	#define LED_OFF    (1UL)
#endif

#define BLINK_RATE_TICKS  (1000U)

void toggle_led_on_systick_handler(void)
{
	static uint32_t tick = 0U;
	
	#if 0
		gDel.gTick++;
	#endif
	
	gSysTick++;

	if (++tick >= BLINK_RATE_TICKS)
	{
		gSysSec++;
		tick = 0U;
	}
	
	#if 0
		//bool i = (bool)motor_ptr->motor_instance;
		
		if(m[0].startMotor == false &&	\
			m[0].stopMotor == false)
		{
			m[0].sysTick++;
		}
	#endif
}



