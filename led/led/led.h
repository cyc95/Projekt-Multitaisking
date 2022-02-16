/*!
 *******************************************************************************
 *
 * @file			led.h
 *
 * @brief			Makros für die Kontrolle der zwölf LEDs LED4 bis LED15.
 *
 * @author			Christopher Zink
 *
 *******************************************************************************
 *
 * @mainpage		Dokumentation der Headerdatei led.h
 *
 * @par				Installation
 * 					Um die in der Headerdatei led.h definierten Makros zu
 * 					Kontrolle der zwölf LEDs LED4 bis LED15 des YRDKRX63N
 * 					nutzen zu können, müssen Sie die Headerdatei led.h
 * 					in ihr Projektverzeichnis kopieren und inkludieren.
 *
 * @par				Verbindungen
@verbatim
  Name auf dem Board |   Argument an Makros  |     Port      |     Bit
---------------------+-----------------------+---------------+--------------
		LED1		 |     nicht nutzbar     |       -       |      -
		LED2		 |     nicht nutzbar     |       -       |      -
		LED3		 |     nicht nutzbar     |       -       |      -
		LED4		 |         LED4          |       D       |      5
		LED5		 |         LED5          |       E       |      3
		LED6		 |         LED6          |       D       |      2
		LED7		 |         LED7          |       E       |      0
		LED8		 |         LED8          |       D       |      4
		LED9		 |         LED9          |       E       |      2
		LED10		 |         LED10         |       D       |      1
		LED11		 |         LED11         |       D       |      7
		LED12		 |         LED12         |       D       |      3
		LED13		 |         LED13         |       E       |      1
		LED14		 |         LED14         |       D       |      0
		LED15		 |         LED15         |       D       |      6
		LED16		 |     nicht nutzbar     |       -       |      -
		D5		     |     nicht nutzbar     |       -       |      -
@endverbatim
 *
 * @par				Beispielprogramm
@code{.c}
#include <limits.h>
#include "led.h"

static void delay(void);

void main(void) {

	int step;

	ledInitialize();

	for ( step = 0; ; step = (++step) % 12 ) {
		switch ( step ) {
			case 0 :
				ledTurnOn(LED4);
				ledTurnOff(LED15);
				delay();
				break;
			case 1 :
				ledTurnOn(LED5);
				ledTurnOff(LED4);
				delay();
				break;
			case 2 :
				ledTurnOn(LED6);
				ledTurnOff(LED5);
				delay();
				break;
			case 3 :
				ledTurnOn(LED7);
				ledTurnOff(LED6);
				delay();
				break;
			case 4 :
				ledTurnOn(LED8);
				ledTurnOff(LED7);
				delay();
				break;
			case 5 :
				ledTurnOn(LED9);
				ledTurnOff(LED8);
				delay();
				break;
			case 6 :
				ledTurnOn(LED10);
				ledTurnOff(LED9);
				delay();
				break;
			case 7 :
				ledTurnOn(LED11);
				ledTurnOff(LED10);
				delay();
				break;
			case 8 :
				ledTurnOn(LED12);
				ledTurnOff(LED11);
				delay();
				break;
			case 9 :
				ledTurnOn(LED13);
				ledTurnOff(LED12);
				delay();
				break;
			case 10:
				ledTurnOn(LED14);
				ledTurnOff(LED13);
				delay();
				break;
			case 11:
				ledTurnOn(LED15);
				ledTurnOff(LED14);
				delay();
				break;
		}
	}

}

static void delay(void) {

	int i;

	for ( i = 0; i < INT_MAX / 10000000; i++ ) {
		asm("nop");
	}

}
@endcode
 *
 *******************************************************************************
 */










// mehrfachincludeschutz
#ifndef __LED__
#define __LED__










/******************************************************************************************************************************************************************************
 *                                                                              Includes                                                                                      *
 ******************************************************************************************************************************************************************************/

//#include "iodefine.h"	// PORTD, PORTE
#include "../bsp/iodefine.h"	// PORTD, PORTE, liegt in /bsp









/******************************************************************************************************************************************************************************
 *                                                                             Konstanten                                                                                     *
 ******************************************************************************************************************************************************************************/

/*!
 * @brief		Als Argument an #ledToggle() gedacht
 */
#define LED4		PORTD.PODR.BIT.B5

/*!
 * @brief		Als Argument an #ledToggle() gedacht
 */
#define LED5		PORTE.PODR.BIT.B3

/*!
 * @brief		Als Argument an #ledToggle() gedacht
 */
#define LED6		PORTD.PODR.BIT.B2

/*!
 * @brief		Als Argument an #ledToggle() gedacht
 */
#define LED7		PORTE.PODR.BIT.B0

/*!
 * @brief		Als Argument an #ledToggle() gedacht
 */
#define LED8		PORTD.PODR.BIT.B4

/*!
 * @brief		Als Argument an #ledToggle() gedacht
 */
#define LED9		PORTE.PODR.BIT.B2

/*!
 * @brief		Als Argument an #ledToggle() gedacht
 */
#define LED10		PORTD.PODR.BIT.B1

/*!
 * @brief		Als Argument an #ledToggle() gedacht
 */
