/*!
 ********************************************************************
   @file            cos_systime.c

   @brief  Dieses Modul benutzt CMT0 als Systemuhr. Es wird in  COS gebraucht

   @par Author    : Ernst Forgber (Fgb)
 ********************************************************************

   @par History   :
   @verbatim
   Version | Date        | Author        | Change Description
   0.0     | 03.04. 2013 | Fgb           | First Version
   0.1     | 01.08. 2013 | Fgb           | bugfix in _milliSecToTicks()
   0.2     | 09.10. 2015 | Fgb           | Umstieg auf renesas controller
   @endverbatim

 ********************************************************************/

/*!\mainpage Beschreibung
Die Zeit wird in Ticks angegeben. Die Beschreibung der
Funktionen ist in cos_systime.c zu finden.
Bei jedem Ueberlauf
von Timer0 wird ein Tick-Zaehler incrementiert. Falls
das Symbol DEBUG ungleich null definiert ist, wird eine LED
in der ISR gesetzt und rueckgesetzt.
 **********************************************************************/

/**************************************************************************
    Copyright 2014 Ernst Forgber

    Dieses Programm ist Freie Software: Sie können es unter den Bedingungen
    der GNU General Public License, wie von der Free Software Foundation,
    Version 3 der Lizenz oder (nach Ihrer Wahl) jeder neueren
    veröffentlichten Version, weiterverbreiten und/oder modifizieren.

    Dieses Programm wird in der Hoffnung, dass es nützlich sein wird, aber
    OHNE JEDE GEW�HRLEISTUNG, bereitgestellt; sogar ohne die implizite
    Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
    Siehe die GNU General Public License fÜr weitere Details.

    Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
    Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.

*****************************************************************************/

#include "cos_systime.h"
#include "iodefine.h"
#include "isr.h"


#define MICROSEC_PER_TICK 1000



/************************************************************
 * Debugging section:
 * Timer ISR used LED15 for debugging
 ************************************************************/
#define DEBUG 0   // 0 for no debugging with LEDs

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












/****************************************************************
 * static variables
 ****************************************************************/

static volatile uint16_t systemTimeInTicks=0; /*!< privater Zaehler */




/*!
 **********************************************************************
 * @par Beschreibung:
   Interrupt Service Routine (ISR) fuer den Ueberlauf von Timer0

 * @see
 * @arg
 *
 * @param  - keine
 * @retval - keiner
 ************************************************************************/
/*******************************
 * angelehnt an 'RX63N_Update.pdf, pp 421. ABER: der dort
 * angegebene Interrupt funktionierte nicht! Daher wurde
 * Timer CMT0 mit 'compare match timer0' benutzt, wie
 * auch bei FreeRtos.
 *******************************/
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
    systemTimeInTicks++;    // Ueberlauf zaehlen
}




/*!
 **********************************************************************
 * @par Beschreibung:
   Diese Funktion initialisiert Timer0 fuer den Betrieb als Systemuhr.
   Die Zeit wird in Ticks gemessen, ein Tick entspricht einem Ueberlauf
   von Timer0. Die Laenge eines Ticks wird eingestellt durch einen
   Vorteiler, der den CPU-Takt teilt und den Startwert des Timer0, von
   dem ab bis zum Ueberlauf hochgezaehlt wird.
 *
 * @verbatim
 *******************************
  * angelehnt an 'RX63N_Update.pdf, pp 421. ABER: der dort
  * angegebene Interrupt funktionierte nicht! Daher wurde
  * Timer CMT0 mit 'compare match timer0' benutzt, wie
  * auch bei FreeRtos.
  *******************************
  @endverbatim

 * @see
 * @arg
 *
 *
 * @param  - keine
 * @retval                - kein Rueckgabewert
 *
 * @par Beispiel :
 * @verbatim
  @endverbatim
 ************************************************************************/
 void _initSystemTime(void)
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
//-------------------------------------------------------


/*!
 **********************************************************************
 * @par Beschreibung:
 *   Diese Funktion liefert die Anzahl der Mikrosekunden, die einem
 *   Timer Tick entsprechen
 * @see
 * @arg _initSystemTime()
 *
 * @param  - keine
 *
 * @retval                - Tick-Interval in Mikrosekunden
  ************************************************************************/
 uint16_t _microSecPerTick(void)
{   return MICROSEC_PER_TICK;
}

/*-------------------------------------------------------*/

/*!
 **********************************************************************
 * @par Beschreibung:
 *   Diese Funktion gibt die Systemzeit in Ticks zurueck.
 * @param  - keine
 *
 * @retval                - Systemzeit in Ticks
 ************************************************************************/
 uint16_t _gettime_Ticks(void)
{   uint16_t t;
    //cli();  // INT sperren, exklusiven Zugriff sichern
    t = systemTimeInTicks;
    //sei(); // INT freigeben, Timer0 ISR darf systemTimeInTicks aendern
    return t;
}
/*-------------------------------------------------------*/
/*!
 **********************************************************************
 * @par Beschreibung:
 *   Diese Funktion rechnet ein Zeitintervall in Millisekunden um
 *   in die entsprechende Anzahl Ticks. Vorraussetzung dafuer ist,
 *   dass das Makro MICROSEC_PER_TICK richtig gesetzt ist.
 *
 * @param  milliSec  - IN, Zeit in Millisekunden
 *
 * @retval                - Zeit in Ticks
 *
 ************************************************************************/
 uint16_t _milliSecToTicks(uint16_t milliSec)
{   uint32_t t_ms;

    t_ms = ((uint32_t) milliSec*1000)/MICROSEC_PER_TICK;
    if (t_ms<1) t_ms=1;
    return ((uint16_t) t_ms);
}
/*-------------------------------------------------------*/




