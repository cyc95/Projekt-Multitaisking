/*!
 *******************************************************************************
 *
 * @file			spi.c
 *
 * @brief			C-Quell-Datei des Moduls spi.
 * 					Konstanten und Funktionen für die
 * 					SPI-Kommunikation.
 *
 * @author			Christopher Zink
 *
 * @version			1.0
 *
 * @date			Juni 2015
 *
 * @par				Vorlage
 * 					Als Vorlage für dieses Modul haben die
 * 					Datei r_rspi_rx600.h, r_rspi_rx600_config.h
 * 					und r_rspi_rx600.c von Renesas fungiert.
 *
 *******************************************************************************
 */










/*
################################################################################
#                                  Inkluds                                     #
################################################################################
 */

#include "spi.h"
#include "iodefine.h"










/*
################################################################################
#                            dateiprivate Variablen                            #
################################################################################
 */

/*!
 * In diesem Array wird vermerkt, an welchen Kanälen
 * spiInitializeChannel( ) aufgerufen wurde.
 *
 * - <tt>isInitialized[ i ] == 0</tt>: Der Kanal \c i ist nicht initialisiert
 * - <tt>isInitialized[ j ] != 0</tt>: Der Kanal \c j ist initialisiert
 * .
 */
static int isInitialized[ spiNUM_CHANNELS ] = { 0, 0, 0 };

#if spiUSE_LOCKING

/*!
 * In diesem Array wird vermerkt, welche der
 * SPI-Kanäle (von einem Prozess) gesperrt sind.
 *
 * - <tt>isLocked[ i ] == 0</tt>: Der Kanal \c i ist nicht gesperrt
 * - <tt>isLocked[ j ] != 0</tt>: Der Kanal \c j ist gesperrt
 * .
 */
static int isLocked[ spiNUM_CHANNELS ] = { 0, 0, 0 };

/*!
 * In diesem Array wird die Prozess-ID des sperrenden
 * Prozesses hinterlegt.
 */
static unsigned long processIDs[ spiNUM_CHANNELS ] = { 0, 0, 0 };

/*!
 * Diese Variable stellt ein Semaphore dar, dass
 * <tt>isLocked[]</tt> und <tt>processIDs[]</tt>
 * schützt.
 *
 * - <tt>semaphore == 0</tt>: Semaphore nicht vergeben
 * - <tt>semaphore == 1</tt>: Semaphore vergeben
 * .
 */
static int semaphore = 0;

#endif /* if spiUSE_LOCKING */










/*
################################################################################
#                  Implementierung der exportierten Funktionen                 #
################################################################################
 */

/*!
 * @brief		Initialisierung eines SPI-Kanals.
 *
 * @details		Initialisiert den SPI-Kanal mit dem
 * 				Index \c channel.
 *
 * @param	channel		Index des zu initialisierenden
 * 						SPI-Kanals. Nutzen Sie
 * 						#spiCHANNEL_0, #spiCHANNEL_1
 * 						oder #spiCHANNEL_2.
 *
 * @return		Bei erfolgreicher Initialisierung wird
 * 				#spiERROR_NONE zurückgegeben. Im Falle
 * 				eines Fehlers wird eine negative Zahl
 * 				(siehe spiERROR_*) zurückgegeben.
 */
