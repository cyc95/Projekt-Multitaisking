/*!
 *******************************************************************************
 *
 * @file			spi.h
 *
 * @brief			C-Header-Datei des Moduls spi.
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










/* Mehrfachinkludschutz */
#ifndef __SPI__
#define __SPI__










/*
################################################################################
#                                Modulsteuerung                                #
################################################################################
 */

/*!
 * Mit dem Ersatztext dieser
 * Konstanten legen Sie fest,
 * ob das Locking genutzt
 * werden soll oder nicht.
 *
 * - <tt>== 0</tt>: kein Locking
 * - <tt>!= 0</tt>: Locking
 * .
 *
 * Bei aktivem Locking stehen
 * Ihnen die Funktionen
 * spiLockChannel( ),
 * spiUnlockChannel( )
 * und spiCheckChannelLock( )
 * zur Verfügung. Außerdem
 * verlangen die übrigen
 * Modulfunktionen nun
 * eine Prozess-ID.
 */
#define spiUSE_LOCKING						1

#if spiUSE_LOCKING
/*!
 * Mit dieser Konstanten legen Sie
 * bei aktivem Locking fest, ob
 * der Lock als Spinlock ausgeführt
 * sein soll oder nicht.
 *
 * - <tt>== 0</tt>: kein Spinlock
 * - <tt>!= 0</tt>: Spinlock
 * .
 *
 * @note	Es wird angenommen, dass
 * 			es beim Spinlock zu
 * 			Verklemmungen auf Grund
 * 			einer Prioritätsinversion
 * 			kommt.
 */
#define spiSPIN_LOCK						0
#endif /* if spiUSE_LOCKING */










/*
################################################################################
#                                   Konstanten                                 #
################################################################################
 */

/* Wichtig
   =======

   ()	spiCHANNEL_0, spiCHANNEL_1 und spiCHANNEL_2
   	    nicht ändern, da sie in Modulfunktionen als
   	   	Arrayindizes Verwendung finden! */

/*!
 * Die Anzahl der zur Verfügung stehenden
 * SPI-Kanäle.
 */
#define spiNUM_CHANNELS						3

/*!
 * Als Argument an Modulfunktionen vorgesehen,
 * deren Signatur <tt>int channel</tt>
 * enthält.
 */
#define spiCHANNEL_0						0

/*!
 * Als Argument an Modulfunktionen vorgesehen,
 * deren Signatur <tt>int channel</tt>
 * enthält.
 */
#define spiCHANNEL_1						1

/*!
 * Als Argument an Modulfunktionen vorgesehen,
 * deren Signatur <tt>int channel</tt>
 * enthält.
 */
#define spiCHANNEL_2						2

/*!
 * Als Argument an Modulfunktionen vorgesehen,
 * deren Signatur <tt>int slave</tt>
 * enthält.
 */
#define spiSELECT_LCD						4

/*!
 * Als Argument an Modulfunktionen vorgesehen,
 * deren Signatur <tt>int slave</tt>
 * enthält.
 */
#define spiSELECT_WIFI						5

/*!
 * Als Argument an Modulfunktionen vorgesehen,
 * deren Signatur <tt>int slave</tt>
 * enthält.
 */
#define spiSELECT_MICROSD					6

/*!
 * Als Argument an Modulfunktionen vorgesehen,
 * deren Signatur <tt>int slave</tt>
 * enthält.
 */
#define spiSELECT_FLASH						7

/*!
 * Dieser Wert wird von einer Modulfunktion
 * zurückgegeben, wenn Sie ihre Aufgabe
 * erfolgreich ausgeführt hat.
 */
#define spiERROR_NONE						0

/*!
 * Dieser Wert wird von einer Modulfunktion
 * zurückgegeben, wenn Sie mit einem
 * ungültigen Argument aufgerufen wird
 * (bspw. <tt>channel == -1 </tt>).
 */
#define spiERROR_INVALID_ARGUMENT			-1

/*!
 * Diesen Wert gibt eine Modulfunktion
 * zurück, wenn Sie auf nicht
 * implementierte Funktionalitäten läuft.
 */
#define spiERROR_UNSUPPORTED_OPERATION		-2

#if spiUSE_LOCKING
/*!
 * Dieser Wert wird von einer Modulfunktion
 * zurückgegeben, die keinen Zugriff
 * auf den Kanal erlangen konnte, da dieser
 * durch einen anderen Prozess/ eine andere Task
 * gesperrt ist.
 */
#define spiERROR_CHANNEL_LOCKED				-3
#if !spiSPIN_LOCK
/*!
 * Diesen Wert gibt eine Modulfunktion
 * zurück, wenn Sie keinen exklusiven
 * Zugriff auf durch Semaphore geschützte
 * Resourcen bekommen hat.
 *
 * @note	Rufen Sie die Funktion
 * 			anschließend erneut auf!
 */
#define spiERROR_SEMAPHORE_TAKEN			-5
#endif /* if !spiSPIN_LOCK */
#endif /* if spiUSE_LOCKING */

/*!
 * Dieser Wert wird von einer Modulfunktion
 * zurückgegeben, die an einem nicht
 * initialisierten Kanal aufgerufen wurde.
 */
#define spiERROR_CHANNEL_FINALIZED			-4










/*
################################################################################
#                                   Konstanten                                 #
################################################################################
 */

/* Diese Funktionen sind alle in spi.c
   implementiert und kommentiert/dokumentiert. */

#if spiUSE_LOCKING
int spiInitializeChannel( int );
int spiFinalizeChannel( int, unsigned long );
int spiLockChannel( int, unsigned long );
int spiUnlockChannel( int, unsigned long );
int spiCheckChannelLock( int, unsigned long );
int spiSelectSlave( int, int, unsigned long );
int spiDeselectSlave( int, int, unsigned long );
int spiTransmitByte( int, unsigned char, unsigned long );
#else
int spiInitializeChannel( int );
int spiFinalizeChannel( int );
int spiSelectSlave( int, int );
int spiDeselectSlave( int, int );
int spiTransmitByte( int, unsigned char );
#endif /* if spiUSE_LOCKING */










#endif /* ifndef __SPI__ */
