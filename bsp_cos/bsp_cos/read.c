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
#include "cos_types.h"


#define Use_FGB_Modification 1


#if Use_FGB_Modification

/***********************************************************************
 * FGB: Empfangspuffer wird eingerichtet, die ISR schreibt in den Puffer,
 * read liest daraus.
 */

#define RX_BUFFER_LENGTH 32 /*! receiver buffer length, see RX-ISR  */

/**********************************************************************/
/*                   private modul data                               */
/**********************************************************************/

static volatile uint8_t rx_fifo[RX_BUFFER_LENGTH] = {0};  /*! receiver buffer */
static volatile uint8_t rx_rd_index = 0; /*! read index for receiver buffer */
static volatile uint8_t rx_wr_index = 0; /*! write index for receiver buffer */
static volatile uint8_t rx_fifo_used_bytes =0; /*! number of bytes in fifo */



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


/*!
 **********************************************************************
 * @par Description:
 *   Read a byte from receiver fifo. The receiver fifo is filled by the
 *   receiver ISR.
 *
 * @see
 * @arg
 *
 *
 * @param  data  - IN/OUT, pointer to variable for received byte
 *
 * @retval       1 for: one byte read, 0 for: nothing read
 *
 ************************************************************************/
static int8_t _readRXBuffer(uint8_t *data)
{
	if(rx_fifo_used_bytes>0)
	{	//get a single byte from fifo and return
		*data = rx_fifo[rx_rd_index];
		rx_rd_index = (rx_rd_index + 1) %  RX_BUFFER_LENGTH; // circular fifo
		rx_fifo_used_bytes--;
		return 1;
	}
	else return 0;
}


/*
################################################################################
#                               Implementierungen                              #
################################################################################
 */

#if USED_LIB == OPTLIB
/*!
 * @brief		Empfangen auf der seriellen Schnittstelle.
 *
 * @details		Empfängt blockierend \c cnt Zeichen auf der seriellen
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
	int8_t anz;
	uint8_t data_byte;



	for ( i = 0; i < cnt; i++ )
	{
		/* Warten bis ein Zeichen im
		   Receive-Data-Register (RDR)
		   eingetroffen ist. Die ISR
		   wird das signalisieren. */
		do
			anz = _readRXBuffer(&data_byte);
		while (anz==0 );

		/* Das empfangende Zeichen auslesen. */
		buf[i] = data_byte;
	}

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
	uint8_t x;

	/* Den Empfangsinterrupt deaktivieren. */
	IEN( SCI2, RXI2 ) = 0;
	SCI2.SCR.BIT.RIE = 0;

	/* Das empfangende Zeichen auslesen, oder wegwerfen, wenn voll */
	x = SCI2.RDR;
	if(rx_fifo_used_bytes<RX_BUFFER_LENGTH)  // fifo not full...
	{	//put a single byte to fifo and return
		rx_fifo[rx_wr_index] = x;
		rx_wr_index = (rx_wr_index + 1) %  RX_BUFFER_LENGTH; // circular fifo
		rx_fifo_used_bytes++;
	}
	/* Empfangeninterrupt aktivieren. */
	SCI2.SCR.BIT.RIE = 1;
	IEN( SCI2, RXI2 ) = 1;

}


/* -------------------------------------------------------------------------- */

/* by FGB */
/*!
 * @brief		Abfrage der seriellen Schnittstelle (Empfang).
 *
 * @details		Empfängt ein Zeichen auf der seriellen
 * 				Schnittstelle gibt den ASCII-Code zurück, oder -1 falls
 * 				kein Zeichen vorhanden. Die Funktion blockert nicht.
 *
 *
 * @return		ASCII-Code des Zeichens oder -1 falls kein Zeichen vorhanden.
 */
int16_t _pollSerialInterface(void)
{
	int8_t anz;
	uint8_t data;

	anz =  _readRXBuffer(&data);
	if(anz==0) return -1;

	/* Das empfangene Zeichen zurückgeben. */
	return data & 0x00FF;

}

/* by FGB */
/*!
 * @brief		Empfangs-Interrupt der seriellen Schnittstelle aktivieren
 *
 * @details		Aktiviert den Empfangs-Interrupt der seriellen Schnittstelle. Diese
 *              Funktion wird in serInit() in der Datei cos_ser.c aufgerufen, um
 *              die Empfangs-ISR zu aktivieren.
 *
 *
 * @return		ASCII-Code des Zeichens oder -1 falls kein Zeichen vorhanden.
 */
void _initSerialInterface_RX_Interrupt(void)
{
	/* Empfangeninterrupt aktivieren. */
		SCI2.SCR.BIT.RIE = 1;
		IEN( SCI2, RXI2 ) = 1;
}


#else

/**************************************************
 * // Original by C. Zink
***************************************************/
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


#endif