int spiInitializeChannel( int channel )
{

	/* Gültigkeitsprüfung von channel.
	   Wird, da channel im Folgenden als
	   Array-Index Verwendung findet, als
	   notwendig angesehen. Ist nicht in
	   Fkt. ausgelagert um (einfacher)
	   nach Fehlern differenzieren zu können.
	   Ist zu Beginn jeder Modulfunktion, die
	   von außen aufgerufen werden kann,
	   implementiert. */
	switch ( channel )
	{

		case spiCHANNEL_0:
			break;

		case spiCHANNEL_1:
		case spiCHANNEL_2:
			return spiERROR_UNSUPPORTED_OPERATION;

		default:
			return spiERROR_INVALID_ARGUMENT;

	}

	/* Durch den folgenden Krams muss
	   nicht mehrmals gelaufen werden. */
	if ( isInitialized[ channel ] )
	{
		return spiERROR_NONE;
	}

	if ( channel == spiCHANNEL_0 )
	{

		/* Den Modul-Stop-State
		   von RSPI0 abschalten. */
		SYSTEM.PRCR.WORD = 0xA502;
		MSTP( RSPI0 )    = 0;
		SYSTEM.PRCR.WORD = 0xA500;

		/* Die Interrupts 'Receive',
		   'Transmit' und 'Idle' in
		   der ICU deaktivieren. Der
		   Interruptrequest (IR(.))
		   wird dennoch generiert
		   (und der ist es, der beim
		   Senden Verwendung findet). */
		IEN( RSPI0, SPRI0 ) = 0;
		IEN( RSPI0, SPTI0 ) = 0;
		IEN( RSPI0, SPII0 ) = 0;

		/* Das Schreiben in die
		   Pin-Function-Register
		   freigeben. */
		MPC.PWPR.BIT.B0WI  = 0;
		MPC.PWPR.BIT.PFSWE = 1;

		/* User's Manual: Hardware,
		   Seite 709 - 710. */

		/* PC0 ist SSLA1.
		   SSLA1 ist der Flash-Chip-Select. */
		PORTC.PODR.BIT.B0   = 1;
		PORTC.PDR.BIT.B0    = 1;
		PORTC.PMR.BIT.B0    = 1;
		MPC.PC0PFS.BIT.PSEL = 0xD;

		#ifdef YRDKRX63N_H_SAYS_PC1_IS_AUDIO_OUT_L
		/* PC1 ist SSLA2. */
		PORTC.PODR.BIT.B1   = 1;
		PORTC.PDR.BIT.B1    = 1;
		PORTC.PMR.BIT.B1    = 1;
		MPC.PC1PFS.BIT.PSEL = 0xD;
		#endif

		/* PC2 ist SSLA3.
		   SSLA3 ist der LCD-Chip-Select. */
		PORTC.PODR.BIT.B2   = 1;
		PORTC.PDR.BIT.B2    = 1;
		PORTC.PMR.BIT.B2    = 1;
		MPC.PC2PFS.BIT.PSEL = 0xD;

		/* PC3 hat keine SPI-Funktion
		   ist aber laut E-Plan
		   RESET_IO (high-active). */

		/* PC4 ist SSLA0.
		   SSLA0 ist der MicroSD-Chip-Select. */
		PORTC.PODR.BIT.B4   = 1;
		PORTC.PDR.BIT.B4    = 1;
		PORTC.PMR.BIT.B4    = 1;
		MPC.PC4PFS.BIT.PSEL = 0xD;

		/* PC5 ist RSPCKA. */
		PORTC.PODR.BIT.B5   = 0;
		PORTC.PDR.BIT.B5    = 1;
		PORTC.PMR.BIT.B5    = 1;
		MPC.PC5PFS.BIT.PSEL = 0xD;

		/* PC6 ist MOSIA. */
		PORTC.PODR.BIT.B6   = 0;
		PORTC.PDR.BIT.B6    = 1;
		PORTC.PMR.BIT.B6    = 1;
		MPC.PC6PFS.BIT.PSEL = 0xD;

		/* PC7 ist MISOA. */
		PORTC.PDR.BIT.B7    = 0;
		PORTC.PMR.BIT.B7    = 1;
		MPC.PC7PFS.BIT.PSEL = 0xD;

		/* Laut yrdkrx63n.h ist
		   P51 der LCD-RS.
		   Das stimmt mit dem
		   E-Plan überein. */

		/* Laut yrdkrx63n.h ist
		   PJ3 ist WiFi-Chip-Select.
		   Das stimmt mit dem
		   E-Plan überein. */

		/* Das Schreiben in die
		   Pin-Function-Register
		   sperren. */
		MPC.PWPR.BIT.PFSWE = 0;
		MPC.PWPR.BIT.B0WI = 1;

		/* User's Manual: Hardware,
		   Seite 1537 - 1554. */

		/* Das RSPI-Control-Register (SPCR) belegen. */
		RSPI0.SPCR.BIT.SPMS   = 0;	/* 4-Wire-Method                           */
		RSPI0.SPCR.BIT.TXMD   = 0;	/* Voll-duplex Kommunikation               */
		RSPI0.SPCR.BIT.MODFEN = 0;	/* Keine Mode-Fehler detektieren           */
		RSPI0.SPCR.BIT.MSTR   = 1;	/* Master-Mode                             */
		RSPI0.SPCR.BIT.SPEIE  = 0;	/* Kein 'Error-Interrupt'                  */
		RSPI0.SPCR.BIT.SPTIE  = 1;	/* Den 'Transmit-Interrupt' aktivieren     */
		RSPI0.SPCR.BIT.SPE    = 0;	/* SPI erstmal aus                         */
		RSPI0.SPCR.BIT.SPRIE  = 0;	/* Kein 'Receive-Interrupt'                */

		/* Das Slave-Select-Polarity-Register (SSLP) belegen.
		   SSLA0 - SSLA3 sind low-active. */
		RSPI0.SSLP.BIT.SSL0P = 0;
		RSPI0.SSLP.BIT.SSL1P = 0;
		RSPI0.SSLP.BIT.SSL2P = 0;
		RSPI0.SSLP.BIT.SSL3P = 0;

		/* Das RSPI-Pin-Control-Register (SPPCR) belegen. */
		RSPI0.SPPCR.BIT.SPLP  = 0;	/* Loopback 1 aus  	                                                        */
		RSPI0.SPPCR.BIT.SPLP2 = 0;	/* Loopback 2 aus                                                           */
		RSPI0.SPPCR.BIT.MOIFV = 0;	/* Idle 0 (1 wäre Idle 1)                                                   */
		RSPI0.SPPCR.BIT.MOIFE = 0;	/* wenn nichts zum senden, letztes nochmal senden (1 wäre MOIFV übernehmen) */

		/* Das RSPI-Sequence-Control-Register (SPSCR) belegen. */
		RSPI0.SPSCR.BIT.SPSLN = 0;	/* Jetzt wird jeder Vorgang anhand der Werte in SPCMD0 gesteuert */

		/* Das RSPI-Bit-Rate-Register (SPBR) belegen.
		   Bitrate = PCLKB / ( ( RSPI0.SPBR + 1 ) * 2 ^ ( RSPI0.SPCMD0.BRDV + 1 ) ).
		   Taktfrequenz = PCLKB / 2. */
		RSPI0.SPBR = 0;

		/* Das RSPI-Data-Control-Register (SPDCR) belegen. */
		RSPI0.SPDCR.BIT.SPFC   = 0;	/* 1 Frame in SPDR                     */
		RSPI0.SPDCR.BIT.SPRDTD = 0;	/* SPDR aus dem Empfangspuffer speisen */
		RSPI0.SPDCR.BIT.SPLW   = 1;	/* Auf SPDR in long zugreifen          */

		/* Das RSPI-Control-Register 2 (SPCR2) belgen. */
		RSPI0.SPCR2.BIT.SPPE  = 0;	/* Ohne Parität                 */
		RSPI0.SPCR2.BIT.SPOE  = 0;	/* Gerade Parität               */
		RSPI0.SPCR2.BIT.SPIIE = 0;	/* Kein 'Idle-Interrupt'        */
		RSPI0.SPCR2.BIT.PTE   = 0;	/* Ohne Paritäts-Selbst-Prüfung */

		/* Das RSPI-Command-Register 0 (SPCMD0) belegen. */
		RSPI0.SPCMD0.BIT.CPHA   = 0;	/* Lesen bei gerader, Ändern bei ungerader Flanke                         */
		RSPI0.SPCMD0.BIT.CPOL   = 0;	/* RSPCKA idle-low                                                        */
		RSPI0.SPCMD0.BIT.BRDV   = 0;	/* Basis-Bitrate, siehe Formel weiter oben                                */
		RSPI0.SPCMD0.BIT.SSLA   = 0;	/* Für den Vorgang, der mit SPCMD0 gesteuert wird SSLA0 auf 0             */
		RSPI0.SPCMD0.BIT.SSLKP  = 0;	/* Nach Abschluss SSL wieder auf 1                                        */
		RSPI0.SPCMD0.BIT.SPB    = 4; 	/* 8 Bit Daten                                                            */
		RSPI0.SPCMD0.BIT.LSBF   = 0;	/* MSB als erstes                                                         */
		RSPI0.SPCMD0.BIT.SPNDEN = 0;	/* Nächster Vorgang nach 1 RSPCKA + 2 PCLKB (1 wäre RSPI0.SPND.BIT.SPNDL) */
		RSPI0.SPCMD0.BIT.SLNDEN = 0;	/* 1 RSPCKA als SLL-Negations-Verzögerung (1 wäre RSPI0.SSLND.BIT.SLNDL)  */
		RSPI0.SPCMD0.BIT.SCKDEN = 0;	/* 1 RSPCKA als RSPCKA-Verzögerung (1 wäre RSPI0.SPCKD.BIT.SCKDL)         */


		/* Das RSPI0-Clock-Delay-Register (SPCKD) belegen.
		   8 Takte von RSPCKA zwischen SSL und RSPCKA.
		   Wird nur bei RSPI0.SPCMD0.BIT.SCKDEN == 1
		   angewand. */
		if ( RSPI0.SPCMD0.BIT.SCKDEN )
		{
			RSPI0.SPCKD.BIT.SCKDL = 7;
		}

		/* Das RSPI-Slave-Select-Negation-Delay-Regitser (SSLND) belegen.
		   8 Takte von RSPCKA + 2 Takte von PCLKB
		   als SLL-Negations-Verzögerung. Wird nur
		   bei RSPI0.SPCMD0.BIT.SLNDEN == 1
		   angewand. */
		if ( RSPI0.SPCMD0.BIT.SLNDEN )
		{
			RSPI0.SSLND.BIT.SLNDL = 7;
		}

		/* Das RSPI-Next-Access-Delay-Register (SPND) belegen.
		   8 Takte von RSPCKA + 2 Takte von PCLKB als
		   Zeit zwischen zwei Vorgängen.
		   Wird nur bei RSPI0.SPCMD0.BIT.SPNDEN == 1
		   verwendet. */
		if ( RSPI0.SPCMD0.BIT.SPNDEN )
		{
			RSPI0.SPND.BIT.SPNDL = 7;
		}

		/* SPI-Kommunikation starten. */
		RSPI0.SPCR.BIT.SPE = 1;

	}

	isInitialized[ channel ] = 1;

	return spiERROR_NONE;

}

