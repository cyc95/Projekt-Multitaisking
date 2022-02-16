/*!
 *******************************************************************************
 *
 * @file			bsp.h
 *
 * @brief			Steuerung des Board-Support-Package (BSP).
 *
 * @details			In dieser Datei können Sie durch Änderung der Konstanten
 * 					Einstellungen am BSP vornehmen.
 *
 * @author			Christopher Zink
 *
 * @date			Juni 2015
 *
 * @version			1.0
 *
 *******************************************************************************
 *
 * @mainpage		Dokumentation des Board-Support-Package (BSP)
 *
 * @par				Installationsanleitung
 * 					<ul>
 * 					<li> Legen Sie zunächst unter e² Studio ein
 * 					     neues Projekt an. Gehen Sie hierfür wie im
 * 					     Folgenden beschrieben vor.
 * 							<ol>
 * 							<li> Navigieren Sie mit der Maus zu
 * 						         File -> New -> C Projekt (siehe Abbildung 1).
 * 					  	         @image html new_project.jpg "Abbildung 1: Anlegen eines neuen Projektes"
 * 							<li> Geben Sie im Feld <i>Project name</i>
 * 								 den Namen des Projektes ein. Wählen Sie
 * 								 unter <i>Project type</i> den Eintrag
 * 								 <i>Executable (Renesas) Sample Project</i>
 * 					  	   		 und unter <i>Toolchains</i> den Eintrag
 * 					  	   		 <i>KPIT GNURX-ELF Toolchain</i> aus
 * 					  	   		 (siehe Abbildung 2). Betätigen Sie im
 * 					  	   		 Anschluss die Schaltfläche <i>Next</i>.
 * 					  	   		 @image html c_project.jpg "Abbildung 2: Projekteinstellungen"
 * 							<li> Wählen Sie nun unter <i>Toolchain Version</i>
 * 						   		 die auf ihrem Rechner installierte Version
 * 						   		 der KPIT GNURX Toolchain aus. Wählen Sie
 * 						   		 unter <i>Debug Hardware</i> den Eintrag
 * 						   		 <i>Segger JLink</i> und unter
 * 						   		 <i>Data endian</i> den Eintrag
 * 						   		 <i>Little-endian data</i> aus. Wählen Sie
 * 						   		 unter <i>Select Traget</i> den Eintrag
 * 						   		 <i>R5F563NBDxFP</i> (siehe Abbildung 3).
 * 						   		 Betätigen Sie abschließend die Schaltfläche
 * 						   		 <i>Next</i>.
 * 					  	   		 @image html target_specifics.jpg "Abbildung 3: Auswahl der Hardware"
 * 							<li> Im folgenden Fenster (siehe Abbildung 4)
 * 								 betätigen Sie einfach die Schaltfläche
 * 								 <i>Next</i>, da für den RX63NB kein
 * 								 Code-Generator verfügbar ist.
 * 					  	   		 @image html code_generator.jpg "Abbildung 4: Code-Generator"
 * 							<li> Lassen Sie im Fenster
 * 								 <i>Select Additional CPU Options</i>
 * 						   		 alle Kontrollfelder abgewählt und betätigen
 * 						   		 Sie dann die Schaltfläche <i>Next</i>
 * 						   		 (siehe Abbildung 5).
 * 					  	   		 @image html cpu_options.jpg "Abbildung 5: Zusätzliche CPU-Optionen"
 * 							<li> Wählen Sie im Fenster
 * 								 <i>Select Library Generator Settings</i>
 * 						   		 (siehe Abbildung 6) unter
 * 						   		 <i>Select Library Source</i> die
 * 						   		 Einstellung <i>Optimized</i>.
 * 						   		 Unter <i>Select Header Files</i>
 * 						   		 können Sie die in ihrem Projekt benötigten
 * 						   		 Header-Dateien angeben (wählen Sie alle an).
 * 						   		 Wählen Sie als <i>Select Library Type</i>
 * 						   		 die Einstellung <i>Pre-Built</i>.
 * 						   		 Betätigen Sie zum Schluss die
 * 						   		 Schaltfläche <i>Finish</i>.
 * 					  	   		 @image html library_settings.jpg "Abbildung 6: Bibliothekseinstellungen"
 * 							<li> Abschließend wird Ihnen eine Übersicht
 * 								 der gewählten Einstellungen angezeigt.
 * 								 Betätigen Sie in diesem Fenster die
 * 								 Schaltfläche <i>OK</i> (siehe Abbildung 7).
 * 					  	   		 @image html summary.jpg "Abbildung 7: Zusammenfassung der Einstellungen"
 * 					  		<li> Sämtliche Einstellungen (und vieles mehr)
 * 					  			 können in den Projekteigenschaften
 * 					  			 nachträglich geändert werden. Zu den
 * 					  	   		 Projekteigenschaften gelangen Sie durch
 * 					  	   		 einen Rechtsklick auf den Projektordner
 * 					  	   		 im Projektexplorer (siehe Abbildung 8) und die
 * 					  	   		 Auswahl des Eintrages <i>Projectproperties</i>
 * 					  	   		 am Ende der Liste.
 * 					  	   		 @image html project_properties.jpg "Abbildung 8: Die Projekteigenschaften"
 *							</ol>
 *					<li> Kopieren Sie nun das Verzeichnis <i>bsp</i>
 *						 in das Verzeichnis <i>src</i> in ihrem
 *						 Projektverzeichnis. Sie haben die Möglichkeit
 *					     durch Änderung der Konstanten in der Datei
 *					     bsp.h das BSP ihren Anforderungen anzupassen.
 *					<li> Löschen Sie nun alle c- und h-Dateien im
 *						 Verzeichnis <i>src</i> ihres Projektverzeichnisses
 *						 mit Ausnahme der Datei <Projektname>.c. Nennen Sie
 *					     die Datei <Projektname>.c gegebenenfalls in main.c um.
 *					<li> Öffnen Sie die Datei <Projektname>.c (oder main.c),
 *						 löschen Sie deren Inhalt und kopieren Sie dann
 *						 den folgenden Quelltext in die Datei.
 *					     @code{.c}
 *					     #include <stdio.h>
 *
 *					     int main( void )
 *					     {
 *
 *					         puts( "Hello World!\r" );
 *
 *					         return 0;
 *
 *					     }
 *					     @endcode
 *					<li> Kompilieren und Linken Sie das Projekt
 *						 (Rechtsklick auf den Projektordner im
 *						 Projektexplorer -> Build Project).
 *				    <li> Navigieren Sie durch einen Rechtsklick auf
 *				    	 den Projektordner im Projektexplorer und
 *				    	 der Auswahl des Eintrags <i>Debug As</i>
 *				    	 -> <i>Debug Configurations</i> zu den
 *				    	 Einstellungen des Debuggers. Stellen Sie
 *				    	 sicher, dass links im Fenster ihr Projekt
 *				    	 angewählt ist. Wählen Sie rechts im Fenster, im
 *				    	 Reiter <i>Startup</i> unter <i>Runtime Options</i>,
 *				    	 die Einstellung <i>set Breakpoint at main</i>
 *				    	 ab.
 *				    <li> Stellen Sie die Verbindung zwischen Board und
 *				    	 Rechner via USB her und übertragen Sie das
 *				    	 kompilierte und gelinkte Projekt durch
 *				    	 Betätigen der Schaltfläche <i>Debug</i>
 *				    	 (der grüne Käfer) auf das Board. Sollten Sie
 *				    	 das Projekt erstmalig übertragen, wählen Sie
 *				    	 im Fenster <i>Select Configuration</i> den
 *				    	 Eintrag <i><Projektname> HardwareDebug</i>.
 *				    <li> Stellen Sie erst jetzt die Verbindung zwischen
 *				         dem Board und einem Terminalprogramm via
 *				         RS232 her.
 *				    <li> Betätigen Sie die Schaltfläche <i>Resume</i>
 *				         (die Schaltfläche mit dem Startzeichen).
 *					</ul>
 *
 * @par				Wo liegt dieser Kommentar
 * 					Dieser Kommentar liegt am Beginn der Datei bsp.h.
 *
 *******************************************************************************
 */










