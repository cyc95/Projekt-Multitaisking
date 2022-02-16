/*!
 *******************************************************************************
 *
 * @file		button.h
 *
 * @brief		Makros für die Nutzung der Taster SWITCH 1, SWICTH 2 und
 * 				SWITCH3 des YRDKRX63N.
 *
 * @details		Diese Datei enthält die Definitionen von Makros für die
 * 				Nutzung der drei Taster SWITCH 1, SWITCH 2 und SWITCH 3 des
 * 				YRDKRX63N.
 *
 * @author		Christopher Zink
 *
 *******************************************************************************
 *
 * @mainpage
 *
 * @par			Installation
 * 				Um die Makros #buttonInitialize(), #buttonIsPressed(x)
 * 				und #buttonIsReleased(x) nutzen zu können, müssen Sie
 * 				die Datei button.h in ihr Projektverzeichnis kopieren
 * 				und in ihr Projekt inkludieren.
 *
 * @par			Funktion
 * 				Rufen Sie zunächst das Makro #buttonInitialize() auf.
 * 				Sie haben nun die Möglichkeit mit den Makros
 * 				#buttonIsPressed(x) und #buttonIsReleased(x) den
 * 				Zustand der drei Taster SW1, SW2 und SW3 zu erfragen.
 *
 * @par			Verbindungen
@verbatim
  Name auf dem Board |   Argument an Makros  |     Port      |     Bit
---------------------+-----------------------+---------------+--------------
	   SWITCH 1    	 |        BUTTON1        |       4       |      0
	   SWITCH 2    	 |        BUTTON2        |       4       |      1
	   SWITCH 3    	 |        BUTTON3        |       4       |      4
	 	 SW4     	 |     nicht nutzbar     |       -       |      -
@endverbatim
 *
 * @par			Beispielprogramm
@code{.c}
#define BUTTON_BLOCK 1						// alternativ kann die konstante BUTTON_BLOCK auch in button.h belegt werden
#include "button.h"


void main(void) {

	buttonInitialize();						// init

#if BUTTON_BLOCK

	buttonIsPressed(BUTTON1);				// blockiert bis switch 1 betätigt ist

	// irgend etwas machen

	buttonIsPressed(BUTTON2);				// blockiert bis switch 2 betätigt ist

	// irgend etwas anderes machen

	buttonIsPressed(BUTTON3);				// blockiert bis switch 3 betätigt ist

	// hier dann auch was machen

	buttonIsReleased(BUTTON3);				// blockiert bis switch 3 unbetätigt ist

	// es gibt viel zu tun

	buttonIsReleased(BUTTON2);				// blockiert bis switch 2 unbetätigt ist

	// und wir sind immer noch nicht fertig

	buttonIsReleased(BUTTON1);				// blockiert bis switch 1 unbetätigt ist

	// jetzt haben wir es geschafft

#else

	while ( buttonIsReleased(BUTTON1) );	// blockiert bis switch 1 betätigt ist

	// irgend etwas machen

	while ( buttonIsReleased(BUTTON2) );	// blockiert bis switch 2 betätigt ist

	// irgend etwas anderes machen

	while ( buttonIsReleased(BUTTON3) );	// blockiert bis switch 3 betätigt ist

	// hier dann auch was machen

	while ( buttonIsPressed(BUTTON3) );		// blockiert bis switch 3 unbetätigt ist

	// es gibt viel zu tun

	while ( buttonIsPressed(BUTTON2) );		// blockiert bis switch 2 unbetätigt ist

	// und wir sind immer noch nicht fertig

	while ( buttonIsPressed(BUTTON1) );		// blockiert bis switch 1 unbetätigt ist

	// jetzt haben wir es geschafft

#endif

}
@endcode
 *
 *******************************************************************************
 */










// mehrfachincludeschutz
#ifndef __BUTTON__
#define __BUTTON__










/******************************************************************************************************************************************************************************
 *                                                                              Includes                                                                                      *
 ******************************************************************************************************************************************************************************/

//#include "iodefine.h"	// PORT4
#include "../bsp/iodefine.h"	// PORT4, liegt in /bsp










/******************************************************************************************************************************************************************************
 *                                                                             Konstanten                                                                                     *
 ******************************************************************************************************************************************************************************/

/*!
 * @brief		Als Argument an #buttonIsPressed(x) und #buttonIsReleased(x) gedacht
 */
#define BUTTON1		PORT4.PIDR.BIT.B0

/*!
 * @brief		Als Argument an #buttonIsPressed(x) und #buttonIsReleased(x) gedacht
 */
#define BUTTON2		PORT4.PIDR.BIT.B1

/*!
 * @brief		Als Argument an #buttonIsPressed(x) und #buttonIsReleased(x) gedacht
 */
#define BUTTON3		PORT4.PIDR.BIT.B4










/******************************************************************************************************************************************************************************
 *                                                                             Steuerung                                                                                      *
 ******************************************************************************************************************************************************************************/

