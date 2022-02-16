/*!
 *******************************************************************************
 *
 * @file			hwsetup.c
 *
 * @brief			Initialisierung der Hardware.
 *
 * @details			Initialisierung der Hardware vor Aufruf von main( ).
 * 					Die zentrale Funktion hardwareSetup( ) wird aus
 * 					start.asm vor dem Aufruf von main( ) aufgerufen.
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
#                                 Prototypen                                   #
################################################################################
 */

void hardwareSetup( void );
static void setupClocks( void );
static void setupOutputs( void );
static void setupInputs( void );
static void setupSCI2ForSerialIO( void );









/*
################################################################################
#                                Implementierung                               #
################################################################################
 */

/*
 * @brief		Initialisierung der Hardware.
 *
 * @details		Diese Funktion initialisiert die Hardware
 * 				vor Aufruf von main( ). Diese Funktion wird
 * 				aus start.asm heraus aufgerufen.
 */
void hardwareSetup(void)
{

	setupClocks( );

	setupOutputs( );

	setupInputs( );

	setupSCI2ForSerialIO( );

}

/* -------------------------------------------------------------------------- */

/*!
 * @brief		Konfiguration der Taktfrequenzen.
 */
static void setupClocks( void )
{

	volatile unsigned int i;

	/* Registerschutz aus.
	   PRCR = protect register. */
	SYSTEM.PRCR.WORD = 0xA503;

	/* Subclockoscillator aus.
	   SOSCCR = sub-clock oscillator control register. */
	SYSTEM.SOSCCR.BIT.SOSTP = 1;

	/* Wartezeit der Mainclock auf 10 ms (131072 cycles @ 12 MHz) setzen.
	   MOSCWTCR = main clock oscillator wait control register,
	   MSTS     = main clock oscillator wait time select. */
	SYSTEM.MOSCWTCR.BIT.MSTS = 0xD;

	/* Wartezeit des PLL-Schaltkreises auf
	   10 ms (2097152 cycles @ 192 MHz) setzen.
	   PLL     = phase locked loop,
	   PLLWTCR = pll wait control register,
	   PSTS    = pll wait time select. */
	SYSTEM.PLLWTCR.BIT.PSTS = 0xE;

	/* Ausgangstakt des PLL-Schaltkreises festlegen.
	   PLL = ( 12 MHz / PLL_DIVISOR ) * PLL_FACTOR.
	   PLLCR  = pll control register,
	   PLIDIV = pll input frequency division ratio select,
	   STC    = frequency multiplication factor select. */
	#if   PLL_DIVISOR == 1
	{
		SYSTEM.PLLCR.BIT.PLIDIV = 0;
	}
	#elif PLL_DIVISOR == 2
	{
		SYSTEM.PLLCR.BIT.PLIDIV = 1;
	}
	#elif PLL_DIVISOR == 4
	{
		SYSTEM.PLLCR.BIT.PLIDIV = 2;
	}
	#else
	{
		/* Der Fall ist in bsp.h schon abgefangen. */
	}
	#endif

	#if   PLL_FACTOR ==  8
	{
		SYSTEM.PLLCR.BIT.STC = 0x7;
	}
	#elif PLL_FACTOR == 10
	{
		SYSTEM.PLLCR.BIT.STC = 0x9;
	}
	#elif PLL_FACTOR == 12
	{
		SYSTEM.PLLCR.BIT.STC = 0xB;
	}
	#elif PLL_FACTOR == 16
	{
		SYSTEM.PLLCR.BIT.STC = 0xF;
	}
	#elif PLL_FACTOR == 20
	{
		SYSTEM.PLLCR.BIT.STC = 0x13;
	}
	#elif PLL_FACTOR == 24
	{
		SYSTEM.PLLCR.BIT.STC = 0x17;
	}
	#elif PLL_FACTOR == 25
	{
		SYSTEM.PLLCR.BIT.STC = 0x18;
	}
	#elif PLL_FACTOR == 50
	{
		SYSTEM.PLLCR.BIT.STC = 0x31;
	}
	#else
	{
		/* Der Fall ist in bsp.h schon abgefangen. */
	}
	#endif

	/* Den externen 12 MHz Oszillator einschalten.
	   MOSCCR = main clock oscillator control register,
	   MOSTP  = main clock oscillator stop. */
	SYSTEM.MOSCCR.BIT.MOSTP = 0;

	/* Den PLL-Schaltkreis einschalten.
	   PLLCR2 = pll control register 2,
	   PLLEN = pll stop control. */
	SYSTEM.PLLCR2.BIT.PLLEN = 0;

	/* Mindestens 12 ms (~2075 op/s @ LOCO) warten.
	   LOCO = low-speed on-chip oscillator = 125 KHz. */
	for( i = 0; i < 2075; i++ )
	{
		__asm__ __volatile__ (
            "NOP"
        );
	}

	/* In der Registerbeschreibung steht,
	   dass das rechte (LSB) und das 5te
	   (2^4) Bit von rechts des SCKCR 1 sein soll.
	   SCKCR = system clock control register. */
	SYSTEM.SCKCR.LONG = 0x00000011;

	/* Pinausgabe von BCLK an P53 aus. */
	SYSTEM.SCKCR.BIT.PSTOP1 = 1;

	/* Die Taktfrequnz von PCLKA festlegen. */
	#if   PCLKA_DIVISOR ==  2
	{
		SYSTEM.SCKCR.BIT.PCKA = 1;
	}
	#elif PCLKA_DIVISOR ==  4
	{
		SYSTEM.SCKCR.BIT.PCKA = 2;
	}
	#elif PCLKA_DIVISOR ==  8
	{
		SYSTEM.SCKCR.BIT.PCKA = 3;
	}
	#elif PCLKA_DIVISOR == 16
	{
		SYSTEM.SCKCR.BIT.PCKA = 4;
	}
	#elif PCLKA_DIVISOR == 32
	{
		SYSTEM.SCKCR.BIT.PCKA = 5;
	}
	#elif PCLKA_DIVISOR == 64
	{
		SYSTEM.SCKCR.BIT.PCKA = 6;
	}
	#else
	{
		/* Der Fall ist in bsp.h schon abgefangen. */
	}
	#endif

	/* Die Taktfrequnz von PCLKB festlegen. */
	#if   PCLKB_DIVISOR ==  2
	{
		SYSTEM.SCKCR.BIT.PCKB = 1;
	}
	#elif PCLKB_DIVISOR ==  4
	{
		SYSTEM.SCKCR.BIT.PCKB = 2;
	}
	#elif PCLKB_DIVISOR ==  8
	{
		SYSTEM.SCKCR.BIT.PCKB = 3;
	}
	#elif PCLKB_DIVISOR == 16
	{
		SYSTEM.SCKCR.BIT.PCKB = 4;
	}
	#elif PCLKB_DIVISOR == 32
	{
		SYSTEM.SCKCR.BIT.PCKB = 5;
	}
	#elif PCLKB_DIVISOR == 64
	{
		SYSTEM.SCKCR.BIT.PCKB = 6;
	}
	#else
	{
		/* Der Fall ist in bsp.h schon abgefangen. */
	}
	#endif

	/* Die Taktfrequenz von BCLK festlegen. */
	#if   BCLK_DIVISOR ==  2
	{
		SYSTEM.SCKCR.BIT.BCK = 1;
	}
	#elif BCLK_DIVISOR ==  4
	{
		SYSTEM.SCKCR.BIT.BCK = 2;
	}
	#elif BCLK_DIVISOR ==  8
	{
		SYSTEM.SCKCR.BIT.BCK = 3;
	}
	#elif BCLK_DIVISOR == 16
	{
		SYSTEM.SCKCR.BIT.BCK = 4;
	}
	#elif BCLK_DIVISOR == 32
	{
		SYSTEM.SCKCR.BIT.BCK = 5;
	}
	#elif BCLK_DIVISOR == 64
	{
		SYSTEM.SCKCR.BIT.BCK = 6;
	}
	#else
	{
		/* Der Fall ist in bsp.h schon abgefangen. */
	}
	#endif

	/* Die Taktfrequenz von ICLK festlegen. */
	#if   ICLK_DIVISOR ==  2
	{
		SYSTEM.SCKCR.BIT.ICK = 1;
	}
	#elif ICLK_DIVISOR ==  4
	{
		SYSTEM.SCKCR.BIT.ICK = 2;
	}
	#elif ICLK_DIVISOR ==  8
	{
		SYSTEM.SCKCR.BIT.ICK = 3;
	}
	#elif ICLK_DIVISOR == 16
	{
		SYSTEM.SCKCR.BIT.ICK = 4;
	}
	#elif ICLK_DIVISOR == 32
	{
		SYSTEM.SCKCR.BIT.ICK = 5;
	}
	#elif ICLK_DIVISOR == 64
	{
		SYSTEM.SCKCR.BIT.ICK = 6;
	}
	#else
	{
		/* Der Fall ist in bsp.h schon abgefangen. */
	}
	#endif

	/* Die Taktfrequenz von FCLK festlegen. */
	#if   FCLK_DIVISOR ==  2
	{
		SYSTEM.SCKCR.BIT.FCK = 1;
	}
	#elif FCLK_DIVISOR ==  4
	{
		SYSTEM.SCKCR.BIT.FCK = 2;
	}
	#elif FCLK_DIVISOR ==  8
	{
		SYSTEM.SCKCR.BIT.FCK = 3;
	}
	#elif FCLK_DIVISOR == 16
	{
		SYSTEM.SCKCR.BIT.FCK = 4;
	}
	#elif FCLK_DIVISOR == 32
	{
		SYSTEM.SCKCR.BIT.FCK = 5;
	}
	#elif FCLK_DIVISOR == 64
	{
		SYSTEM.SCKCR.BIT.FCK = 6;
	}
	#else
	{
		/* Der Fall ist in bsp.h schon abgefangen. */
	}
	#endif

	/* Die Taktfrequenz von IECLK festlegen.
	   SCKCR2 = system clock control register 2. */
	#if   IECLK_DIVISOR ==  2
	{
		SYSTEM.SCKCR2.BIT.IEBCK = 1;
	}
	#elif IECLK_DIVISOR ==  4
	{
		SYSTEM.SCKCR2.BIT.IEBCK = 2;
	}
	#elif IECLK_DIVISOR ==  6
	{
		SYSTEM.SCKCR2.BIT.IEBCK = 0xC;
	}
	#elif IECLK_DIVISOR ==  8
	{
		SYSTEM.SCKCR2.BIT.IEBCK = 3;
	}
	#elif IECLK_DIVISOR == 16
	{
		SYSTEM.SCKCR2.BIT.IEBCK = 4;
	}
	#elif IECLK_DIVISOR == 32
	{
		SYSTEM.SCKCR2.BIT.IEBCK = 5;
	}
	#elif IECLK_DIVISOR == 64
	{
		SYSTEM.SCKCR2.BIT.IEBCK = 6;
	}
	#else
	{
		/* Der Fall ist in bsp.h schon abgefangen. */
	}
	#endif

	/* Die Taktfrequenz von UCLK festlegen. */
	#if   UCLK_DIVISOR == 3
	{
		SYSTEM.SCKCR2.BIT.UCK = 2;
	}
	#elif UCLK_DIVISOR == 4
	{
		SYSTEM.SCKCR2.BIT.UCK = 3;
	}
	#else
	{
		/* Der Fall ist in bsp.h schon abgefangen. */
	}
	#endif

	/* PLL als Taktquelle wählen.
	   SCKCR3 = system clock control register 3,
	   CKSEL = clock source select. */
	SYSTEM.SCKCR3.BIT.CKSEL = 4;

	/* Registerschutz an. */
	SYSTEM.PRCR.WORD = 0xA500;

}