/* Mehrfachinkludschutz. */
#ifndef __BSP_HEADER__
#define __BSP_HEADER__










/*
################################################################################
#                                 Konstanten                                   #
################################################################################
 */

/* Es folgen 'unabhängige' Konstanten. */

/*!
 * Alternative zu \c #if \c defined(\c __\c BSP_HEADER\c __\c ).
 */
#define BSP_IS_PRESENT

/*!
 * Als Ersatztext für #USED_LIB für
 * Testzwecke gedacht.
 */
#define NOLIB					( 0 )

/*!
 * Als Ersatztext für #USED_LIB gedacht.
 */
#define OPTLIB					( 1 )

/*!
 * Als Ersatztext für #USED_LIB gedacht.
 */
#define NEWLIB					( 2 )

/*!
 * Als Ersatztext für #PARITY gedacht.
 */
#define ODD						( 1 )

/*!
 * Als Ersatztext für #PARITY gedacht.
 */
#define EVEN					( 2 )










/*
################################################################################
#                               Einstellungen                                  #
################################################################################
 */

/*!
 * Mit dieser Konstanten geben Sie
 * an ob Sie die Bibliothek Optimizedlib
 * oder die Bibliothek Newlib verwenden.
 * Nutzen Sie #OPTLIB oder #NEWLIB als
 * Ersatztext.
 *
 * - OPTLIB: Sie nutzen die Bibliothek Optimizedlib
 * - NEWLIB: Sie nutzen die Bibliothek Newlib
 * .
 *
 * @note	Sie haben die Möglichkeit die
 * 			Definition dieser Konstanten als
 * 			Option an den Compiler zu übergeben.
 * 			Projectproperties -> C/C++ Build ->
 * 			Settings -> Tool Settings ->
 *			Compiler -> Source -> Macro Defines
 */