#define LED11		PORTD.PODR.BIT.B7

/*!
 * @brief		Als Argument an #ledToggle() gedacht
 */
#define LED12		PORTD.PODR.BIT.B3

/*!
 * @brief		Als Argument an #ledToggle() gedacht
 */
#define LED13		PORTE.PODR.BIT.B1

/*!
 * @brief		Als Argument an #ledToggle() gedacht
 */
#define LED14		PORTD.PODR.BIT.B0

/*!
 * @brief		Als Argument an #ledToggle() gedacht
 */
#define LED15		PORTD.PODR.BIT.B6










/******************************************************************************************************************************************************************************
 *                                                                               Makros                                                                                       *
 ******************************************************************************************************************************************************************************/

/*!
 ************************************************
 *
 * @brief	Dieses Makro initialisiert
 * 			die den LEDs 4 bis 15
 * 			zugeordneten Register.
 *
 * @details	Es werden die entsprechenden
 * 			Bits der Port-Output-Data-Register
 * 			(PODR) der Ports D und E
 * 			mit 1en belegt, sodass
 * 			die zwölf LEDs mit dem Zustand 'aus'
 * 			initialisiert werden.
 * 			Anschließend werden die
 * 			entsprechenden Bits der
 * 			Port-Direction-Register (PDR)
 * 			der Ports D und E mit 1en belegt,
 * 			sodass die LED-Pins als Ausgänge
 * 			fungieren und nun mit
 * 			den Makros #ledToggle(),
 * 			#ledTurnOn() und #ledTurnOff()
 * 			genutzt werden können.
 *
 ************************************************
 */
#define ledInitialize()							\
{												\
												\
	/*
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
												\
}

/*!
 ************************************************
 *
 * @brief	Dieses Makro invertiert den
 * 			Zustand der LED \c x.
 *
 * @param	x	LED deren Zustand invertiert
 * 			werden soll. Nutzen Sie zwingend
 * 			#LED4, #LED5, ..., #LED15.
 *
 * @note	Nutzen Sie zwingend
 * 			#LED4, #LED5, ..., #LED15 als
 * 			Argument!
 *
 * @pre		Rufen Sie zu Beginn ihres
 * 			Programms einmal #ledInitialize()
 * 			auf. Erst dann funktioniert dieses
 * 			Makro wie erwartet.
 *
 * @par		Beispiel
 *			@code{.c}
 *			#include "led.h"
 *
 *			void main(void) {
 *
 *				ledInitialize();
 *				ledToggle(LED4);
 *
 *			}
 *			@endcode
 *
 ************************************************
 */
#define ledToggle(x)							\
{												\
												\
	/*
	 * den wert des bits im PODR invertieren
	 * (siehe LED4, LED5, ..., LED15)
	 */											\
	(x) ^= 1;									\
												\
}

/*!
 ************************************************
 *
 * @brief	Dieses Makro schaltet die
 * 			LED \c x ein.
 *
 * @param	x	LED die eingeschaltet
 * 			werden soll. Nutzen Sie zwingend
 * 			#LED4, #LED5, ..., #LED15.
 *
 * @note	Nutzen Sie zwingend
 * 			#LED4, #LED5, ..., #LED15 als
 * 			Argument!
 *
 * @pre		Rufen Sie zu Beginn ihres
 * 			Programms einmal #ledInitialize()
 * 			auf. Erst dann funktioniert dieses
 * 			Makro wie erwartet.
 *
 * @par		Beispiel
 *			@code{.c}
 *			#include "led.h"
 *
 *			void main(void) {
 *
 *				ledInitialize();
 *				ledTurnOn(LED4);
 *
 *			}
 *			@endcode
 *
 ************************************************
 */
#define ledTurnOn(x)							\
{												\
												\
	/*
	 * nur wenn led aus ((x) == 1)
	 * led togglen (=einschalten)
	 */											\
	if ( (x) ) ledToggle(x);					\
	 	 	 	 	 	 	 	 	 	 	 	\
}

/*!
 ************************************************
 *
 * @brief	Dieses Makro schaltet die
 * 			LED \c x aus.
 *
 * @param	x	LED die ausgeschaltet
 * 			werden soll. Nutzen Sie zwingend
 * 			#LED4, #LED5, ..., #LED15.
 *
 * @note	Nutzen Sie zwingend
 * 			#LED4, #LED5, ..., #LED15 als
 * 			Argument!
 *
 * @pre		Rufen Sie zu Beginn ihres
 * 			Programms einmal #ledInitialize()
 * 			auf. Erst dann funktioniert dieses
 * 			Makro wie erwartet.
 *
 * @par		Beispiel
 *			@code{.c}
 *			#include "led.h"
 *
 *			void main(void) {
 *
 *				ledInitialize();
 *				ledTurnOff(LED4);
 *
 *			}
 *			@endcode
 *
 ************************************************
 */
#define ledTurnOff(x)							\
{												\
												\
	/*
	 * nur wenn led an ((x) == 0)
	 * led togglen (=ausschalten)
	 */											\
	if ( !(x) ) ledToggle(x)					\
												\
}









#endif // ifndef __LED__
