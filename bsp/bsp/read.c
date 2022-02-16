/*!
 *******************************************************************************
 *
 * @file			read.c
 *
 * @brief			Implementierung der Funktion
 *					<tt>int _read( int fd, char * buf, int cnt )</tt>.
 *
 * @details			Diese Datei implementiert die Funktion _read( ) (und
 * 					weitere, benötigte Funktionalitäten), sodass die
 * 					Funktionen scanf( ), getchar( ) und gets( ) aus stdio.h.
 *					von der serielle Schnittstelle lesen.
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
#                                    Inkluds                                   #
################################################################################
 */

#include "bsp.h"
#include "iodefine.h"










/*
################################################################################
#                           dateiprivate Variablen                             #
################################################################################
 */

/*!
 * Zeigt bei <tt>isFullRDR == 1</tt> an, dass
 * ein Zeichen im Receive-Data-Register (RDR)
 * eingetroffen ist.
 */
static char isFullRDR = 0;










/*
################################################################################
#                                   Prototypen                                 #
################################################################################
 */

#if USED_LIB == OPTLIB
int _read( int, char *, int );
#endif
/*!
 * @cond		doxygen hat probleme mit '__attribute__ ( ( interrupt ) )'
 */
void __attribute__ ( ( interrupt ) ) INT_Excep_SCI2_RXI2( void );
/*!
 * @endcond
 */










/*
################################################################################
#                               Implementierungen                              #
################################################################################
 */

#if USED_LIB == OPTLIB
/*!
 * @brief		Empfangen auf der seriellen Schnittstelle.
 *
 * @details		Empfängt \c cnt Zeichen auf der seriellen
 * 				Schnittstelle und schreibt sie bei
 * 				Index 0 beginnend in \c buf.
 *
 * @param		fd		Filedescriptor, ungenutzt.
 * @param		buf		Puffer für die Zeichen.
 * @param		cnt		Anzahl der zu empfangenden Zeichen.
 *
 * @return		Die Anzahl der empfangenden Zeichen.
 */
int _read( int fd, char * buf, int cnt )
{

	int i;

	/* Empfangeninterrupt aktivieren. */
	SCI2.SCR.BIT.RIE = 1;
	IEN( SCI2, RXI2 ) = 1;

	for ( i = 0; i < cnt; i++ )
	{

		/* Warten bis ein Zeichen im
		   Receive-Data-Register (RDR)
		   eingetroffen ist. Die ISR
		   wird das signalisieren. */
		while ( !isFullRDR );

		/* Das empfangende Zeichen auslesen. */
		buf[i] = SCI2.RDR;

		/* Aufgrund dessen das RDR nicht
		   mehr belegt ist. */
		isFullRDR = 0;

	}

	/* Den Empfangsinterrupt deaktivieren. */
	IEN( SCI2, RXI2 ) = 0;
	SCI2.SCR.BIT.RIE = 0;

	/* Die Anzahl der empfangenen Zeichen zurückgeben. */
	return i;

}
#endif

/* -------------------------------------------------------------------------- */

/*!
 * @brief		ISR des 'Ein-Zeichen-Ist-Eingetroffen-Interrupt's
 * 				des zweiten SCI-Kanals.
 *
 * @note		Rufen Sie diese Funktion niemals aus!
 */
void INT_Excep_SCI2_RXI2(void)
{

	/* _read( ) signalisieren, dass
	   ein Zeichen eingetroffen ist
	   und im RDR liegt. */
	isFullRDR = 1;

}

/* -------------------------------------------------------------------------- */