#ifndef USED_LIB
# define USED_LIB				( OPTLIB )
#endif

/*!
 * Nutzen Sie in ihrem Projekt freeRTOS?
 *
 * - 0: Sie nutzen freeRTOS nicht
 * - 1: Sie nutzen freeRTOS
 */
#define FREERTOS_IS_PRESENT		( 0 )

/*!
 * Soll ihr Programm im Supervisior-
 * oder Usermode laufen?
 *
 * - 0: Supervisiormode
 * - 1: Usermode
 *
 * @note freeRTOS funktioniert nur
 *       im Supervisiormode!
 */
#define RUN_IN_USERMODE			( 1 && !FREERTOS_IS_PRESENT )

/*!
 * Was soll mit dem primary Thread
 * nach der Bearbeitung von main( )
 * passieren?
 *
 * - 0: Der Thread betritt nach der
 *      Bearbeitung von main( ) in
 *      start.asm. eine Endlosschleife.
 * - 1: Nach der Bearbeitung von main( )
 *      wird in start.asm exit( 0 )
 *      aufgerufen und der Thread wird
 *      schlussendlich in exit.c gefangen.
 *.
 * Der Aufruf von exit( ) am Programmende
 * ist aktuell nur bei der Verwendung
 * von Optimizedlib unterstützt.
 */
#define CALL_EXIT_ON_END		( 1 && ( USED_LIB == OPTLIB ) )

/*!
 * Mit dieser Konstanten können Sie
 * den Teiler am Eingang des
 * PLL-Schaltkreises festlegen.
 *
 * PLL = ( 12 MHz / PLL_DIVISOR  ) * PLL_FACTOR.
 *
 * Gültige Teiler sind:
 * - 1
 * - 2
 * - 4
 * .
 * Andere Teiler führen zu einem Fehler.
 *
 * @see		#PLL_FACTOR
 */
