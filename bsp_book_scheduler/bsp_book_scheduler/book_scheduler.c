/*
 * book_scheduler.c
 *
 *  Created on: 01.10.2015
 *      Author: Forgber
 */

/*
 * uses INT_Excep_TMR0_CMIA0 for timer interrupt!
 */

#include "book_scheduler.h"
#include "iodefine.h"
#include "isr.h"


#define MAX_TASKS 10

#ifndef NULL
	#define NULL ((void *)0)
#endif





/************************************************************
 * Debugging section:
 * Timer ISR used LED15 for debugging
 ************************************************************/
#define DEBUG 1   // 0 for no debugging with LEDs

#define _LED4		PORTD.PODR.BIT.B5
#define _LED5		PORTE.PODR.BIT.B3
#define _LED6		PORTD.PODR.BIT.B2
#define _LED7		PORTE.PODR.BIT.B0
#define _LED8		PORTD.PODR.BIT.B4
#define _LED9		PORTE.PODR.BIT.B2
#define _LED10		PORTD.PODR.BIT.B1
#define _LED11		PORTD.PODR.BIT.B7
#define _LED12		PORTD.PODR.BIT.B3
#define _LED13		PORTE.PODR.BIT.B1
#define _LED14		PORTD.PODR.BIT.B0
#define _LED15		PORTD.PODR.BIT.B6


#if DEBUG

static volatile unsigned char _led15_state=0;

#define _LedInitPortDirections()				\
{	/*
	 * port output data register (PODR)
	 * des ports d mit 1en belegen,
	 * d.h. leds an port d ausschalten
	 */											\
	PORTD.PODR.BYTE  = 0xFF;					\
												\
	/*
	 * port output data register (PODR)
	 * des ports e mit 1en belegen,
	 * d.h. leds an port e ausschalten
	 */											\
	PORTE.PODR.BYTE |= 0x0F;					\
												\
	/*
	 * port direction register (PDR)
	 * des ports d mit 1en belegen,
	 * d.h. leds an port d als
	 * ausgänge definieren
	 */											\
	PORTD.PDR.BYTE   = 0xFF;					\
												\
	/*
	 * port direction register (PDR)
	 * des ports e mit 1en belegen,
	 * d.h. leds an port e als
	 * ausgänge definieren
	 */											\
	PORTE.PDR.BYTE  |= 0x0F;					\
}

#else
  #define _LedInitPortDirections()
#endif  // DEBUG
/************************************************************
 * End of Debugging section
 ************************************************************/


/************************************************************
 * private section
 ************************************************************/
typedef struct
{	int initialTimerValue;
	int timer;
	int task_is_ready;  // 0 if not ready, 1 if ready to run
	int task_enable; // 1 if task is present and enabled, 0 if no task or suspended
	void (* task_func)(void);
} task_t;

static volatile task_t GBL_task_table[MAX_TASKS];

/*******************************
 * angelehnt an 'RX63N_Update.pdf, pp 421. ABER: der dort
 * angegebene Interrupt funktionierte nicht! Daher wurde
 * Timer CMT0 mit 'compare match timer0' benutzt, wie
 * auch bei FreeRtos.
 *******************************/
static void init_Timer_ISR(void)
{
	// geklaut aus FreeRtos
	/* Den Registerschutz abschalten.
	   PRCR = protect register. */
	SYSTEM.PRCR.WORD = 0xA50B;

	/* Den Modul-Stop-State des Compare-Match-Timer 0 abschalten.
	   MSTP = module stop state, das ist ein Makro aus iodefine.h. */
	MSTP( CMT0 ) = 0;

	/* Den Registerschutz einschalten. */
	SYSTEM.PRCR.WORD = 0xA500;

	/* Den Interrupt in der Peripherie aktivieren.
	   CMT0 = compare match timer 0,
	   CMCR = compare match timer control register,
	   CMIE = compare match interrupt enable. */
	CMT0.CMCR.BIT.CMIE = 1;

	/* Den Zählerstand, bei dem der Interrupt
	   ausgelöst werden soll, festlegen.
	   CMCOR = compare match timer constant register. */
	//CMT0.CMCOR = ( unsigned short ) ( ( ( configPCLKB_HZ / configTICK_RATE_HZ ) - 1 ) / 8 );
    //CMT0.CMCOR = 0x5555; // 3.5 ms
	//CMT0.CMCOR = 0x2222; // 1.5 ms
	//CMT0.CMCOR = 0x1560; // 0.917 ms
	//CMT0.CMCOR = 0x1600; // 0.956 ms
	CMT0.CMCOR = 0x1700; // 0.99 ms

	/* PCLKB/8 als Taktquelle wählen.
	   CKS = clock select. */
	CMT0.CMCR.BIT.CKS = 0;

	/* Die Priorität des Interrupts festlegen. */
	//IPR( CMT0, CMI0 ) = configKERNEL_INTERRUPT_PRIORITY;
    IPR(CMT0,CMI0) = 2;

	/* Den Interrupt in der ICU aktivieren. */
	IEN( CMT0, CMI0 ) = 1;

	/* Den Timer starten.
	   CMSTR0 = compare match timer start register 0,
	   STR0   = count start 0. */
	CMT.CMSTR0.BIT.STR0 = 1;
#if DEBUG
	_LedInitPortDirections();
#endif
}