/* -------------------------------------------------------------------------- */

#if spiUSE_LOCKING
/*!
 * @brief		Deinitialisieren eines SPI-Kanals.
 *
 * @details 	Beschreibt die Register, die spiInitializeChannel( )
 * 				beschrieben hat, mit ihren Initialwerten.
 */
#if !spiSPIN_LOCK
/*!
 * @return		Bei erfolgreichem Verlauf wird #spiERROR_NONE
 * 				zurückgegeben. Im Falle eines Fehlers wird
 * 				eine negative Zahl, die den Fehler beschreibt,
 * 				zurückgegeben (siehe #spiERROR_UNSUPPORTED_OPERATION,
 * 				#spiERROR_INVALID_ARGUMENT, #spiERROR_SEMAPHORE_TAKEN
 * 				oder #spiERROR_CHANNEL_LCOKED).
 */
#else
/*!
 * @return		Bei erfolgreichem Verlauf wird #spiERROR_NONE
 * 				zurückgegeben. Im Falle eines Fehlers wird
 * 				eine negative Zahl, die den Fehler beschreibt,
 * 				zurückgegeben (siehe #spiERROR_UNSUPPORTED_OPERATION,
 * 				#spiERROR_INVALID_ARGUMENT oder #spiERROR_CHANNEL_LCOKED).
 */