#define PLL_DIVISOR				( 1 )

/*!
 * Mit dieser Konstanten können Sie
 * den Faktor am Ausgang des
 * PLL-Schaltkreises festlegen.
 *
 * PLL = ( 12 MHz / PLL_DIVISOR  ) * PLL_FACTOR.
 *
 * Gültige Faktoren sind:
 * - 8
 * - 10
 * - 12
 * - 16
 * - 20
 * - 24
 * - 25
 * - 50
 * .
 * Andere Faktoren führen zu einem Fehler.
 *
 * @see		#PLL_FACTOR
 */
#define PLL_FACTOR				( 16 )

/*!
 * Mit dieser Konstanten legen Sie
 * die Taktfrequenz der Peripheral
 * Modul Clock A (PCLKA) fest.
 *
 * PCLKA = PLL / PCLKA_DIVISOR
 *
 * PCLKA wird von dem Ethernetcontroller (ETHERC)
 * und dem Ethernet Controller Direct Momory Access Controller (EDMAC)
 * genutzt.
 *
 * Gültige Teiler sind:
 * - 2
 * - 4
 * - 8
 * - 16
 * - 32
 * - 64
 * .
 * Andere Teiler führen zu einem Fehler.
 */
#define PCLKA_DIVISOR			( 2 )

/*!
 * Mit dieser Konstanten legen Sie
 * die Taktfrequenz der Peripheral
 * Modul Clock B (PCLKB) fest.
 *
 * PCLKB = PLL / PCLKB_DIVISOR
 *
 * PCLKB wird von allen Modulen mit Ausnahme des
 * Modulen Ethernetcontroller (ETHERC) und des
 * Ethernet Controller Direct Momory Access Controller (EDMAC)
 * genutzt.
 *
 * Gültige Teiler sind:
 * - 2
 * - 4
 * - 8
 * - 16
 * - 32
 * - 64
 * .
 * Andere Teiler führen zu einem Fehler.
 */
#define PCLKB_DIVISOR			( 4 )

/*!
 * Mit dieser Konstanten legen Sie die
 * Taktfrequenz der External Bus Clock (BCLK)
 * fest.
 *
 * BCLK = PLL / BCLK_DIVISOR
 *
 * BCLK wird für den externen Bus genutzt.
 *
 * Gültige Teiler sind:
 * - 2
 * - 4
 * - 8
 * - 16
 * - 32
 * - 64
 * .
 * Andere Teiler führen zu einem Fehler.
 */
#define BCLK_DIVISOR			( 4 )

/*!
 * Mit dieser Konstanten legen Sie die
 * Taktfrequnz des Systemtaktes
 * Internal Clock (ICLK) fest.
 *
 * ICLK = PLL / ICLK_DIVISOR
 *
 * Gültige Teiler sind:
 * - 2
 * - 4
 * - 8
 * - 16
 * - 32
 * - 64
 * .
 * Andere Teiler führen zu einem Fehler.
 *
 */
#define ICLK_DIVISOR			( 2 )

/*!
 * Mit dieser Konstanten legen Sie die
 * Taktfrequenz des FlashIF Clock (FCLK)
 * fest.
 *
 * FCLK = PLL / FCLK_DIVISOR
 *
 * Gültige Teiler sind:
 * - 2
 * - 4
 * - 8
 * - 16
 * - 32
 * - 64
 * .
 * Andere Teiler führen zu einem Fehler.
 */
#define FCLK_DIVISOR			( 4 )

/*!
 * Mit dieser Konstanten legen Sie die
 * Taktfrequenz der IEBus Clock (IECLK)
 * fest.
 *
 * IECLK = PLL / IECLK_DIVISOR
 *
 * Gültige Teiler sind:
 * - 2
 * - 4
 * - 6
 * - 8
 * - 16
 * - 32
 * - 64
 * .
 * Andere Teiler führen zu einem Fehler.
 */