/*!
 *
 * @brief		Steuerung des Moduls
 *
 * @details		- BUTTON_BLOCK != 0
 * 					- #buttonIsPressed(x) blockiert
 * 					  bis der Taster \c x betätigt ist
 * 					- #buttonIsReleased(x) blockiert
 * 					  bis der Taster \c x unbetätigt ist
 *				.
 * 				- BUTTON_BLOCK == 0
 * 					- #buttonIsPressed(x) gibt 1 zurück
 * 				      wenn der Taster \c x betätigt
 * 				      und 0 wenn \c x unbetätigt ist
 * 					- #buttonIsReleased(x) gibt 1 zurück
 * 					 wenn der Taster \c x unbetätigt ist
 * 					 und 0 wenn \c x betätigt ist
 * 				.
 *
 * @note		Sie haben die Möglichkeit
 * 				die Konstante #BUTTON_BLOCK
 * 				vor \c #include \c "button.h" mit beispielsweise
 * 				\c #define \c BUTTON_BLOCK \c 1 zu belegen
 *
 */
#ifndef BUTTON_BLOCK
#define BUTTON_BLOCK 1
#endif










/******************************************************************************************************************************************************************************
 *                                                                               Makros                                                                                       *
 ******************************************************************************************************************************************************************************/

/*!
 ********************************************
 *
 * @brief	Initialisieren der den Tastern
 * 			zugeordneten Register.
 *
 * @details	Belegt das Port-Direction-Register
 * 			(PDR) des Ports 4 an den Stellen 2^0,
 * 			2^1 und 2^4 mit 0en, sodass
 * 			die Taster anschließend als
 * 			Eingänge fungieren.
 *
 * @note	Rufen Sie dieses Makro einmal
 * 			zu Beginn ihres Programms auf.
 * 			Erst dann arbeiten die Makros
 * 			#buttonIsPressed(x) und
 * 			#buttonIsReleased(x) wie erwartet.
 *
 ********************************************
 */
#define buttonInitialize()					\
{											\
											\
	/*
	 * das port-direction-register
	 * (PDR) des ports 4 an den
	 * stellen 2^0, 2^1 und 2^4
	 * mit 0en belegen, d.h. die
	 * pins 0, 1 und 4 als eingänge
	 * definieren
	 */										\
	PORT4.PDR.BYTE &= 0xEC;					\
	 	 	 	 	 	 	 	 			\
}

/*!
 ********************************************
 *
 * @brief		Zustand eines Tasters
 * 				abfragen.
 *
 * @param	x	Taster dessen Zustand
 * 				abgefragt werden soll.
 * 				Nutzen Sie zwingend
 * 				#BUTTON1, #BUTTON2 oder
 * 				#BUTTON3.
 *
 * @return		Ist die Konstante
 * 				#BUTTON_BLOCK mit 0 belegt,
 * 				wird 1 zurückgegeben wenn
 * 				\c x betätigt ist und 0 wenn
 * 				\c x unbetätigt ist. Ist
 * 				#BUTTON_BLOCK mit einem
 * 				Wert von 0 verschieden belegt,
 * 				blockiert dieses Makro
 * 				bis \c x betätigt ist und
 * 				gibt nichts zurück.
 *
 * @pre			Rufen Sie vor dem Aufruf
 * 				dieses Makros einmal das
 * 				Makro #buttonInitialize()
 * 				auf.
 *
 * @note		Nutzen Sie zwingend
 * 				#BUTTON1, #BUTTON2
 * 				oder #BUTTON3 als
 * 				Argument.
 *
 *
 ********************************************
 */
#if BUTTON_BLOCK
#define buttonIsPressed(x)					\
{											\
											\
	/*
	 * 'warten' bis ((x) == 0),
	 * d.h. x ist betätigt
	 */										\
	while ( (x) );							\
											\
}
#else
#define buttonIsPressed(x)					\
({											\
											\
	/*
	 * wenn x betätig ist 1 zurückgeben
	 * sonst 0 zurückgeben
	 */										\
	(x) == 0 ? 1 : 0;						\
											\
})
#endif

/*!
 ********************************************
 *
 * @brief		Zustand eines Tasters
 * 				abfragen.
 *
 * @param	x	Taster dessen Zustand
 * 				abgefragt werden soll.
 * 				Nutzen Sie zwingend
 * 				#BUTTON1, #BUTTON2 oder
 * 				#BUTTON3.
 *
 * @return		Ist die Konstante
 * 				#BUTTON_BLOCK mit 0 belegt,
 * 				wird 1 zurückgegeben wenn
 * 				\c x unbetätigt ist und 0 wenn
 * 				\c x betätigt ist. Ist
 * 				#BUTTON_BLOCK mit einem
 * 				Wert von 0 verschieden belegt,
 * 				blockiert dieses Makro
 * 				bis \c x unbetätigt ist und
 * 				gibt nichts zurück.
 *
 * @pre			Rufen Sie vor dem Aufruf
 * 				dieses Makros einmal das
 * 				Makro #buttonInitialize()
 * 				auf.
 *
 * @note		Nutzen Sie zwingend
 * 				#BUTTON1, #BUTTON2
 * 				oder #BUTTON3 als
 * 				Argument.
 *
 *
 ********************************************
 */
#if BUTTON_BLOCK
#define buttonIsReleased(x)					\
{											\
											\
	/*
	 * 'warten' bis ((x) == 1),
	 * d.h. x ist unbetätigt
	 */										\
	while ( !(x) );							\
											\
}
#else
#define buttonIsReleased(x)					\
({											\
											\
	/*
	 * wenn x unbetätigt ist 1 zurückgeben
	 * sonst 0 zurückgeben
	 */										\
	(x) == 1 ? 1 : 0;						\
											\
})
#endif










#endif // ifndef __BUTTON__