#endif /* if !spiSPIN_LOCKING */
int spiFinalizeChannel( int channel, unsigned long processID )
#else
/*!
 * @brief		Deinitialisieren eines SPI-Kanals.
 *
 * @details 	Beschreibt die Register, die spiInitializeChannel( )
 * 				beschrieben hat, mit ihren Initialwerten.
 *
 * @return		Bei erfolgreichem Verlauf wird #spiERROR_NONE
 * 				zurückgegeben. Im Falle eines Fehlers wird
 * 				eine negative Zahl, die den Fehler beschreibt,
 * 				zurückgegeben (siehe #spiERROR_UNSUPPORTED_OPERATION
 * 				oder #spiERROR_INVALID_ARGUMENT).
 */
int spiFinalizeChannel( int channel )
#endif /* if spiUSE_LOCKING */
{

	/* Siehe Kommentare in spiInitializeChannel( ). */

	switch ( channel )
	{

		case spiCHANNEL_0:
			break;

		case spiCHANNEL_1:
		case spiCHANNEL_2:
			return spiERROR_UNSUPPORTED_OPERATION;

		default:
			return spiERROR_INVALID_ARGUMENT;

	}


	if ( !isInitialized[ channel ] )
	{
		return spiERROR_NONE;
	}

	#if spiUSE_LOCKING
	{
		int returnValue;
		if ( ( returnValue = spiCheckChannelLock( channel, processID ) ) != spiERROR_NONE )
		{
			return returnValue;
		}
	}
	#endif

	if ( channel == spiCHANNEL_0 )
	{

		RSPI0.SPCR.BYTE = 0x00;

		RSPI0.SPND.BYTE = 0x00;

		RSPI0.SSLND.BYTE = 0x00;

		RSPI0.SPCKD.BYTE = 0x00;

		RSPI0.SPCMD0.WORD = 0x070D;

		RSPI0.SPCR2.BYTE = 0x00;

		RSPI0.SPDCR.BYTE = 0x00;

		RSPI0.SPBR = 0xFF;

		RSPI0.SPSCR.BYTE = 0x00;

		RSPI0.SPPCR.BYTE = 0x00;

		RSPI0.SSLP.BYTE = 0x00;

		/* PODR, PDR und PMR wäre übertrieben! */

		SYSTEM.PRCR.WORD = 0xA502;
		MSTP( RSPI0 ) = 1;
		SYSTEM.PRCR.WORD = 0xA500;

	}

	isInitialized[ channel ] = 0;

	return spiERROR_NONE;

}

/* -------------------------------------------------------------------------- */

#if spiUSE_LOCKING
/*!
 * @brief		Sperren eines SPI-Kanals.
 *
 * @details		Diese Funktion sperrt den SPI-Kanal mit dem Index \c channel
 * 				für den aufrufenden Prozess/ die aufrufende Task.
 *
 * @param	channel		Index des zu sperrenden Kanals. Nutzen Sie
 * 						#spiCHANNEL_0, #spiCHANNEL_1 oder #spiCHANNEL_2.
 * @param	processID	ID des aufrufenden Prozesses/ der aufrufenden Task.
 */
#if !spiSPIN_LOCK
/*!
 * @return		Wenn die Funktion den Kanal erfolgreich sperren konnte,
 * 				gibt sie #spiERROR_NONE zurück. Tritt beim Sperren ein
 * 				Fehler auf, wird eine negative Zahl zurückgegeben
 * 				(siehe #spiERROR_UNSUPPORTED_OPERATION,
 * 				#spiERROR_INVALID_ARGUMENT,
 * 				#spiERROR_CHANNEL_FINALIZED,
 * 				#spiERROR_SEMAPHORE_TAKEN oder
 * 				#spiERROR_CHANNEL_LOCKED).
 */
#else
/*!
 * @return		Wenn die Funktion den Kanal erfolgreich sperren konnte,
 * 				gibt sie #spiERROR_NONE zurück. Tritt beim Sperren ein
 * 				Fehler auf, wird eine negative Zahl zurückgegeben
 * 				(siehe #spiERROR_UNSUPPORTED_OPERATION,
 * 				#spiERROR_INVALID_ARGUMENT,
 * 				#spiERROR_CHANNEL_FINALIZED oder
 * 				#spiERROR_CHANNEL_LOCKED).
 */