#define IECLK_DIVISOR			( 8 )

/*!
 * Mit dieser Konstanten legen Sie die
 * Taktfrequenz der USB Clock (UCLK)
 * fest.
 *
 * UCLK = PLL / UCLK_DIVISOR
 *
 * Gültige Teiler sind:
 * - 3
 * - 4
 * .
 * Andere Teiler führen zu einem Fehler.
 */
#define UCLK_DIVISOR			( 4 )

/*!
 * Mit dieser Konstanten legen Sie die
 * Takfrequenz des Baudrategenerators
 * der seriellen Kommunikation fest.
 *
 * SRLCLK = PCLKB / SRLCLK_DIVISOR
 *
 * Gültige Teiler sind:
 * - 1
 * - 4
 * - 16
 * - 64
 * .
 * Andere Teiler führen zu einem Fehler.
 */
#define SRLCLK_DIVISOR			( 1 )

/*!
 * Mit dieser Konstanten legen Sie
 * die Priorität der Interrupts, die
 * für die serielle Kommunikation
 * benötigt werden, fest.
 *
 * Gültig sind 1 - 15. 15 ist die höchste
 * Priorität.
 *
 * Bei gleicher Priorität wird, beim
 * gleichzeitigen Feuern der Interrupts,
 * der Interrupt mit der kleineren Vektornummer
 * zuerst bearbeitet.
 *
 * @note	Wenn Sie mit freeRTOS arbeiten, achten Sie
 * 			darauf, dass diese Priorität größer als
 *			configMAX_SYSCALL_INTERRUPT_PRIORITY ist.
 */
#define SRLCOM_INTERRUPTPRIO	( 4 )

/*!
 * Mit dieser Konstanten legen
 * Sie die Baudrate der seriellen
 * Kommunikation fest.
 *
 * Gültig sind:
 * - 9600
 * - 57600
 * - 115200
 */
#define BAUDRATE				( 9600 )

/*!
 * Mit dieser Konstanten legen Sie die
 * Anzahl der Datenbits der seriellen
 * Kommunikation fest.
 *
 * Gültig sind:
 * - 7
 * - 8
 */
#define N_DATABITS				( 8 )

/*!
 * Mit dieser Konstanten legen Sie
 * fest, ob die serielle Kommunikation
 * mit oder ohne Paritätsprüfung arbeiten
 * soll.
 *
 * - 0: Die Paritätsprüfung ist
 * 		nicht aktiv.
 * - 1: Die Paritätsprüfung ist
 * 		aktiv.
 */
#define PARITY_ENABLE			( 0 )

/*!
 * Mit dieser Konstanten legen Sie,
 * bei aktiver Paritätsprüfung
 * (siehe #PARITY_ENABLE), die
 * Parität auf endweder ungerade
 * oder gerade.
 *
 * Nutzen Sie #ODD oder #EVEN.
 */
#define PARITY					( ODD )

/*!
 * Mit dieser Konstanten legen Sie
 * die Anzahl der Stopbits der
 * seriellen Kommunikation fest.
 *
 * Gültig sind:
 * - 1
 * - 2
 */
#define N_STOPBITS				( 1 )

/*!
 * Mit dieser Konstanten steuern Sie
 * die Hintergrundbeleuchtung des
 * LCD.
 *
 * - 0: Hintergrundbeleuchtung an
 * - 1: Hintergrundbeleuchtung aus
 */
#define LCD_BACKLIGHT_DISABLE	( 1 )










/*
################################################################################
#                                 Konstanten                                   #
################################################################################
 */

/* Es folgen die 'abhängigen' Konstanten. */

/*!
 * Diese Konstante hält die
 * Taktfrequenz PLL in Hz.
 */
#define PLL_HZ					( ( 12000000 / PLL_DIVISOR ) * PLL_FACTOR )

