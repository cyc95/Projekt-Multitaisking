/*!
 *******************************************************************************
 *
 * @file			write.c
 *
 * @brief			Implementierung der Funktion
 *					<tt>int _write( int fd, const char * buf, int cnt )</tt>.
 *
 * @details			Diese Datei implementiert die Funktion _write( ) (und
 * 					weitere, benötigte Funktionalitäten), sodass die
 * 					Ausgaben von printf( ), putchar( ) und puts( ) aus stdio.h.
 *					in die serielle Schnittstelle umgeleitet werden.
 *
 * @author			Christopher Zink
 *
 * @date			Juni 2015
 *
 * @version			1.0
 *
 * @see				r01uh0041ej0160_rx63n631.pdf für die Beschreibung der
 * 					Register.
 *
 *******************************************************************************
 */










/*
################################################################################
#                                   Inkluds                                    #
################################################################################
 */

#include "bsp.h"
#include "iodefine.h"










/*
################################################################################
#                             dateiprivate Variablen                           #
################################################################################
 */

/*!
 * Zeigt bei <tt>isEmptyTDR == 1</tt> an,
 * dass das Zeichen übertragen wurde.
 */
static char isEmptyTDR = 0;










/*
################################################################################
#                                  Prototypen                                  #
################################################################################
 */

#if USED_LIB == OPTLIB
int _write( int, const char *, int );
#endif
/*!
 * @cond		doxygen hat probleme mit '__attribute__ ( ( interrupt ) )'
 */
void __attribute__ ( ( interrupt ) ) INT_Excep_SCI2_TXI2( void );
/*!
 * @endcond
 */








/*
################################################################################
#                             Implementierungen                                #
################################################################################
 */

#if USED_LIB == OPTLIB
/*!
 * @brief		Senden auf der seriellen Schnittstelle.
 *
 * @details		Sendet \c cnt Zeichen aus \c buf, beginnend
 * 				mit Index 0, auf der seriellen Schnittstelle.
 *
 * @param		fd		Filedescriptor, ungenutzt.
 * @param		buf		Puffer mit den zu sendenen Zeichen.
 * @param		cnt		Anzahl der zu sendenen Zeichen.
 *
 * @return		Die Anzahl der gesendeten Zeichen.
 */
int _write( int fd, const char * buf, int cnt )
{

	/* Ich konnte beobachten, dass _write( )
	   stets mit cnt == 1 aufgerufen wird.
	   Da ich nicht weiss ob das immer so ist,
	   die for-Schleife. */

	int i;

	/* Den Sendeninterrupt aktivieren. */
	SCI2.SCR.BIT.TIE = 1;
	IEN( SCI2, TXI2 ) = 1;

	for ( i = 0; i < cnt; i++ )
	{

		/* Das zu übertragende Zeichen in
	       in das Transmit-Data-Register (TDR)
	       schreiben. */
		SCI2.TDR = buf[i];

		/* Aufgrund dessen ist das TDR nicht leer. */
		isEmptyTDR = 0;

		/* Warten bis das Zeichen gesendet ist.
		   Die ISR wird dies signalisieren. */
		while ( !isEmptyTDR );

	}

	/* Den Sendeninterrupt deaktivieren. */
	IEN( SCI2, TXI2 ) = 0;
	SCI2.SCR.BIT.TIE = 0;

	/* Die Anzahl der gesendeten Zeichen zurückgeben. */
	return i;

}
#endif

/* -------------------------------------------------------------------------- */

/*!
 * @brief		ISR des 'Senden-Eines-Zeichens-Abgeschlossen-Interrupts'
 * 				des zweiten SCI-Kanals.
 *
 * @note		Rufen Sie diese Funktion niemals aus!
 */
void INT_Excep_SCI2_TXI2( void )
{

	/* _write( ) signalisieren, dass
	   das Zeichen gesendet wurde. */
	isEmptyTDR = 1;

}

/* -------------------------------------------------------------------------- */