#endif /* if !spiSPIN_LOCK */
int spiLockChannel( int channel, unsigned long processID )
{

	int sema = 1;

	switch ( channel )
	{

		case spiCHANNEL_0:
			break;

		case spiCHANNEL_1:
		case spiCHANNEL_2:
			return spiERROR_UNSUPPORTED_OPERATION;

		default:
			return spiERROR_INVALID_ARGUMENT;

	}

	if ( !isInitialized[ channel ] )
	{
		return spiERROR_CHANNEL_FINALIZED;
	}

	#if spiSPIN_LOCK
	{

		/* Das könnte zu einer
		   Verklemmung auf Grund
		   einer Prioritätsinversion
		   führen, oder nicht!? */
		do
		{
			/* semaphore und sema in einem
			   Maschinenbefehl (XCHG) swapen. */
			__builtin_rx_xchg( &semaphore, &sema );
		} while ( sema == 1 );

	}
	#else
	{

		/* semaphore und sema in einem
		   Maschinenbefehl (XCHG) swapen. */
		__builtin_rx_xchg( &semaphore, &sema );

		if ( sema == 1 )
		{
			return spiERROR_SEMAPHORE_TAKEN;
		}

	}
	#endif

	/* Egal ob Spinlock oder nicht,
	   wer den Quelltext ab hier
	   bearbeitet, der hat den
	   exklusiven Zugriff auf
	   isLocked[] und
	   processIDs[]. Man muss
	   aufpassen, dass man das
	   Semaphore vor dem Verlassen
	   der Fkt. wieder freigibt. */

	if ( !isLocked[ channel ] )
	{

		/* Den Kanal sperren. */
		isLocked[ channel ] = 1;

		/* Und processID als Eigentümer
		   der Sperre merken. */
		processIDs[ channel ] = processID;

		/* Das Semaphore wieder freigeben. */
		sema = 0;
		__builtin_rx_xchg( &semaphore, &sema );

		/* Erfolgreiches Ende. */
		return spiERROR_NONE;

	}

	sema = 0;
	__builtin_rx_xchg( &semaphore, &sema );
	return spiERROR_CHANNEL_LOCKED;

}
#endif /* if spiUSE_LOCKING */

/* -------------------------------------------------------------------------- */

#if spiUSE_LOCKING
/*!
 * @brief		Entsperren eines SPI-Kanals.
 *
 * @details		Diese Funktion entsperrt den SPI-Kanal mit dem
 * 				Index \c channel, wenn der aufrufende Prozess
 * 				der Prozess ist, der die Sperre hält.
 *
 * @param		channel		Index des zu entsperrenden Kanals.
 * @param		processID	ID des aufrufenden Prozesses/
 * 							der aufrufenden Task.
 */
#if !spiSPIN_LOCK
/*!
 * @return		Wenn die Funktion den Kanal erfolgreich entsperren konnte,
 * 				gibt sie #spiERROR_NONE zurück. Tritt beim Sperren ein
 * 				Fehler auf, wird eine negative Zahl zurückgegeben
 * 				(siehe #spiERROR_UNSUPPORTED_OPERATION,
 * 				#spiERROR_INVALID_ARGUMENT,
 * 				#spiERROR_CHANNEL_FINALIZED,
 * 				#spiERROR_SEMAPHORE_TAKEN oder
 * 				#spiERROR_CHANNEL_LOCKED).
 */
#else
/*!
 * @return		Wenn die Funktion den Kanal erfolgreich entsperren konnte,
 * 				gibt sie #spiERROR_NONE zurück. Tritt beim Sperren ein
 * 				Fehler auf, wird eine negative Zahl zurückgegeben
 * 				(siehe #spiERROR_UNSUPPORTED_OPERATION,
 * 				#spiERROR_INVALID_ARGUMENT,
 * 				#spiERROR_CHANNEL_FINALIZED oder
 * 				#spiERROR_CHANNEL_LOCKED).
 */
#endif /* if !spiSPIN_LOCK */
int spiUnlockChannel( int channel, unsigned long processID )
{

	/* Siehe Kommentare in spiLockChannel( ). */

	int sema = 1;

	switch ( channel )
	{

		case spiCHANNEL_0:
			break;

		case spiCHANNEL_1:
		case spiCHANNEL_2:
			return spiERROR_UNSUPPORTED_OPERATION;

		default:
			return spiERROR_INVALID_ARGUMENT;

	}

	if ( !isInitialized[ channel ] )
	{
		return spiERROR_CHANNEL_FINALIZED;
	}

	#if spiSPIN_LOCK
	{

		do
		{
			__builtin_rx_xchg( &semaphore, &sema );
		} while ( sema == 1 );

	}
	#else
	{

		__builtin_rx_xchg( &semaphore, &sema );

		if ( sema == 1 )
		{
			return spiERROR_SEMAPHORE_TAKEN;
		}

	}
	#endif

	if ( !isLocked[ channel ] )
	{
		sema = 0;
		__builtin_rx_xchg( &semaphore, &sema );
		return spiERROR_NONE;
	}

	if ( processIDs[ channel ] == processID )
	{
		isLocked[ channel ] = 0;
		processIDs[ channel ] = 0;
		sema = 0;
		__builtin_rx_xchg( &semaphore, &sema );
		return spiERROR_NONE;
	}

	sema = 0;
	__builtin_rx_xchg( &semaphore, &sema );
	return spiERROR_CHANNEL_LOCKED;

}
#endif /* if spiUSE_LOCKING */

/* -------------------------------------------------------------------------- */

#if spiUSE_LOCKING
/*!
 * @brief		Sperrabfrage eines SPI-Kanals.
 *
 * @details		Erfragt ob der SPI-Kanal \c channel gesperrt ist
 * 				und der aufrufende Prozess/ die aufrufende Task siese Sperre hält.
 *
 * @param	channel		Index des abzufragenden Kanals.
 * @param	processID	ID des aufrufenden Prozesses/ der aufrufenden Task
 */