/*!
 * Diese Konstante hält die
 * Taktfrequenz PCLKA in Hz.
 */
#define PCLKA_HZ				( PLL_HZ / PCLKA_DIVISOR )

/*!
 * Diese Konstante hält die
 * Taktfrequenz PCLKB in Hz.
 */
#define PCLKB_HZ				( PLL_HZ / PCLKB_DIVISOR )

/*!
 * Diese Konstante hält die
 * Taktfrequenz BCLK in Hz.
 */
#define BCLK_HZ					( PLL_HZ / BCLK_DIVISOR )

/*!
 * Diese Konstante hält die
 * Taktfrequenz ICLK in Hz.
 */
#define ICLK_HZ					( PLL_HZ / ICLK_DIVISOR )

/*!
 * Diese Konstante hält die
 * Taktfrequenz FCLK in Hz.
 */
#define FCLK_HZ					( PLL_HZ / FCLK_DIVISOR )

/*!
 * Diese Konstante hält die
 * Taktfrequenz IECLK in Hz.
 */
#define IECLK_HZ				( PLL_HZ / IECLK_DIVISOR )

/*!
 * Diese Konstante hält die
 * Taktfrequenz UCLK in Hz.
 */
#define UCLK_HZ					( PLL_HZ / UCLK_DIVISOR )










/*
################################################################################
#                        Validierung der Einstellungen                         #
################################################################################
 */

#if USED_LIB != OPTLIB && USED_LIB != NEWLIB && USED_LIB != NOLIB
# error "Invalid value for USED_LIB!"	/* Nicht "in bsp.h!", da das Definieren
                                           von USED_LIB auch als Option an den
                                           Compiler übergeben werden kann. */
#endif
#if USED_LIB == NOLIB
# warning "You have chosen USED_LIB == NOLIB. Are you sure?"
#endif
#if USED_LIB == NEWLIB
# error "Currently there is nothing implemented for newlib!"
#endif

#if FREERTOS_IS_PRESENT != 0 && FREERTOS_IS_PRESENT != 1
# error "Invalid value for FREERTOS_IS_PRESENT in bsp.h!"
#endif

#if RUN_IN_USERMODE != 0 && RUN_IN_USERMODE != 1
# error "Invalid value for RUN_IN_USERMODE in bsp.h!"
#endif

#if CALL_EXIT_ON_END != 0 && CALL_EXIT_ON_END != 1
# error "Invalid value for CALL_EXIT_ON_END in bsp.h!"
#endif

#if PLL_DIVISOR != 1 && PLL_DIVISOR != 2 && PLLL_DIVISOR != 4
# error "Invalid value for PLL_DIVISOR in bsp.h!"
#endif

#if PLL_FACTOR !=  8 && PLL_FACTOR != 10 && PLL_FACTOR != 12 && \
    PLL_FACTOR != 16 && PLL_FACTOR != 20 && PLL_FACTOR != 24 && \
    PLL_FACTOR != 25 && PLL_FACTOR != 50
# error "Invalid value for PLL_FACTOR in bsp.h!"
#endif

#if PCLKA_DIVISOR !=  2 && PCLKA_DIVISOR !=  4 && PCLKA_DIVISOR !=  8 && \
	PCLKA_DIVISOR != 16 && PCLKA_DIVISOR != 32 && PCLKA_DIVISOR != 64
# error "Invalid value for PCLKA_DIVISOR in bsp.h!"
#endif
#if PCLKA_HZ > 100000000
# error "PCLKA must not be greater than 100 MHz! Choose another divisor in bsp.h!"
#endif

#if PCLKB_DIVISOR !=  2 && PCLKB_DIVISOR !=  4 && PCLKB_DIVISOR !=  8 && \
	PCLKB_DIVISOR != 16 && PCLKB_DIVISOR != 32 && PCLKB_DIVISOR != 64