void INT_Excep_CMT0_CMI0(void)
{
#if DEBUG
	if(_led15_state)
	{
		_LED15=0;
		_led15_state = 0;
	}
	else
	{
		_LED15=1;
		_led15_state = 1;
	}
#endif
	int i;
	for(i=0; i<MAX_TASKS; i++)
	{	if((GBL_task_table[i].task_func != NULL) &&
		(GBL_task_table[i].task_enable!=0) &&
		(GBL_task_table[i].timer > 0))
		{	if(--GBL_task_table[i].timer == 0)
			{	GBL_task_table[i].task_is_ready = 1;
				GBL_task_table[i].timer = GBL_task_table[i].initialTimerValue;
			}
		}
	}
}

/************************************************************
 * End of private section
 ************************************************************/




/************************************************************
 * exported functions
 ************************************************************/
void Init_Book_Scheduler(void)
{   int i;

	init_Timer_ISR();
	/* init all Tasks */
	for(i=0; i<MAX_TASKS; i++)
	{
		GBL_task_table[i].initialTimerValue = 0;
		GBL_task_table[i].task_enable = 0;
		GBL_task_table[i].task_is_ready = 0;
		GBL_task_table[i].timer = 0;
		GBL_task_table[i].task_func = NULL;
	}

}
/*-----------------------------------------------*/

int  Add_Task(void (*task)(void), int time, int priority)
{	// priority ok?
	if((priority >= MAX_TASKS) || (priority <0) )
	{ return -1;}
	// task already present?
	if(GBL_task_table[priority].task_func != NULL)
	{ return -2;}
	// schedule the task
	GBL_task_table[priority].task_func = task;
	GBL_task_table[priority].task_is_ready = 0;
	GBL_task_table[priority].initialTimerValue = time;
	GBL_task_table[priority].task_enable = 1;
	GBL_task_table[priority].timer = time;
	return 0;

}
/*-----------------------------------------------*/

void Remove_Task(int task_number)
{	// remove it
	GBL_task_table[task_number].initialTimerValue = 0;
	GBL_task_table[task_number].task_enable = 0;
	GBL_task_table[task_number].task_is_ready = 0;
	GBL_task_table[task_number].timer = 0;
	GBL_task_table[task_number].task_func = NULL;
}
/*-----------------------------------------------*/

void Enable_Task(int task_number)
{	GBL_task_table[task_number].task_enable = 1;
}
/*-----------------------------------------------*/

void Disable_Task(int task_number)
{	GBL_task_table[task_number].task_enable = 0;
}
/*-----------------------------------------------*/

void Set_Task_Period(int task_number, int new_timer_val)
{	GBL_task_table[task_number].initialTimerValue = new_timer_val;
	GBL_task_table[task_number].timer = new_timer_val;
}
/*-----------------------------------------------*/

void Create_Idle_Task(void)
{

}
/*-----------------------------------------------*/

void Run_Book_Scheduler(void)
{	int i;
    // loop forever
    while(1)
    {	// chech each task
    	for(i=0; i<MAX_TASKS; i++)
    	{	// task present?
    		if(GBL_task_table[i].task_func != NULL)
    		{	if(GBL_task_table[i].task_enable != 0) // enabled?
    			{	if(GBL_task_table[i].task_is_ready != 0)
    				{	GBL_task_table[i].task_is_ready = 0;
    					GBL_task_table[i].task_func();  // run it...
    				    break; // restart with i=0
    				}
    			}
    		}
    	}
    }
}
/*-----------------------------------------------*/

/************************************************************
 * End of exported functions
 ************************************************************/