#if !spiSPIN_LOCK
/*!
 * @return		Wenn der Kanal nicht gesperrt ist oder der Kanal gesperrt
 * 				ist und der aufrufenden Prozess diese Sperre hält, gibt
 * 				die Funktion #spiERROR_NONE zurück. Sonst wird eine
 * 				negative Zahl zurückgegeben, die den Fehler beschreibt
 * 				(siehe #spiERROR_UNSUPPORTED_OPERATION,
 * 				#spiERROR_INVALID_ARGUMENT,
 * 				#spiERROR_CHANNEL_FINALIZED,
 * 				#spiERROR_SEMAPHORE_TAKEN oder
 * 				#spiERROR_CHANNEL_LOCKED).
 */
#else
/*!
 * @return		Wenn der Kanal nicht gesperrt ist oder der Kanal gesperrt
 * 				ist und der aufrufenden Prozess diese Sperre hält, gibt
 * 				die Funktion #spiERROR_NONE zurück. Sonst wird eine
 * 				negative Zahl zurückgegeben, die den Fehler beschreibt
 * 				(siehe #spiERROR_UNSUPPORTED_OPERATION,
 * 				#spiERROR_INVALID_ARGUMENT,
 * 				#spiERROR_CHANNEL_FINALIZED oder
 * 				#spiERROR_CHANNEL_LOCKED).
 */
#endif /* if !spiSPIN_LOCK */
int spiCheckChannelLock( int channel, unsigned long processID )
{

	/* Siehe Kommentare in spiLockChannel( ). */

	int sema = 1;

	switch ( channel )
	{

		case spiCHANNEL_0:
			break;

		case spiCHANNEL_1:
		case spiCHANNEL_2:
			return spiERROR_UNSUPPORTED_OPERATION;

		default:
			return spiERROR_INVALID_ARGUMENT;

	}

	if ( !isInitialized[ channel ] )
	{
		return spiERROR_CHANNEL_FINALIZED;
	}

	#if spiSPIN_LOCK
	{

		do
		{
			__builtin_rx_xchg( &semaphore, &sema );
		} while ( sema == 1 );

	}
	#else
	{

		__builtin_rx_xchg( &semaphore, &sema );

		if ( sema == 1 )
		{
			return spiERROR_SEMAPHORE_TAKEN;
		}

	}
	#endif

	if ( !isLocked[ channel ] )
	{
		sema = 0;
		__builtin_rx_xchg( &semaphore, &sema );
		return spiERROR_NONE;
	}

	if ( processIDs[ channel ] == processID )
	{
		sema = 0;
		__builtin_rx_xchg( &semaphore, &sema );
		return spiERROR_NONE;
	}

	sema = 0;
	__builtin_rx_xchg( &semaphore, &sema );
	return spiERROR_CHANNEL_LOCKED;

}
#endif /* if spiUSE_LOCKING */

/* -------------------------------------------------------------------------- */

/*!
 * @brief		Anwahl eines Slaves.
 *
 * @details		Wählt einen der 4 möglichen Slaves LCD, WiFi, Flash oder
 * 				MicroSD an indem der entsprechende Chip-Select auf 0
 * 				gesetzt wird.
 *
 * @note		Aktuell ist nur die Anwahl des LCD implementiert!
 *
 * @param		channel		SPI-Kanal
 * @param		slave		Hiermit geben Sie an welcher Slave angewählt
 * 							werden soll. Nutzen Sie #spiSELECT_LCD,
 * 							#spiSELECT_FLASH, #spiSELECT_MICROSD
 * 							oder #spiSELECT_WIFI.
 */
#if spiUSE_LOCKING
#if !spiSPIN_LOCK
/*!
 * @return		Bei erfolgreicher Anwahl des vorgegebenen Slaves
 * 				wird #spiERROR_NONE zurückgegeben. Beim Auftreten
 * 				eines Fehlers wird eine negative Zahl, die den
 * 				Fehler beschreibt, zurückgegeben (siehe
 * 				#spiERROR_UNSUPPORTED_OPERATION,
 * 				#spiERROR_INVALID_ARGUMENT,
 * 				#spiERROR_SEMAPHORE_TAKEN,
 * 				#spiERROR_CHANNEL_LOCKED oder
 * 				#spiERROR_CHANNEL_FINALIZED).
 */
#else
/*!
 * @return		Bei erfolgreicher Anwahl des vorgegebenen Slaves
 * 				wird #spiERROR_NONE zurückgegeben. Beim Auftreten
 * 				eines Fehlers wird eine negative Zahl, die den
 * 				Fehler beschreibt, zurückgegeben (siehe
 * 				#spiERROR_UNSUPPORTED_OPERATION,
 * 				#spiERROR_INVALID_ARGUMENT,
 * 				#spiERROR_CHANNEL_LOCKED oder
 * 				#spiERROR_CHANNEL_FINALIZED).
 */
#endif /* if !spiSPIN_LOCK */
int spiSelectSlave( int channel, int slave, unsigned long processID )
#else
/*!
 * @return		Bei erfolgreicher Anwahl des vorgegebenen Slaves
 * 				wird #spiERROR_NONE zurückgegeben. Beim Auftreten
 * 				eines Fehlers wird eine negative Zahl, die den
 * 				Fehler beschreibt, zurückgegeben (siehe
 * 				#spiERROR_UNSUPPORTED_OPERATION,
 * 				#spiERROR_INVALID_ARGUMENT oder
 * 				#spiERROR_CHANNEL_FINALIZED).
 */