/* -------------------------------------------------------------------------- */

/*!
 * @brief		Konfiguration der Ausgangspins.
 */
static void setupOutputs( void )
{

	/* Die Hintergrundbeleuchtung des LCD abschalten. */
	#if LCD_BACKLIGHT_DISABLE
	{
		/* P21 (BL_ENA) als Ausgang definieren.
		   PDR = port direction register. */
		PORT2.PDR.BIT.B1  = 1;
		/* LCD-Hintergrundbeleuchtung aus (BL_ENA = 0).
		   PODR = port output data register. */
		PORT2.PODR.BIT.B1 = 0;
	}
	#endif

    /* Den LCD-Chip-Select (LCD_CS) Konfigurieren.
	   LCD_CD ist ein low-active Ausgang. */
	PORTC.PODR.BIT.B2 = 1;
	PORTC.PDR.BIT.B2 = 1;

	/* Den Flash-Chip-Select (FLASH_CS) Konfigurieren.
	   FLASH_CD ist ein low-active Ausgang. */
	PORTC.PODR.BIT.B0 = 1;
	PORTC.PDR.BIT.B0 = 1;

	/* Den MicroSD-Chip-Select (SDMICRO_CS) Konfigurieren.
	   SDMICRO_CS ist ein low-active Ausgang. */
	PORTC.PODR.BIT.B4 = 1;
	PORTC.PDR.BIT.B4 = 1;

	/* Den WiFi-Chip-Select (WIFI_CS) Konfigurieren.
	   WIFI_CS ist ein low-active Ausgang. */
	PORTJ.PODR.BIT.B3 = 1;
	PORTJ.PDR.BIT.B3 = 1;

}