# error "Invalid value for PCLKB_DIVISOR in bsp.h!"
#endif
#if PCLKB_HZ > 50000000
# error "PCLKB must not be greater than 50 MHz! Choose another divisor in bsp.h!"
#endif

#if BCLK_DIVISOR !=  2 && BCLK_DIVISOR !=  4 && BCLK_DIVISOR !=  8 && \
	BCLK_DIVISOR != 16 && BCLK_DIVISOR != 32 && BCLK_DIVISOR != 64
# error "Invalid value for BCLK_DIVISOR in bsp.h!"
#endif
#if BCLK_HZ > 100000000
# error "BCLK must not be greater than 100 MHz! Choose another divisor in bsp.h!"
#endif

#if ICLK_DIVISOR !=  2 && ICLK_DIVISOR !=  4 && ICLK_DIVISOR !=  8 && \
	ICLK_DIVISOR != 16 && ICLK_DIVISOR != 32 && ICLK_DIVISOR != 64
# error "Invalid value for ICLK_DIVISOR in bsp.h!"
#endif
#if ICLK_HZ > 100000000
# error "ICLK must not be greater than 100 MHz! Choose another divisor in bsp.h!"
#endif

#if FCLK_DIVISOR !=  2 && FCLK_DIVISOR !=  4 && FCLK_DIVISOR !=  8 && \
	FCLK_DIVISOR != 16 && FCLK_DIVISOR != 32 && FCLK_DIVISOR != 64
# error "Invalid value for FCLK_DIVISOR in bsp.h!"
#endif
#if FCLK_HZ > 50000000
# error "FCLK must not be greater than 50 MHz! Choose another divisor in bsp.h!"
#endif

#if IECLK_DIVISOR !=  2 && IECLK_DIVISOR !=  4 && IECLK_DIVISOR !=  6 && \
	IECLK_DIVISOR !=  8 && IECLK_DIVISOR != 16 && IECLK_DIVISOR != 32 && \
	IECLK_DIVISOR != 64
# error "Invalid value for IECLK_DIVISOR in bsp.h!"
#endif
#if IECLK_HZ > 50000000
# error "IECLK must not be greater than 50 MHz! Choose another divisor in bsp.h!"
#endif

#if UCLK_DIVISOR != 3 && UCLK_DIVISOR != 4
# error "Inavlid value for UCLK_DIVISOR in bsp.h!"
#endif
#if UCLK_HZ > 48000000
# error "UCLK must not be greater than 48 MHz! Choose another divisor in bsp.h!"
#endif

#if SRLCLK_DIVISOR !=  1 && SRLCLK_DIVISOR != 4 && SRLCLK_DIVISOR != 16 && \
	SRLCLK_DIVISOR != 64
# error "Invalid value for SRLCLK_DIVISOR in bsp.h!"
#endif

#if SRLCOM_INTERRUPTPRIO < 1 || 15 < SRLCOM_INTERRUPTPRIO
# error "Invalid value for SRLCOM_INTERRUPTPRIO in bsp.h!"
#endif

#if BAUDRATE != 9600 && BAUDRATE != 115200 && BAUDRATE != 57600
# error "Invalid value for BAUDRATE in bsp.h!"
#endif

#if N_DATABITS != 7 && N_DATABITS != 8
# error "Invalid value for N_DATABITS in bsp.h!"
#endif

#if PARITY_ENABLE != 0 && PARITY_ENABLE != 1
# error "Invalid value for PARITY_ENABLE in bsp.h!"
#endif

#if PARITY != ODD && PARITY != EVEN
# error "Ivalid value for PARITY in bsp.h!"
#endif

#if N_STOPBITS != 1 && N_STOPBITS != 2
# error "Invalid value for N_STOPBITS in bsp.h!"
#endif

#if LCD_BACKLIGHT_DISABLE != 0 && LCD_BACKLIGHT_DISABLE != 1
# error "Invalid value for LCD_BACKLIGHT_DISABLE in bsp.h!"
#endif










#endif /* ifndef __BSP_HEADER__ */