int spiSelectSlave( int channel, int slave )
#endif /* if spiUSE_LOCKING */
{

	switch ( channel )
	{

		case spiCHANNEL_0:
			break;

		case spiCHANNEL_1:
		case spiCHANNEL_2:
			return spiERROR_UNSUPPORTED_OPERATION;

		default:
			return spiERROR_INVALID_ARGUMENT;

	}

	switch ( slave )
	{

		case spiSELECT_LCD:
			break;

		case spiSELECT_FLASH:
		case spiSELECT_MICROSD:
		case spiSELECT_WIFI:
			return spiERROR_UNSUPPORTED_OPERATION;

		default:
			return spiERROR_INVALID_ARGUMENT;

	}

	if ( !isInitialized[ channel ] )
	{
		return spiERROR_CHANNEL_FINALIZED;
	}

	#if spiUSE_LOCKING
	{
		int returnValue;
		if ( ( returnValue = spiCheckChannelLock( channel, processID ) ) != spiERROR_NONE )
		{
			return returnValue;
		}
	}
	#endif

	switch ( channel )
	{

        case spiCHANNEL_0:
            if ( slave == spiSELECT_LCD )
            {
            	RSPI0.SPCMD0.BIT.SSLA = 3;
            }
            break;

        default:
            break;

	}

	return spiERROR_NONE;

}

/* -------------------------------------------------------------------------- */

#if spiUSE_LOCKING
/*!
 * @brief		Abwahl eines Slaves.
 *
 * @details		Wählt einen der 4 möglichen Slaves LCD, WiFi, Flash oder
 * 				MicroSD ab indem der entsprechende Chip-Select auf 1
 * 				gesetzt wird. Die wird nur ausgeführt, wenn der
 * 				SPI-Kanal \c channel nicht gesperrt ist oder der
 * 				aufrufende Prozess/ die aufrufende Task diese
 * 				Sperre hält.
 *
 * @note		Aktuell ist nur die Abwahl des LCD implementiert!
 *
 * @param		channel		SPI-Kanal
 * @param		slave		Hiermit geben Sie an welcher Slave abgewählt
 * 							werden soll. Nutzen Sie #spiSELECT_LCD,
 * 							#spiSELECT_FLASH, #spiSELECT_MICROSD
 * 							oder #spiSELECT_WIFI.
 * @param		processID	ID des aufrufenden Prozesses/ der aufrufenden
 * 							Task.
 */
#if !spiSPIN_LOCK
/*!
 * @return		Bei erfolgreicher Abwahl des vorgegebenen Slaves
 * 				wird #spiERROR_NONE zurückgegeben. Beim Auftreten
 * 				eines Fehlers wird eine negative Zahl, die den
 * 				Fehler beschreibt, zurückgegeben (siehe
 * 				#spiERROR_UNSUPPORTED_OPERATION,
 * 				#spiERROR_INVALID_ARGUMENT,
 * 				#spiERROR_SEMAPHORE_TAKEN,
 * 				#spiERROR_CHANNEL_LOCKED oder
 * 				#spiERROR_CHANNEL_FINALIZED).
 */
#else
/*!
 * @return		Bei erfolgreicher Abwahl des vorgegebenen Slaves
 * 				wird #spiERROR_NONE zurückgegeben. Beim Auftreten
 * 				eines Fehlers wird eine negative Zahl, die den
 * 				Fehler beschreibt, zurückgegeben (siehe
 * 				#spiERROR_UNSUPPORTED_OPERATION,
 * 				#spiERROR_INVALID_ARGUMENT,
 * 				#spiERROR_CHANNEL_LOCKED oder
 * 				#spiERROR_CHANNEL_FINALIZED).
 */
#endif /* if !spiSPIN_LOCK */
int spiDeselectSlave( int channel, int slave, unsigned long processID )
#else
/*!
 * @brief		Abwahl eines Slaves.
 *
 * @details		Wählt einen der 4 möglichen Slaves LCD, WiFi, Flash oder
 * 				MicroSD ab indem der entsprechende Chip-Select auf 1
 * 				gesetzt wird.
 *
 * @note		Aktuell ist nur die Abwahl des LCD implementiert!
 *
 * @param		channel		SPI-Kanal
 * @param		slave		Hiermit geben Sie an welcher Slave abgewählt
 * 							werden soll. Nutzen Sie #spiSELECT_LCD,
 * 							#spiSELECT_FLASH, #spiSELECT_MICROSD
 * 							oder #spiSELECT_WIFI.
 *
 * @return		Bei erfolgreicher Abwahl des vorgegebenen Slaves
 * 				wird #spiERROR_NONE zurückgegeben. Beim Auftreten
 * 				eines Fehlers wird eine negative Zahl, die den
 * 				Fehler beschreibt, zurückgegeben (siehe
 * 				#spiERROR_UNSUPPORTED_OPERATION,
 * 				#spiERROR_INVALID_ARGUMENT oder
 * 				#spiERROR_CHANNEL_FINALIZED).
 */
