/*!
 *******************************************************************************
 *
 * @file		exit.c
 *
 * @brief		Implementierung der Funktion
 * 				<tt>void _exit( int err )</tt>.
 *
 * @details		Die Funktion _exit( ) wird sowohl
 * 				in newlib als auch in optlib aus exit( ) herraus aufgerufen.
 * 				Mit der Implementierung von _exit( ) kann exit( ) (stdlib.h)
 * 				verwendet werden.
 *
 * @author		Christopher Zink
 *
 * @date		Juni 2015
 *
 * @version		1.0
 *
 *******************************************************************************
 */

#include "bsp.h"

#if USED_LIB == OPTLIB
/*!
 * Diese Funktion kehrt nicht zurück!
 *
 * @param	err		Fehlernummer. Sie können den Debugger nutzen um
 * 					den aktuellen Wert von \c err zu ermitteln.
 *
 * @note			Rufen Sie diese Funktion niemals auf! Rufen Sie
 * 					stets exit( ) auf.
 */
void _exit( int err )
{

	/* Damit es aus der folgenden (Endlos-)Schleife
	   wirklich kein Entkommen gibt, die Interrupts
	   deaktivieren. Nicht maskierbare Interrupts
	   können dennoch weiter feuern. */
	__asm__ __volatile__ (
		"CLRPSW		I"
	);

	while ( 1 );

}
#endif

/* -------------------------------------------------------------------------- */