/* -------------------------------------------------------------------------- */

/*!
 * @brief		Konfiguration der Eingangspins.
 */
static void setupInputs( void )
{
}

/* -------------------------------------------------------------------------- */

/*!
 * @brief		Konfiguration des zweiten SCI-Kanals
 * 				für die serielle Ein- und Ausgabe.
 */
static void setupSCI2ForSerialIO( void )
{

	double d;

	/* Das Schreiben in PFSWE freigeben.
	   PFSWE = pin function control register write enable,
	   MPC   = multi-function pin controller,
	   PWPR  = write-protect register,
	   B0WI  = PFSWE bit write disable. */
	MPC.PWPR.BIT.B0WI = 0;

	/* Das Schreiben in PFS freigeben.
	   PFS = pin function control register. */
	MPC.PWPR.BIT.PFSWE = 1;

	/* Registerschutz aus.
	   PRCR = protect control register. */
	SYSTEM.PRCR.WORD = 0xA50B;

	/* Taktfreigabe.
	   MSTPCRA = module stop control register a,
	   ACSE    = all-module clock stop mode enable. */
	SYSTEM.MSTPCRA.BIT.ACSE = 0;

	/* Den Module-Stop-State von SCI2 abschalten. */
	MSTP( SCI2 ) = 0;

	/* Registerschutz an. */
	SYSTEM.PRCR.WORD = 0xA500;

	/* Das Serial-Control-Register (SCR) belegen.
       Für die folgenden Einstellungen (erstmal) (alles) aus. */
	SCI2.SCR.BYTE = 0;

	/* P50 als TXD2 und IRQ-Pin. */
	MPC.P50PFS.BYTE = 0x4A;

	/* P52 als RXD2 und IRQ-Pin. */
	MPC.P52PFS.BYTE = 0x4A;

	/* P50 als Ausgang(spin).
	   PDR = port direction register. */
	PORT5.PDR.BIT.B0 = 1;

	/* P52 als Eingang(spin). */
	PORT5.PDR.BIT.B2 = 0;

	/* P50 als Peripherie.
	   PMR = port mode register. */
	PORT5.PMR.BIT.B0 = 1;

	/* P52 als Peripherie. */
	PORT5.PMR.BIT.B2 = 1;

	/* Die Taktfrequenz des
	   Baudrategenerators festlegen.
	   SMR = serial mode register,
	   CKS = clock select. */
	#if   SRLCLK_DIVISOR ==  1
	{
		SCI2.SMR.BIT.CKS = 0;
	}
	#elif SRLCLK_DIVISOR ==  4
	{
		SCI2.SMR.BIT.CKS = 1;
	}
	#elif SRLCLK_DIVISOR == 16
	{
		SCI2.SMR.BIT.CKS = 2;
	}
	#elif SRLCLK_DIVISOR == 64
	{
		SCI2.SMR.BIT.CKS = 3;
	}
	#else
	{
		/* Der Fall ist in bsp.h schon abgefangen. */
	}
	#endif

	/* Keine Multi-Prozessor Kommunikation. */
	SCI2.SMR.BIT.MP = 0;

	/* Die Anzahl der Stopbits festlegen. */
	#if   N_STOPBITS == 1
	{
		SCI2.SMR.BIT.STOP = 0;
	}
	#elif N_STOPBITS == 2
	{
		SCI2.SMR.BIT.STOP = 1;
	}
	#else
	{
		/* Der Fall ist in bsp.h schon abgefangen. */
	}
	#endif

	/* Die Paritätsprüfung konfigurieren. */
	#if PARITY_ENABLE
	{
		SCI2.SMR.BIT.PE   = 1;
		#if PARITY == ODD
		{
			SCI2.SMR.BIT.PM   = 1;
		}
		#elif PARITY == EVEN
		{
			SCI2.SMR.BIT.PM   = 0;
		}
		#endif
	}
	#else
	{
		SCI2.SMR.BIT.PE   = 0;
	}
	#endif

	/* Die Anzahl der Datenbits festlegen. */
	#if N_DATABITS == 7
	{
		SCI2.SMR.BIT.CHR  = 1;
	}
	#elif N_DATABITS == 8
	{
		SCI2.SMR.BIT.CHR  = 0;
	}
	#else
	{
		/* Der Fall ist in bsp.h schon abgefangen. */
	}
	#endif

	/* Kommunikation im asynchronen Modus. */
	SCI2.SMR.BIT.CM = 0;

	/* Serial-Interface-Modus. */
	SCI2.SCMR.BIT.SMIF = 0;

	/* Die Daten nicht invertieren. */
	SCI2.SCMR.BIT.SINV = 0;

	/* Das LSB als erstes Bit. */
	SCI2.SCMR.BIT.SDIR = 0;

	/* Die Übertragungsrate festlegen.
	   Siehe Seite 1312 in r01uh0041ej0160_rx63n631.pdf.
	   N = PCLKB / ( 64 * ( 2 ^ ( 2 * SCI2.SMR.BIT.CKS - 1 ) ) * BAUDRATE ) - 1. */
	switch ( SCI2.SMR.BIT.CKS )
	{
		case 0:
			d = 32.;
			break;
		case 1:
			d = 128.;
			break;
		case 2:
			d = 512.;
			break;
		case 3:
			d = 2048.;
			break;
		default:
			/* Hier sollte man
			   eigentlich nicht
			   hinkommen. */
			d = 32.;
			break;
	}
	SCI2.BRR = ( int ) ( PCLKB_HZ / ( d * BAUDRATE ) - 1 );

	/* Laufende Interrupts löschen.
	   RXI2 = 'Etwas-Empfangen-Interrupt',
	   TXI2 = 'Senden-Eines-Zeichens-Abgeschlossen-Interrupt',
	   TEI2 = 'Übertragung-Abgeschlossen-Interrupt'. */
	IR( SCI2, RXI2 ) = 0;
	IR( SCI2, TXI2 ) = 0;
	IR( SCI2, TEI2 ) = 0;

	/* Interrupts deaktivieren. */
	IEN( SCI2, RXI2 ) = 0;
	IEN( SCI2, TXI2 ) = 0;
	IEN( SCI2, TEI2 ) = 0;

	/* Die Priorität der Interrupts festlegen. */
	IPR( SCI2, RXI2 ) = SRLCOM_INTERRUPTPRIO;
	IPR( SCI2, TXI2 ) = SRLCOM_INTERRUPTPRIO;

	/* Das Senden einschalten. */
	SCI2.SCR.BIT.TE = 1;

	/* Das Empfangen einschalten. */
	SCI2.SCR.BIT.RE = 1;

	/* Den Sendeninterrupt aktiviert
	   und deaktiviert _write( ) in
	   write.c */

	/* Den Empfangsinterrupt aktivieren. */
	//SCI2.SCR.BIT.RIE = 1;
	//IEN( SCI2, RXI2 ) = 1;

	/* Das Schreiben in PFS blockieren. */
	MPC.PWPR.BIT.PFSWE = 0;

    /* Das Schreiben in PFSWE blockieren. */
	MPC.PWPR.BIT.B0WI = 1;

}

/* -------------------------------------------------------------------------- */