int spiDeselectSlave( int channel, int slave )
#endif /* if spiUSE_LOCKING */
{

	switch ( channel )
	{

		case spiCHANNEL_0:
			break;

		case spiCHANNEL_1:
		case spiCHANNEL_2:
			return spiERROR_UNSUPPORTED_OPERATION;

		default:
			return spiERROR_INVALID_ARGUMENT;

	}

	switch ( slave )
	{

		case spiSELECT_LCD:
			break;

		case spiSELECT_FLASH:
		case spiSELECT_MICROSD:
		case spiSELECT_WIFI:
			return spiERROR_UNSUPPORTED_OPERATION;

		default:
			return spiERROR_INVALID_ARGUMENT;

	}

	if ( !isInitialized[ channel ] )
	{
		return spiERROR_CHANNEL_FINALIZED;
	}

	#if spiUSE_LOCKING
	{
		int returnValue;
		if ( ( returnValue = spiCheckChannelLock( channel, processID ) ) != spiERROR_NONE )
		{
			return returnValue;
		}
	}
	#endif

	switch ( channel )
	{

        case spiCHANNEL_0:
            if ( slave == spiSELECT_LCD )
            {
            	RSPI0.SPCMD0.BIT.SSLA = 0;
            }
            break;

        default:
            break;

	}

	return spiERROR_NONE;

}

/* -------------------------------------------------------------------------- */

#if spiUSE_LOCKING
/*!
 * @brief		Senden eines Zeichens auf einem SPI-Kanal.
 *
 * @details		Diese Funktion sendet das Zeichen \c byte auf dem
 * 				SPI-Kanal \c channel, wenn der aufrufende Prozess/
 * 				die aufrufende Task den Kanal für sich gesperrt hat
 * 				oder der Kanal nicht gesperrt ist.
 *
 * @param	channel		Kanal, auf dem gesendet werden soll
 * @param	byte		Zeichen, das gesendet werden soll
 * @param	processID	ID des aufrufenden Prozesses/ der aufrufenden Task
 */
#if !spiSPIN_LOCK
/*!
 * @return		Wenn das Senden erfolgreich verlaufen ist, wird
 * 				#spiERROR_NONE zurückgegeben. Tritt beim Senden ein
 * 				Fehler auf, wird eine negative Zahl, die den
 * 				Fehler beschreibt, zurückgegeben (siehe
 * 				#spiERROR_UNSUPPORTED_OPERATION,
 * 				#spiERROR_INVALID_ARGUMENT,
 * 				#spiERROR_CHANNEL_FINALIZED,
 * 				#spiERROR_SEMAPHORE_TAKEN oder
 * 				#spiERROR_CHANNEL_LOCKED).
 */
#else
/*!
 * @return		Wenn das Senden erfolgreich verlaufen ist, wird
 * 				#spiERROR_NONE zurückgegeben. Tritt beim Senden ein
 * 				Fehler auf, wird eine negative Zahl, die den
 * 				Fehler beschreibt, zurückgegeben (siehe
 * 				#spiERROR_UNSUPPORTED_OPERATION,
 * 				#spiERROR_INVALID_ARGUMENT,
 * 				#spiERROR_CHANNEL_FINALIZED oder
 * 				#spiERROR_CHANNEL_LOCKED).
 */
#endif /* if !spiSPIN_LOCK */
int spiTransmitByte( int channel, unsigned char byte, unsigned long processID )
#else
/*!
 * @brief		Senden eines Zeichens auf einem SPI-Kanal.
 *
 * @details		Diese Funktion sendet das Zeichen \c byte auf dem
 * 				SPI-Kanal \c channel.
 *
 * @param	channel		Kanal, auf dem gesendet werden soll
 * @param	byte		Zeichen, das gesendet werden soll
 *
 * @return		Wenn das Senden erfolgreich verlaufen ist, wird
 * 				#spiERROR_NONE zurückgegeben. Tritt beim Senden ein
 * 				Fehler auf, so wird eine negative Zahl, die den
 * 				Fehler beschreibt, zurückgegeben (siehe
 * 				#spiERROR_UNSUPPORTED_OPERATION,
 * 				#spiERROR_INVALID_ARGUMENT oder
 * 				#spiERROR_CHANNEL_FINALIZED).
 */
int spiTransmitByte( int channel, unsigned char byte )
#endif /* if spiUSE_LOCKING */
{

	switch ( channel )
	{

		case spiCHANNEL_0:
			break;

		case spiCHANNEL_1:
		case spiCHANNEL_2:
			return spiERROR_UNSUPPORTED_OPERATION;

		default:
			return spiERROR_INVALID_ARGUMENT;

	}

	if ( !isInitialized[ channel ] )
	{
		return spiERROR_CHANNEL_FINALIZED;
	}

	#if spiUSE_LOCKING
	{
		int returnValue;
		if ( ( returnValue = spiCheckChannelLock( channel, processID ) ) != spiERROR_NONE )
		{
			return returnValue;
		}
	}
	#endif

	if ( channel == spiCHANNEL_0 )
	{

		/* Warten solange RSPI0 im 'Transfer-State' ist. */
		while ( RSPI0.SPSR.BIT.IDLNF );

		/* Das zu übertragende Zeichen in
		   das RSPI-Data-Register schreiben. */
		RSPI0.SPDR.LONG = byte;

		/* Auf den 'Transmit-Interrupt-Request'
		   warten (transmit buffer empty). */
		while ( IR( RSPI0, SPTI0 ) == 0 );

		/* Den ausstehenden 'Transmit-Interrupt-Request'
		   löschen. */
		IR( RSPI0, SPTI0 ) = 0;

	}

	return spiERROR_NONE;

}
