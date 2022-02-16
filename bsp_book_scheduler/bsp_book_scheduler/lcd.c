/*!
 *******************************************************************************
 *
 * @file			lcd.c
 *
 * @brief			C-Header-Datei des Moduls lcd. Konstanten
 * 					und Funktionen für die Verwendung des LCD.
 *
 * @details			Dieses Modul realisiert Funktionen zur Bedienung des LCD.
 *                  Das LCD laesst sich auf zwei Arten benutzen:
 *                  Im Textmodus und im Grafikmodus. Die Funktionen fuer den
 *                  Textmodus beginnen mit 'lcd...' die Funktionen fuer den
 *                  Grafikmodus beginnen mit 'lcd_grf_...'.
 *                  Im Grafikmodus benutzt man einen separaten Grafikspeicher.
 *                  Mit verschiedenen Grafik-Funktionen kann man Pixel des
 *                  Grafikspeichers manipulieren.
 *                  Mit einer speziellen Funktion kann man den kompletten
 *                  Grafikspeicher auf das LCD kopieren und so sichtbar machen.
 *
 * @author			Christopher Zink, E. Forgber (Fgb)
 *
 * @par History   :
   @verbatim
   Version | Date       | Author        | Change Description
   1.0     |   .06.2015 | Zink          | First Version
   1.1     | 18.10.2016 | FGB           | Debugging. Functions st7579_clear(),
                                        | lcdClearScreen() used  too much stack
                                        | space, changed to dynamic memory
   1.2     | 19.10.2016 | Fgb           | Grafik-Routinen eingebaut

   @endverbatim
 *
 * @version			1.2
 *
 *
 *******************************************************************************
 */









/*
################################################################################
#                                    Inkluds                                   #
################################################################################
*/

#include <stdlib.h>		/* für NULL               */
#include <string.h>		/* für memset( )          */
#include "lcd.h"		/* Modulheader            */
#include "spi.h"		/* für SPI-Kom. mit LCD   */
#include "iodefine.h"	/* für Init der LCD-Ports */
#include "font.h"		/* für die "Schriftart"   */










/*
################################################################################
#                                 Modulsteuerung                               #
################################################################################
*/

#if spiUSE_LOCKING
/*!
 * Diese Konstante wird bei
 * <tt>spiUSE_LOCKING != 0</tt>
 * benötigt. Geben Sie eine
 * einmalige Prozess-ID
 * an (eine ganze Zahl
 * zwischen 0 und <tt>ULONG_MAX</tt>.
 *
 * @note		Es ist klar, dass
 * 				der aufrufende Prozess
 * 				mit nichten immer der selbe
 * 				ist. Das Locking soll sich
 * 				aber nicht auch noch durch
 * 				lcd weiter fortführen und
 * 				dennoch nutzbar sein!
 */
#define lcdPID							0x3EB1A04F
#endif /* if spiUSE_LOCKING */

/*!
 * Dies ist der SPI-Kanal, den das Modul
 * für die Kommunikation mit dem LCD
 * verwendet.
 *
 * @note		Dieser \c define liegt in
 * 				lcd.c (und nicht in lcd.h) um nicht
 * 				spi.h in lcd.h zu inkludieren.
 */
#define lcdSPI_CHANNEL					spiCHANNEL_0










/*
################################################################################
#                            Konstanten von st7579                             #
################################################################################
*/

/*!
 * @cond	Das wird von st7579 benötigt und soll nicht in der Doxygen-Doku erscheinen
 */
#define ST7579_X_PIXELS 102
#define ST7579_Y_ROWS 9
#define ST7579_NOP 0x00
#define ST7579_NOP_MASK 0x00
#define ST7579_NOP_RS 0
#define ST7579_NOP_H 255
#define ST7579_FUNCSET 0x20
#define ST7579_FUNCSET_MASK 0x1F
#define ST7579_FUNCSET_RS 0
#define ST7579_FUNCSET_H 255
#define	ST7579_FUNCSET_PARAM_MX 0x10
#define	ST7579_FUNCSET_PARAM_MY 0x08
#define	ST7579_FUNCSET_PARAM_PD 0x04
#define ST7579_WRITE 0x00
#define ST7579_WRITE_MASK 0xFF
#define ST7579_WRITE_RS 1
#define ST7579_WRITE_H 255
#define ST7579_SETVLCD 0x04
#define ST7579_SETVLCD_MASK 0x01
#define ST7579_SETVLCD_RS 0
#define ST7579_SETVLCD_H 0
#define	ST7579_SETVLCD_PARAM_PRS 0x01
#define ST7579_END 0x06
#define ST7579_END_MASK 0x00
#define ST7579_END_RS 0
#define ST7579_END_H 0
#define ST7579_READMODIFYWRITE 0x07
#define ST7579_READMODIFYWRITE_MASK 0x00
#define ST7579_READMODIFYWRITE_RS 0
#define ST7579_READMODIFYWRITE_H 0
#define ST7579_DISPLAYCTRL 0x08
#define ST7579_DISPLAYCTRL_MASK 0x05
#define ST7579_DISPLAYCTRL_RS 0
#define ST7579_DISPLAYCTRL_H 0
#define	ST7579_DISPLAYCTRL_PARAM_OFF 0x00
#define	ST7579_DISPLAYCTRL_PARAM_NORMAL 0x04
#define	ST7579_DISPLAYCTRL_PARAM_ALLON 0x01
#define	ST7579_DISPLAYCTRL_PARAM_INVERSE 0x05
#define ST7579_SETY 0x40
#define ST7579_SETY_MASK 0x0F
#define ST7579_SETY_RS 0
#define ST7579_SETY_H 0
#define ST7579_SETX 0x80
#define ST7579_SETX_MASK 0x7F
#define ST7579_SETX_RS 0
#define ST7579_SETX_H 0
#define ST7579_ROWDATAORDER 0x08
#define ST7579_ROWDATAORDER_MASK 0x04
#define ST7579_ROWDATAORDER_RS 0
#define ST7579_ROWDATAORDER_H 1
#define	ST7579_ROWDATAORDER_PARAM_DO 0x04
#define ST7579_SETBIAS 0x10
#define ST7579_SETBIAS_MASK 0x07
#define ST7579_SETBIAS_RS 0
#define ST7579_SETBIAS_H 1
#define ST7579_SETSTARTS6 0x04
#define ST7579_SETSTARTS6_MASK 0x01
#define ST7579_SETSTARTS6_RS 0
#define ST7579_SETSTARTS6_H 1
#define ST7579_SETSTART 0x40
#define ST7579_SETSTART_MASK 0x3F
#define ST7579_SETSTART_RS 0
#define ST7579_SETSTART_H 1
#define ST7579_SETVOP 0x80
#define ST7579_SETVOP_MASK 0x7F
#define ST7579_SETVOP_RS 0
#define ST7579_SETVOP_H 1
#define ST7579_PARTIALSCREEN_MODE 0x04
#define ST7579_PARTIALSCREEN_MODE_MASK 0x01
#define ST7579_PARTIALSCREEN_MODE_RS 0
#define ST7579_PARTIALSCREEN_MODE_H 2
#define	ST7579_PARTIALSCREEN_MODE_PARAM_PS 0x01
#define ST7579_PARTIALSCREEN_SIZE 0x08
#define ST7579_PARTIALSCREEN_SIZE_MASK 0x01
#define ST7579_PARTIALSCREEN_SIZE_RS 0
#define ST7579_PARTIALSCREEN_SIZE_H 2
#define	ST7579_PARTIALSCREEN_SIZE_PARAM_WS 0x01
#define ST7579_DISPLAYPART 0x10
#define ST7579_DISPLAYPART_MASK 0x07
#define ST7579_DISPLAYPART_RS 0
#define ST7579_DISPLAYPART_H 2
#define ST7579_TESTMODE 0x00
#define ST7579_TESTMODE_MASK 0x01
#define ST7579_TESTMODE_RS 0
#define ST7579_TESTMODE_H 3
#define ST7579_RESET 0x03
#define ST7579_RESET_MASK 0x00
#define ST7579_RESET_RS 0
#define ST7579_RESET_H 3
#define ST7579_FRAMERATE 0x08
#define ST7579_FRAMERATE_MASK 0x07
#define ST7579_FRAMERATE_RS 0
#define ST7579_FRAMERATE_H 3
#define ST7579_NLINE_INVERT 0x40
#define ST7579_NLINE_INVERT_MASK 0x1F
#define ST7579_NLINE_INVERT_RS 0
#define ST7579_NLINE_INVERT_H 3
#define ST7579_BOOSTER_EFFICIENCY 0x90
#define ST7579_BOOSTER_EFFICIENCY_MASK 0x0F
#define ST7579_BOOSTER_EFFICIENCY_RS 0
#define ST7579_BOOSTER_EFFICIENCY_H 3
/*!
 * @endcond
 */










/*
################################################################################
#                            dateiprivate Variablen                            #
################################################################################
*/

/*!
 * In dieser Variablen wird vermerkt, dass
 * lcdInitialize( ) aufgerufen wurde
 * (und lcdFinalize( ) noch nicht).
 *
 * - <tt>isInitialized == 0</tt>: lcdInitialize( ) wurde noch nicht
 *   aufgerufen
 * - <tt>isInitialized != 0</tt>: lcdInitialize( ) wurde aufgerufen
 * .
 */
static int isInitialized = 0;

/*!
 * Das ist die aktuelle x-Koordinate des
 * "Cursors" in Pixelen (0 <= x < lcdWIDTH).
 * x "läuft" nach rechts. Rechts ist rechts,
 * wenn die MAC-Adresse rechts neben dem LCD liegt.
 */
static int cursorX = 0;

/*!
 * Das ist die aktuelle y-Koordinate des
 * "Cursors" in Zeilen (0 <= y < lcdHEIGHT / 8).
 * y "läuft" nach unten.
 * Unten ist unten, wenn die MAC-Adresse
 * rechts neben dem LCD liegt.
 */
static int cursorY = 0;

/*!
 * @cond		Auch das stammt von st7579 und soll nicht in der Doku erscheinen
 *
 * Es wird von st7579_command( ) verwendet, um im Bilde darüber zu sein
 * ob der 'Instruction-Mode' noch gewechselt werden muss bevor der
 * Befehl gesendet wird.
 */
static unsigned char st7579_funcset;
/*!
 * @endcond
 */










/*
################################################################################
#                   Prototypen der dateiprivaten Funktionen                    #
################################################################################
*/

/*!
 * @cond		Die will ich auch nicht in der Doku!
 */
static void st7579_init();
static void st7579_go(unsigned char x, unsigned char y);
static void st7579_write( const unsigned char *buf, int len);
static void st7579_command(unsigned char reg, unsigned char mask, unsigned char data, unsigned char h);
static void st7579_rs(unsigned char onoff);
static void st7579_clear();
static void st7579_testpattern();
static void st7579_poweron();
static void st7579_poweroff();
static void st7579_display_normal();
static void st7579_display_off();
static void st7579_display_invert();
static void st7579_display_allon();
static void st7579_config_booster(unsigned char be, unsigned char pc);
static void st7579_config_vlcd(unsigned char vop);
static void st7579_config_bias(unsigned char bias);
static void st7579_config_framerate(unsigned char framerate);
static void st7579_config_startline(unsigned char st);
/*!
 * @endcond
 */



static int sgn(int x);  /* signum Funktion */






/*
################################################################################
#                  Implementierung der dateiprivaten Funktionen                #
################################################################################
*/

/*!
 * @cond		Und das soll auch nicht in der Doku erscheinen
 */
static void st7579_init()
{
	st7579_funcset = ST7579_FUNCSET_PARAM_MY | 0x01;  // MX=0, MY=1, PD=0, H=0,1
	// Init H=0,1
	st7579_command(ST7579_FUNCSET, ST7579_FUNCSET_MASK, st7579_funcset, 255);

	// YRDKRX62N Okaya LCD parameters
	st7579_config_bias(6);
	st7579_config_booster(2, 3);
	st7579_config_vlcd(0x68);

	st7579_config_framerate(0x07);
	st7579_go(0, 0);
	st7579_display_normal();
	st7579_config_startline(0);

	st7579_clear();
}

/* -------------------------------------------------------------------------- */

static void st7579_command(unsigned char reg, unsigned char mask, unsigned char data, unsigned char h)
{

	/* rs ist standardmäßig auf 0 */

	if (h < 255) {
		// Set H mode (instruction mode) if we're not already there.
		if ( (st7579_funcset & 0x03) != h ) {
			st7579_funcset = (st7579_funcset & 0xFC) | h;
			st7579_command(ST7579_FUNCSET, ST7579_FUNCSET_MASK, st7579_funcset, 255);
		}
	}

	#if ( spiUSE_LOCKING )
	{
		spiLockChannel( lcdSPI_CHANNEL, lcdPID );
		spiSelectSlave( lcdSPI_CHANNEL, spiSELECT_LCD, lcdPID );
		spiTransmitByte( lcdSPI_CHANNEL, reg | ( data & mask ), lcdPID );
		spiDeselectSlave( lcdSPI_CHANNEL, spiSELECT_LCD, lcdPID );
		spiUnlockChannel( lcdSPI_CHANNEL, lcdPID );
	}
	#else
	{
		spiSelectSlave( lcdSPI_CHANNEL, spiSELECT_LCD );
		spiTransmitByte( lcdSPI_CHANNEL, reg | ( data & mask ) );
		spiDeselectSlave( lcdSPI_CHANNEL, spiSELECT_LCD );
	}
	#endif

}

/* -------------------------------------------------------------------------- */

static void st7579_write(const unsigned char *buf, int len)
{
	int i;

	#if ( spiUSE_LOCKING )
	{
		spiLockChannel( lcdSPI_CHANNEL, lcdPID );
		spiSelectSlave( lcdSPI_CHANNEL, spiSELECT_LCD, lcdPID );
	}
	#else
	{
		spiSelectSlave( lcdSPI_CHANNEL, spiSELECT_LCD );
	}
	#endif

	st7579_rs(1);

	for (i=0; i < len; i++) {
		#if ( spiUSE_LOCKING )
		{
			spiTransmitByte( lcdSPI_CHANNEL, buf[ i ], lcdPID );
		}
		#else
		{
			spiTransmitByte( lcdSPI_CHANNEL, buf[ i ] );
		}
		#endif
	}

	#if ( spiUSE_LOCKING )
	{
		spiDeselectSlave( lcdSPI_CHANNEL, spiSELECT_LCD, lcdPID );
		spiUnlockChannel( lcdSPI_CHANNEL, lcdPID );
	}
	#else
	{
		spiDeselectSlave( lcdSPI_CHANNEL, spiSELECT_LCD );
	}
	#endif

	st7579_rs(0);

}

/* -------------------------------------------------------------------------- */
/* FGB, 18.10.2016 DEBUGGING
 * this function uses more than 102 byte stack space. THis may cause problems when
 * used in other functions, also using some stack!
 * work-around: don't use stack space for the array, but dynamic memory!
 * */
static void st7579_clear()
{
#if 0	// the original: uses stack space, may cause stack overflow!
	unsigned char y, dpydata[102];

	memset(dpydata, 0, 102);  // Newlib uses ASM string-manipulation instructions for this!
	for (y=0; y < 10; y++) {
		st7579_command(ST7579_SETY, ST7579_SETY_MASK, y, ST7579_SETY_H);  // Set row
		st7579_command(ST7579_SETX, ST7579_SETX_MASK, 0, ST7579_SETX_H);  // Set column
		st7579_write(dpydata, 102);
	}
	st7579_command(ST7579_SETY, ST7579_SETY_MASK, 0, ST7579_SETY_H);  // Reset current position to origin
	st7579_command(ST7579_SETX, ST7579_SETX_MASK, 0, ST7579_SETX_H);

#else

	unsigned char y, *dpydata;

	if(NULL == (dpydata = malloc(102))) return;

	memset(dpydata, 0, 102);  // Newlib uses ASM string-manipulation instructions for this!
	for (y=0; y < 10; y++) {
		st7579_command(ST7579_SETY, ST7579_SETY_MASK, y, ST7579_SETY_H);  // Set row
		st7579_command(ST7579_SETX, ST7579_SETX_MASK, 0, ST7579_SETX_H);  // Set column
		st7579_write(dpydata, 102);
	}
	st7579_command(ST7579_SETY, ST7579_SETY_MASK, 0, ST7579_SETY_H);  // Reset current position to origin
	st7579_command(ST7579_SETX, ST7579_SETX_MASK, 0, ST7579_SETX_H);

	free(dpydata);
#endif
}

/* -------------------------------------------------------------------------- */

static void st7579_testpattern()
{
	unsigned char y, x, c=0x80, dpydata[102];

	for (x=0; x < 102; x++) {  // Set a pattern with a single bit getting shifted down
		dpydata[x] = c;
		c >>= 1;
		if (!c) c = 0x80;
	}

	st7579_go(0, 0);
	for (y=0; y < 9; y++) {
		//st7579_go(0, y);
		st7579_write(dpydata, 102);
	}
	st7579_go(0, 0);
}

/* -------------------------------------------------------------------------- */

/* Koordinatenursprung ist die linke, obere Ecke.
 * x läuft nach rechts, y läuft nach unten.
 * (x,y) mein nicht einen Pixel sondern x ist eine der 102 Spalten (a 1 Pixel)
 * und y ist eine der 8 Zeilen (a 8 Pixel).
 */
static void st7579_go(unsigned char x, unsigned char y)
{
	st7579_command(ST7579_SETY, ST7579_SETY_MASK, y, ST7579_SETY_H);
	st7579_command(ST7579_SETX, ST7579_SETX_MASK, x, ST7579_SETX_H);
}

/* -------------------------------------------------------------------------- */

static void st7579_poweron()
{
	st7579_funcset &= ~ST7579_FUNCSET_PARAM_PD;
	st7579_command(ST7579_FUNCSET, ST7579_FUNCSET_MASK, st7579_funcset, ST7579_FUNCSET_H);
}

/* -------------------------------------------------------------------------- */

static void st7579_poweroff()
{
	st7579_funcset |= ST7579_FUNCSET_PARAM_PD;
	st7579_command(ST7579_FUNCSET, ST7579_FUNCSET_MASK, st7579_funcset, ST7579_FUNCSET_H);
}

/* -------------------------------------------------------------------------- */

static void st7579_display_normal()
{
	st7579_command(ST7579_DISPLAYCTRL, ST7579_DISPLAYCTRL_MASK, ST7579_DISPLAYCTRL_PARAM_NORMAL, ST7579_DISPLAYCTRL_H);
}

/* -------------------------------------------------------------------------- */

static void st7579_display_off()
{
	st7579_command(ST7579_DISPLAYCTRL, ST7579_DISPLAYCTRL_MASK, ST7579_DISPLAYCTRL_PARAM_OFF, ST7579_DISPLAYCTRL_H);
}

/* -------------------------------------------------------------------------- */

static void st7579_display_invert()
{
	st7579_command(ST7579_DISPLAYCTRL, ST7579_DISPLAYCTRL_MASK, ST7579_DISPLAYCTRL_PARAM_INVERSE, ST7579_DISPLAYCTRL_H);
}

/* -------------------------------------------------------------------------- */

static void st7579_display_allon()
{
	st7579_command(ST7579_DISPLAYCTRL, ST7579_DISPLAYCTRL_MASK, ST7579_DISPLAYCTRL_PARAM_ALLON, ST7579_DISPLAYCTRL_H);
}

/* -------------------------------------------------------------------------- */

static void st7579_config_booster(unsigned char be, unsigned char pc)
{
	st7579_command(ST7579_BOOSTER_EFFICIENCY, ST7579_BOOSTER_EFFICIENCY_MASK, ((be & 0x03) << 2) | (pc & 0x03),
		       ST7579_BOOSTER_EFFICIENCY_H);
}

/* -------------------------------------------------------------------------- */

static void st7579_config_vlcd(unsigned char vop)
{
	st7579_command(ST7579_SETVLCD, ST7579_SETVLCD_MASK, (vop & 0x80) >> 7, ST7579_SETVLCD_H);
	st7579_command(ST7579_SETVOP, ST7579_SETVOP_MASK, vop & 0x7F, ST7579_SETVOP_H);
}

/* -------------------------------------------------------------------------- */

static void st7579_config_bias(unsigned char bias)
{
	st7579_command(ST7579_SETBIAS, ST7579_SETBIAS_MASK, bias & 0x07, ST7579_SETBIAS_H);
}

/* -------------------------------------------------------------------------- */

static void st7579_config_framerate(unsigned char framerate)
{
	st7579_command(ST7579_FRAMERATE, ST7579_FRAMERATE_MASK, framerate & 0x07, ST7579_FRAMERATE_H);
}

/* -------------------------------------------------------------------------- */

static void st7579_config_startline(unsigned char st)
{
	st7579_command(ST7579_SETSTART, ST7579_SETSTART_MASK, st & 0x3F, ST7579_SETSTART_H);
	st7579_command(ST7579_SETSTARTS6, ST7579_SETSTARTS6_MASK, (st & 0x40) >> 6, ST7579_SETSTARTS6_H);
}

/* -------------------------------------------------------------------------- */

static void st7579_rs(unsigned char onoff)
{
	PORT5.PODR.BIT.B1 = onoff;
}
/*!
 * @endcond
 */




static int sgn(int x){
  return (x > 0) ? 1 : (x < 0) ? -1 : 0;
}





/*
################################################################################
#                  Implementierung der exportierten Funktionen                 #
################################################################################
*/

/*!
 * @brief		Initialisierung des Moduls lcd.
 *
 * @details		Führt alle nötigen Initialisierungen aus.
 *
 * @return		Wenn das Initialiseren erfolgreich abgeschlossen
 * 				wurde, gibt die Fkt. #lcdERROR_NONE zurück. Wenn
 * 				während der Initialisierung ein Fehler auftritt,
 * 				wird eine negative Zahl zurückgegeben (siehe
 * 				#lcdERROR_SPI_INIT_FAILED).
 */
int lcdInitialize( void )
{

	if ( isInitialized )
	{
		return lcdERROR_NONE;
	}

	if ( spiInitializeChannel( lcdSPI_CHANNEL ) != spiERROR_NONE )
	{
		return lcdERROR_SPI_INIT_FAILED;
	}

	/* BL_ENA (Backlight-Enable)
	   als GPI0-Ausgangspin
	   konfigurieren und mit
	   1 initialisieren.
	   0 -> Hintergrundbeleuchtung aus,
	   1 -> Hintergrundbeleuchtung ein. */
	PORT2.PODR.BIT.B1 = 1;
	PORT2.PMR.BIT.B1  = 0;
	PORT2.PDR.BIT.B1  = 1;

	/* LCD_RS als GPIO-Ausgangspin
	   konfigurieren und auf 1 setzen.
	   1 -> Data,
	   0 -> Command. */
	PORT5.PODR.BIT.B1 = 0;
	PORT5.PMR.BIT.B1  = 0;
	PORT5.PDR.BIT.B1  = 1;

    /* Den LCD_RESET (laut E-Plan RESET_IO)
       zunächst als GPIO-Ausgangs-Pin
       konfigurieren. */
	PORTC.PMR.BIT.B3 = 0;
	PORTC.PDR.BIT.B3 = 1;

    st7579_init( );

    lcdSetBackLight( 1 );

    //st7579_testpattern();

	isInitialized = 1;

	return lcdERROR_NONE;

}

/* -------------------------------------------------------------------------- */

/*!
 * @brief		Deinitialisierung des Moduls lcd.
 *
 * @return		Wenn die Deinitialisierung erfolgreich abgeschlossen
 * 				werden konnte, gibt diese Funktion #lcdERROR_NONE
 * 				zurück. Tritt beim Deinitialisieren ein Fehler auf,
 * 				wird eine negative Zahl, die den Fehler beschreibt,
 * 				zurückgegeben (siehe #lcdERROR_SPI_FINALIZE_FAILED).
 */
int lcdFinalize( void )
{

	int returnValue;

	if ( !isInitialized )
	{
		return lcdERROR_NONE;
	}

	lcdClearScreen( );

	lcdSetBackLight( 0 );

	st7579_config_bias( 0 );

	#if spiUSE_LOCKING
	{
		returnValue = spiFinalizeChannel( lcdSPI_CHANNEL, lcdPID );
	}
	#else
	{
		returnValue = spiFinalizeChannel( lcdSPI_CHANNEL );
	}
	#endif

	if ( returnValue != spiERROR_NONE )
	{
		return lcdERROR_SPI_FINALIZE_FAILED;
	}

	isInitialized = 0;

	return lcdERROR_NONE;

}

/* -------------------------------------------------------------------------- */

/*!
 * @brief		Kontrolle der Hintergrundbeleuchtung des LCD.
 *
 * @details		Diese Funktion schaltet, wenn das Modul lcd
 * 				ordnungsgemäß initialisiert ist (siehe lcdInitialize( )),
 * 				die Hintergrundbeleuchtung des LCD ein, wenn \c
 * 				onIfNoneZero von Null verschieden ist und aus
 * 				wenn <tt>onIfNoneZero == 0</tt> ist.
 *
 * @param		onIfNoneZero	Bei <tt>!= 0</tt> wird die
 * 								Hintergrundbeleuchtung ein- und bei
 * 								<tt>== 0</tt> abgeschaltet.
 *
 * @return		Bei fehlerfreiem Verlauf gibt die Funktion #lcdERROR_NONE
 * 				zurück. Im Falle eines Fehlers gibt sie eine negative Zahl
 * 				zurück (siehe #lcdERROR_NOT_INITIALIZED).
 */
int lcdSetBackLight( int onIfNoneZero )
{

	if ( isInitialized )
	{
		PORT2.PODR.BIT.B1 = onIfNoneZero != 0;
		return lcdERROR_NONE;
	}

	return lcdERROR_NOT_INITIALIZED;

}

/* -------------------------------------------------------------------------- */

/*!
 * @brief		Ausgabe eines Musters zu Testzwecken.
 *
 * @return		Wenn das Muster erfolgreich gesendet wurde, gibt die
 * 				Funktion #lcdERROR_NONE zurück. Wenn das Modul
 * 				lcd vor Aufruf nicht initialisiert wurde, gibt die
 * 				Funktion #lcdERROR_NOT_INITIALIZED zurück.
 */
int lcdPrintTestPattern( void )
{

	if ( isInitialized )
	{

		int i, j;

		/* Cursor links oben in die Ecke. */
		st7579_go( 0, 0 );
		cursorX = 0;
		cursorY = 0;

		/* 0 ... 0xFF drei mal durchlaufen. */
		for ( i = 0; i <= 0x2FF; i++ )
		{
			if ( i % lcdWIDTH == 0 && i )
			{
				/* Der ST7579 verfügt über
				   102 Spalten, der LCD aber
				   nur über 96 (lcdWIDTH),
				   also 6 Dummy-Writes um in
				   die nächste Zeile zu gelangen
				   bevor die nächsten Daten
				   gesendet werden. */
				for ( j = 0; j < 6; j++ )
				{
					st7579_write( ( unsigned char * ) &j, 1 );
				}
			}
			st7579_write( ( unsigned char * ) &i, 1 );
		}

		/* Der ST7579 hat 68 Zeilen, der LCD
		   aber nur 64, also 6 Dummy-Writes um in
		   die nächste Zeile zu gelangen und 102
		   Dummy-Writes um an das Ende der Zeile zu
		   gelangen. */
		for ( i = 0; i < 6 + 102; i++ )
		{
			st7579_write( ( unsigned char * ) &i, 1 );
		}

		/* Was jetzt gesendet werden würde, würde wieder
		   oben links erscheinen. */

		return lcdERROR_NONE;

	}

	return lcdERROR_NOT_INITIALIZED;

}

/* -------------------------------------------------------------------------- */

/*!
 * @brief		Ausgabe einer Zeichenkette auf dem LCD.
 *
 * @details		Gibt die Zeichenkette \c string auf
 * 				dem LCD aus. Für die Schriftart siehe
 * 				font.h und font.c.
 *
 * @param	string	Die Zeichenkette, die auf dem CLD
 * 					angezeigt werden soll.
 *
 * @return		Wenn die Zeichenkette erfolgreich zum
 * 				LCD übertragen wurde, gibt die Funktion
 * 				#lcdERROR_NONE zurück. Wenn ein
 * 				Fehler auftritt, gibt die Funktion eine
 * 				negative Zahl zurück, die den Fehler
 * 				beschreibt (siehe #lcdERROR_INVALID_ARGUMENT
 * 				und #lcdERROR_NOT_INITIALIZED).
 */
int lcdPutString( const char * string )
{

	int i;

	if ( string == NULL )
	{
		return lcdERROR_INVALID_ARGUMENT;
	}

	if ( !isInitialized )
	{
		return lcdERROR_NOT_INITIALIZED;
	}

	for ( i = 0; string[ i ]; i++ )
	{

		st7579_write( font[ string[ i ] ], fontWIDTH );

		/* Wenn der nächste Buchstabe nicht mehr in
		   die Zeile passt, in x auf den Zeilen Beginn
		   und in y auf die nächste Zeile oder die
		   erste Zeile springen. Die Spalten werden in
		   Pixel adressiert. die Zeilen in Blöcken a
		   8 Pixel. */
		if ( ( cursorX = cursorX + fontWIDTH ) > lcdWIDTH - fontWIDTH )
		{
			cursorX = 0;
			cursorY = ( cursorY + 1 ) % ( lcdHEIGHT / 8 );
			st7579_go( cursorX, cursorY );
		}

	}

	return lcdERROR_NONE;

}

/* -------------------------------------------------------------------------- */

/*!
 * @brief		Löschen des Anzeigeinhaltes des LCD.
 *
 * @details		Diese Funktion löscht die Anzeige.
 *
 * @return		Wenn die Anzeige erfolgreich gelöscht
 * 				wurde, gibt die Funktion #lcdERROR_NONE
 * 				zurück. Beim Auftreten eines Fehlers
 * 				wird eine negative Zahl zurückgegeben
 * 				(siehe #lcdERROR_NOT_INITIALIZED).
 */
int lcdClearScreen( void )
{

	if ( isInitialized )
	{
		st7579_clear( );
		cursorX = 0;
		cursorY = 0;

		return lcdERROR_NONE;
	}

	return lcdERROR_NOT_INITIALIZED;

}



/*! #######################################################################
 * @brief		Positioniert den Textcursor
 *
 * @details		Positioniert den Textcursor fuer den naechsten Aufruf von
 *              lcdPutString().
 *
 * @see
 * @arg         lcdPutString()
 *
 * @param   row: 0..7, Zeile fuer die naechste Textausgabe mit lcdPutString()
 * @param   col: 0..11, Spalte fuer die naechste Textausgabe mit lcdPutString()
 *
 * @return      lcdERROR_NONE: ok
 * @return      lcdERROR_NOT_INITIALIZED: LCD nicht initialisiert
 *
 * ######################################################################### */
int lcdSetTextPosition(unsigned char row, unsigned char col)
{
	if ( !isInitialized )
	{
		return lcdERROR_NOT_INITIALIZED;
	}

	// Cursorposition auf gueltigen Bereich begrenzen
	if(row >= ( lcdHEIGHT / 8 ))
	{	row =  ( lcdHEIGHT / 8 ) - 1;
	}
	if(col >= (lcdWIDTH / fontWIDTH))
	{	col = (lcdWIDTH / fontWIDTH) - 1;
	}
	// globale Variable setzen
	cursorX = col*fontWIDTH;
	cursorY = row;
	// Cursorposition ans LCD senden
	st7579_go( cursorX, cursorY );

	return 0;
}








int lcdPrintTestPattern_V2( void )
{   unsigned char x=0,y=2,i;

	if(!isInitialized)
		return lcdERROR_NONE;

	st7579_go(x,y);
	for(x=0; x<96;x++)
	{
		st7579_write( &x, 1 );
	}

	y=5;
	x=20;
	st7579_go(x,y);
	for(i=0xff; i>0x9f;i--)
	{
			st7579_write( &i, 1 );
	}



#if 0
	if ( isInitialized )
	{

		int i, j;

		/* Cursor links oben in die Ecke. */
		st7579_go( 0, 0 );
		cursorX = 0;
		cursorY = 0;

		/* 0 ... 0xFF drei mal durchlaufen. */
		for ( i = 0; i <= 0x2FF; i++ )
		{
			if ( i % lcdWIDTH == 0 && i )
			{
				/* Der ST7579 verfügt über
				   102 Spalten, der LCD aber
				   nur über 96 (lcdWIDTH),
				   also 6 Dummy-Writes um in
				   die nächste Zeile zu gelangen
				   bevor die nächsten Daten
				   gesendet werden. */
				for ( j = 0; j < 6; j++ )
				{
					st7579_write( ( unsigned char * ) &j, 1 );
				}
			}
			st7579_write( ( unsigned char * ) &i, 1 );
		}

		/* Der ST7579 hat 68 Zeilen, der LCD
		   aber nur 64, also 6 Dummy-Writes um in
		   die nächste Zeile zu gelangen und 102
		   Dummy-Writes um an das Ende der Zeile zu
		   gelangen. */
		for ( i = 0; i < 6 + 102; i++ )
		{
			st7579_write( ( unsigned char * ) &i, 1 );
		}

		/* Was jetzt gesendet werden würde, würde wieder
		   oben links erscheinen. */

		return lcdERROR_NONE;

	}
#endif
	return lcdERROR_NOT_INITIALIZED;

}




/* ******************************************************************************* */
/*                   Grafik-Routinen                                               */
/* ******************************************************************************* */


#define GRF_COLS_X 96
#define GRF_ROWS_Y 64

static unsigned char *_grf_mem_pt=NULL;              /*! graphic memory */


/*************************************************************************
 * Grafikspeicher:
 *
 * Das LCD bietet eine Aufloesung von 96 Spalten und 64 Zeilen. Dabei ist die Montage:
 * RS232-Anschluss ist oben, RJ45-Anschluss ist rechts.
 *
 * Die interne Speicherorganisation ist etwas komlizierter: Es gibt 96*8 Byte Speicher,
 * ein Byte speichert 8 vertikale Pixel
 *
 * Im Bild sind zwei Byte exemplarisch dargestellt:
 *                  Byte A realisiert die Pixel (3|0),(3|1),...,(3|7)
 *                  Byte B realisiert die Pixel (5|8),(5|9),...,(5|15)
 *
 *        0   1   2   3   4   5                                 x -->
 *      -----------------------------------------------------------------
 *  0   |   |   |   |Ab0|   |   |
 *      -----------------------------------------------------------------
 *  1   |   |   |   |Ab1|   |   |
 *      -----------------------------------------------------------------
 *  2   |   |   |   |Ab2|   |   |
 *      -----------------------------------------------------------------
 *  3   |   |   |   |Ab3|   |   |
 *      -----------------------------------------------------------------
 *  4   |   |   |   |Ab4|   |   |
 *      -----------------------------------------------------------------
 *  5   |   |   |   |Ab5|   |   |
 *      -----------------------------------------------------------------
 *  6   |   |   |   |Ab6|   |   |
 *      -----------------------------------------------------------------
 *  7   |   |   |   |Ab7|   |   |
 *      -----------------------------------------------------------------
 *  8   |   |   |   |   |   |Bb0|
 *      -----------------------------------------------------------------
 *  9   |   |   |   |   |   |Bb1|
 *      -----------------------------------------------------------------
 *  10  |   |   |   |   |   |Bb2|
 *      -----------------------------------------------------------------
 *           ...
 *  15  |   |   |   |   |   |Bb7|
 *      -----------------------------------------------------------------
 *  y
 *  |
 *  \/
 *
 *
 * Der Font-Speicher ist entsprechend organisiert (siehe font.c)
 *
 *
 *
 *
 *
 *
 *******************************************************************************/


/*! ######################################################################
 * @brief		Initialisiert den Grafikspeicher, lcdInitialize() muss vorher ausgefuehrt werden
 *
 * @details		Initialisiert den Grafikspeicher, lcdInitialize()
 *              muss vorher ausgefuehrt werden!
 *              Der Grafikspeicher ist ein separater Speicher. Alle
 *              Plotfunktionen benutzen diesen Speicher. Aenderungen werden
 *              erst sichtbar, wenn
 *              man den Speicher mit der Funktion lcd_grf_UpdateGraphicScreen()
 *              zur Anzeige bringt.
 *
 *
 * @see
 * @arg         lcdInitialize(), lcd_grf_Finalize()
 *
 * @param		keine
 *
 * @return		0: ok
 * @return      -1: Fehler in malloc()
 *
 * ###################################################################### */
int lcd_grf_Initialize( void )
{
	/* ein Byte repraesentiert 8 Pixel in vertikaler Richtung! */
	_grf_mem_pt = malloc(GRF_COLS_X*GRF_ROWS_Y/8);
	if(NULL==_grf_mem_pt)
	{ 	return -1;
	}
	return 0;
}


/*! ######################################################################
 * @brief		Gibt den Grafikspeicher wieder frei.
 *
 * @details		Der Grafikspeicher ist ein separater Speicher. Alle
 *              Plotfunktionen benutzen diesen Speicher. Diese Funktion
 *              gibt den zuvor mit lcd_grf_Initialize() reservierten
 *              Speicher wieder frei.
 *
 *
 * @see
 * @arg         lcd_grf_Initialize()
 *
 * @param		keine
 *
 * @return		0: ok
 * @return      -1: Fehler, Speicher ist gar nicht initialisiert worden.
 *
 * ###################################################################### */
int lcd_grf_Finalize( void )
{
	if(NULL==_grf_mem_pt)
	{ 	return -1;
	}
	free(_grf_mem_pt);
	return 0;
}


/*! ######################################################################
 * @brief		Schreibt ein Testmuster in den Grafikspeicher
 *
 * @details		Schreibt ein Testmuster in den Grafikspeicher
 *
 *
 * @see
 * @arg
 *
 * @param		keine
 *
 * @return		0: ok
 * @return      -1: Fehler, Grafikspeicher ist nicht initialisiert.
 *
 * ###################################################################### */
int lcd_grf_PlotTestPattern( void )
{	int x,y;

    if(NULL==_grf_mem_pt)
	{ 	return -1;
	}

    lcd_grf_ClearGraphicScreen();


    for(x=0; x<GRF_COLS_X; x++)
    {
    	for(y=0; y<GRF_ROWS_Y/8; y+=2)
    	{
    		_grf_mem_pt[y*GRF_COLS_X+x] = 0x55;
    	}
    }

	return 0;
}


/*! ######################################################################
 * @brief		Kurzversion
 *
 * @details		Langversion
 *
 *
 * @see
 * @arg         aehnliche Fktn
 *
 * @param		jeder Parameter mit einem Eintrag
 *
 * @return		jeder Rueckgabewert mit einem Eintrag
 *
 * ###################################################################### */
int lcd_grf_PlotString( const char * str)
{
	puts("TODO:lcd_grf_PlotString!\r\n");
	return 0;
}


/*! ######################################################################
 * @brief		Loescht den Grafikspeicher
 *
 * @details		Der Grafikspeicher wird mit 0 gefuellt. Der Grafikspeicher wird
 *              mit lcd_grf_UpdateGraphicScreen() zur Anzeige gebracht.
 *
 *
 * @see
 * @arg         aehnliche Fktn
 *
 * @param		jeder Parameter mit einem Eintrag
 *
 * @return		jeder Rueckgabewert mit einem Eintrag
 *
 * ###################################################################### */
int lcd_grf_ClearGraphicScreen( void )
{	int x,y;

	for(x=0; x<GRF_COLS_X; x++)
	{	for(y=0; y<GRF_ROWS_Y/8; y++)
		{	_grf_mem_pt[y*GRF_COLS_X+x] = 0x00;
		}
	}
	return 0;
}



/*! ######################################################################
 * @brief		Stellt den Grafikspeicher auf dem LCD dar.
 *
 * @details		Der Grafikspeicher ist ein separater Speicher. Alle
 *              Plotfunktionen benutzen diesen Speicher. Diese Funktion
 *              kopiert den Grafikspeicher auf das LCD.
 *
 *
 * @see
 * @arg         aehnliche Fktn
 *
 * @param		keine
 *
 * @return		0: ok
 * @return      -1: Grafikspeicher nicht initialisiert
 *
 * ###################################################################### */
int lcd_grf_UpdateGraphicScreen( void )
{	unsigned char y;
	unsigned char *pt;

	if(NULL==_grf_mem_pt)
	{	return -1;
	}

	pt = _grf_mem_pt;
	for(y=0; y<GRF_ROWS_Y/8; y++)
	{	st7579_go(0,y);
		st7579_write( pt, GRF_COLS_X );
		pt += GRF_COLS_X;
	}

	return 0;
}



/*! ######################################################################
 * @brief		Setzt das Pixel[x][y] im Grafikspeicher
 *
 * @details		Alle Plot-Befehle bearbeiten nur den Grafikspeicher. Die
 *              Aenderungen muessen mit lcd_grf_UpdateGraphicScreen()
 *              explizit sichtbar gemacht werden. Liegt das Pixel ausserhalb
 *              des darstellbaren Bereiches, so wird nichts gezeichnet.
 *
 *
 * @see
 * @arg         lcd_grf_ClearPixel(), lcd_grf_UpdateGraphicScreen()
 *
 * @param		x: x-Koordinate des Pixels 0..GRF_COLS_X-1
 * @param       y: y-Koordinate des Pixels 0..GRF_ROWS_Y-1
 * @param       val: 0 zeichnet weiss, sonst schwarz
 *
 * @return		0: ok
 * @return      -1: Grafikspeicher nicht initialisiert
 * @return      -2: Koordinaten ausserhalb des darstellbaren Bereichs
 * ###################################################################### */
int lcd_grf_SetPixel(unsigned char x, unsigned char y, unsigned char val)
{   unsigned char buf;
    unsigned short bit_in_buf, x_mem, y_mem;

	if(NULL==_grf_mem_pt)
	{ 	return -1;
	}
	if((x>=GRF_COLS_X) || (y>=GRF_ROWS_Y))
	{	return -2;
	}
	// Koordinaten im Grafikspeicher:
	x_mem = x;
	y_mem = y / 8;  // Spalte x, 8 Spaltenpixel in Byte gespeichert
	bit_in_buf = y % 8;
	// das betreffende Byte aus dem Grafikspeicher holen:
	buf = _grf_mem_pt[y_mem*GRF_COLS_X+x_mem];
	if(0!=val)
	{	// das betreffende Bit setzen:
		buf |= (1<<bit_in_buf);
	}
	else
	{	// das betreffende Bit loeschen:
		buf &= ~(1<<bit_in_buf);
	}
	// das betreffende Byte zurueck schreiben:
	_grf_mem_pt[y_mem*GRF_COLS_X+x_mem] = buf;

	return 0;
}




/*! ######################################################################
 * @brief		Zeichnet eine Linie
 *
 * @details		Zeichnet eine Linie von (xstart,ystart) nach (xend,yend). Die Linie kann
 *              schwarz oder weiss sein.
 * 				Alle Plot-Befehle bearbeiten nur den Grafikspeicher. Die
 *              Aenderungen muessen mit lcd_grf_UpdateGraphicScreen()
 *              explizit sichtbar gemacht werden. Liegt ein Pixel ausserhalb
 *              des darstellbaren Bereiches, so wird nichts gezeichnet.
 *              Es wird der Bresenham-Algorithmus verwendet [Wikipedia.org].
 *
 *
 * @see
 * @arg         lcd_grf_SetPixel(), lcd_grf_UpdateGraphicScreen()
 *
 * @param		xstart: x-Koordinate Punkt P1 0..GRF_COLS_X-1
 * @param       ystart: y-Koordinate Punkt P2 0..GRF_ROWS_Y-1
 * @param		xend: x-Koordinate Punkt P1 0..GRF_COLS_X-1
 * @param       yend: y-Koordinate Punkt P2 0..GRF_ROWS_Y-1
 * @param       val: 0 zeichnet eine weisse Linie, sonst schwarz
 *
 *
 * @return		0: ok
 * @return      -1: Grafikspeicher nicht initialisiert
 * ###################################################################### */
int lcd_grf_DrawLine(unsigned char xstart, unsigned char ystart,
		             unsigned char xend,   unsigned char yend,
		             unsigned char val)
{	int x, y, t, dx, dy, incx, incy, pdx, pdy, ddx, ddy, es, el, err;


	if(NULL==_grf_mem_pt)
	{ 	return -1;
	}

	/* Entfernung in beiden Dimensionen berechnen */
	   dx = xend - xstart;
	   dy = yend - ystart;

	/* Vorzeichen des Inkrements bestimmen */
	   incx = sgn(dx);
	   incy = sgn(dy);
	   if(dx<0) dx = -dx;
	   if(dy<0) dy = -dy;

	/* feststellen, welche Entfernung größer ist */
	   if (dx>dy)
	   {
	      /* x ist schnelle Richtung */
	      pdx=incx; pdy=0;    /* pd. ist Parallelschritt */
	      ddx=incx; ddy=incy; /* dd. ist Diagonalschritt */
	      es =dy;   el =dx;   /* Fehlerschritte schnell, langsam */
	   } else
	   {
	      /* y ist schnelle Richtung */
	      pdx=0;    pdy=incy; /* pd. ist Parallelschritt */
	      ddx=incx; ddy=incy; /* dd. ist Diagonalschritt */
	      es =dx;   el =dy;   /* Fehlerschritte schnell, langsam */
	   }

	/* Initialisierungen vor Schleifenbeginn */
	   x = xstart;
	   y = ystart;
	   err = el/2;
	   lcd_grf_SetPixel((unsigned char) x, (unsigned char) y, val);
	   //SetPixel(x,y);

	/* Pixel berechnen */
	   for(t=0; t<el; ++t) /* t zaehlt die Pixel, el ist auch Anzahl */
	   {
	      /* Aktualisierung Fehlerterm */
	      err -= es;
	      if(err<0)
	      {
	          /* Fehlerterm wieder positiv (>=0) machen */
	          err += el;
	          /* Schritt in langsame Richtung, Diagonalschritt */
	          x += ddx;
	          y += ddy;
	      } else
	      {
	          /* Schritt in schnelle Richtung, Parallelschritt */
	          x += pdx;
	          y += pdy;
	      }
	      lcd_grf_SetPixel((unsigned char) x, (unsigned char) y, val);
	      //SetPixel(x,y);
	   }

	return 0;

}



/*! ######################################################################
 * @brief		Zeichnet ein Rechteck
 *
 * @details		Zeichnet ein Rechteck. Die Linie kann
 *              schwarz oder weiss sein.
 * 				Alle Plot-Befehle bearbeiten nur den Grafikspeicher. Die
 *              Aenderungen muessen mit lcd_grf_UpdateGraphicScreen()
 *              explizit sichtbar gemacht werden. Liegt ein Pixel ausserhalb
 *              des darstellbaren Bereiches, so wird nichts gezeichnet.
 *
 *
 * @see
 * @arg         lcd_grf_DrawLine(), lcd_grf_DrawRectFilled(), lcd_grf_UpdateGraphicScreen()
 *
 * @param		x0: x-Koordinate linke obere Ecke 0..GRF_COLS_X-1
 * @param       y0: y-Koordinate linke obere Ecke 0..GRF_ROWS_Y-1
 * @param		width : Breite in Pixel
 * @param       height: Hoehe in Pixel
 * @param       val: 0 zeichnet eine weisse Linie, sonst schwarz
 *
 *
 * @return		0: ok
 * @return      -1: Grafikspeicher nicht initialisiert
 * ###################################################################### */
int lcd_grf_DrawRect(unsigned char x0, unsigned char y0,
		             unsigned char width, unsigned char height,
		             unsigned char val)
{
	if(NULL==_grf_mem_pt)
	{ 	return -1;
	}
	// horizontale Linie oben
	lcd_grf_DrawLine(x0, y0, x0+width, y0, val);
	// vertikale Linie links
	lcd_grf_DrawLine(x0, y0, x0, y0+height, val);
	// vertikale Linie rechts
	lcd_grf_DrawLine(x0+width, y0, x0+width, y0+height, val);
	// horizontale Linie unten
	lcd_grf_DrawLine(x0, y0+height, x0+width, y0+height, val);
	return 0;
}



/*! ######################################################################
 * @brief		Zeichnet ein gefuelltes Rechteck
 *
 * @details		Zeichnet ein Rechteck. Das Rechteck kann
 *              schwarz oder weiss sein.
 * 				Alle Plot-Befehle bearbeiten nur den Grafikspeicher. Die
 *              Aenderungen muessen mit lcd_grf_UpdateGraphicScreen()
 *              explizit sichtbar gemacht werden. Liegt ein Pixel ausserhalb
 *              des darstellbaren Bereiches, so wird nichts gezeichnet.
 *
 *
 * @see
 * @arg         lcd_grf_DrawLine(), lcd_grf_DrawRect(), lcd_grf_UpdateGraphicScreen()
 *
 * @param		x0: x-Koordinate linke obere Ecke 0..GRF_COLS_X-1
 * @param       y0: y-Koordinate linke obere Ecke 0..GRF_ROWS_Y-1
 * @param		width : Breite in Pixel
 * @param       height: Hoehe in Pixel
 * @param       val: 0 zeichnet ein weisses Recheck, sonst schwarz
 *
 *
 * @return		0: ok
 * @return      -1: Grafikspeicher nicht initialisiert
 * ###################################################################### */
int lcd_grf_DrawRectFilled(unsigned char x0, unsigned char y0,
		             unsigned char width, unsigned char height,
		             unsigned char val)
{   unsigned char y;
	if(NULL==_grf_mem_pt)
	{ 	return -1;
	}
	for(y=y0; y<=y0+height; y++)
	{	lcd_grf_DrawLine(x0, y, x0+width, y, val);
	}
	return 0;
}


/*! ######################################################################
 * @brief		Zeichnet einen Kreis
 *
 * @details		Zeichnet einen Kreis. Der Kreis kann
 *              schwarz oder weiss sein.
 * 				Alle Plot-Befehle bearbeiten nur den Grafikspeicher. Die
 *              Aenderungen muessen mit lcd_grf_UpdateGraphicScreen()
 *              explizit sichtbar gemacht werden. Liegt ein Pixel ausserhalb
 *              des darstellbaren Bereiches, so wird nichts gezeichnet.
 *
 *
 * @see
 * @arg         lcd_grf_DrawLine(), lcd_grf_DrawRect(), lcd_grf_UpdateGraphicScreen()
 *
 * @param		x0: x-Koordinate Mittelpunkt 0..GRF_COLS_X-1
 * @param       y0: y-Koordinate Mittelpunkt 0..GRF_ROWS_Y-1
 * @param		radius : Kreisradius
 * @param       val: 0 zeichnet weiss, sonst schwarz
 *
 *
 * @return		0: ok
 * @return      -1: Grafikspeicher nicht initialisiert
 * ###################################################################### */
int lcd_grf_DrawCircle(unsigned char x0, unsigned char y0,
		               unsigned char radius, unsigned char val)
{	int f = 1 - radius;
	int ddF_x = 0;
	int ddF_y = -2 * radius;
	int x = 0;
	int y = radius;

	if(NULL==_grf_mem_pt)
	{ 	return -1;
	}

	lcd_grf_SetPixel(x0, y0+radius, val);  //    setPixel(x0, y0 + radius);
	lcd_grf_SetPixel(x0, y0-radius, val);  //    setPixel(x0, y0 - radius);
	lcd_grf_SetPixel(x0+radius, y0, val);  //    setPixel(x0 + radius, y0);
	lcd_grf_SetPixel(x0-radius, y0, val);  //    setPixel(x0 - radius, y0);

	while(x < y)
	{
		if(f >= 0)
	    {
	        y--;
	        ddF_y += 2;
	        f += ddF_y;
	    }
	    x++;
	    ddF_x += 2;
	    f += ddF_x + 1;

	    lcd_grf_SetPixel(x0 + x, y0 + y, val);  //    setPixel(x0 + x, y0 + y);
	    lcd_grf_SetPixel(x0 - x, y0 + y, val);  //    setPixel(x0 - x, y0 + y);
	    lcd_grf_SetPixel(x0 + x, y0 - y, val);  //    setPixel(x0 + x, y0 - y);
	    lcd_grf_SetPixel(x0 - x, y0 - y, val);  //    setPixel(x0 - x, y0 - y);
	    lcd_grf_SetPixel(x0 + y, y0 + x, val);  //    setPixel(x0 + y, y0 + x);
	    lcd_grf_SetPixel(x0 - y, y0 + x, val);  //    setPixel(x0 - y, y0 + x);
	    lcd_grf_SetPixel(x0 + y, y0 - x, val);  //    setPixel(x0 + y, y0 - x);
	    lcd_grf_SetPixel(x0 - y, y0 - x, val);  //    setPixel(x0 - y, y0 - x);
	}
	return 0;
}




/*! ######################################################################
 * @brief		Zeichnet einen gefuellten Kreis
 *
 * @details		Zeichnet gefuellten einen Kreis. Der Kreis kann
 *              schwarz oder weiss sein.
 * 				Alle Plot-Befehle bearbeiten nur den Grafikspeicher. Die
 *              Aenderungen muessen mit lcd_grf_UpdateGraphicScreen()
 *              explizit sichtbar gemacht werden. Liegt ein Pixel ausserhalb
 *              des darstellbaren Bereiches, so wird nichts gezeichnet.
 *
 *
 * @see
 * @arg         lcd_grf_DrawCircle(), lcd_grf_DrawRect(), lcd_grf_UpdateGraphicScreen()
 *
 * @param		x0: x-Koordinate Mittelpunkt 0..GRF_COLS_X-1
 * @param       y0: y-Koordinate Mittelpunkt 0..GRF_ROWS_Y-1
 * @param		radius : Kreisradius
 * @param       val: 0 zeichnet weiss, sonst schwarz
 *
 *
 * @return		0: ok
 * @return      -1: Grafikspeicher nicht initialisiert
 * ###################################################################### */
int lcd_grf_DrawCircleFilled(unsigned char x0, unsigned char y0,
		               unsigned char radius, unsigned char val)
#if 0    // die naive Version, fuellt nicht komplett...
{   unsigned char r;

	if(NULL==_grf_mem_pt)
	{ 	return -1;
	}

	for(r=0; r<=radius; r++)
	{
		lcd_grf_DrawCircle(x0, y0, r, val);
	}
	return 0;
}
#else
{	int f = 1 - radius;
	int ddF_x = 0;
	int ddF_y = -2 * radius;
	int x = 0;
	int y = radius;

	if(NULL==_grf_mem_pt)
	{ 	return -1;
	}

	lcd_grf_DrawLine(x0, y0+radius, x0, y0-radius, val);
	//lcd_grf_SetPixel(x0, y0+radius, val);  //    setPixel(x0, y0 + radius);
	//lcd_grf_SetPixel(x0, y0-radius, val);  //    setPixel(x0, y0 - radius);

	lcd_grf_DrawLine(x0+radius, y0, x0-radius, y0, val);
	//lcd_grf_SetPixel(x0+radius, y0, val);  //    setPixel(x0 + radius, y0);
	//lcd_grf_SetPixel(x0-radius, y0, val);  //    setPixel(x0 - radius, y0);

	while(x < y)
	{
		if(f >= 0)
	    {
	        y--;
	        ddF_y += 2;
	        f += ddF_y;
	    }
	    x++;
	    ddF_x += 2;
	    f += ddF_x + 1;

	    lcd_grf_DrawLine(x0 + x, y0 + y, x0 - x, y0 + y, val);
	    //lcd_grf_SetPixel(x0 + x, y0 + y, val);  //    setPixel(x0 + x, y0 + y);
	    //lcd_grf_SetPixel(x0 - x, y0 + y, val);  //    setPixel(x0 - x, y0 + y);

	    lcd_grf_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, val);
	    //lcd_grf_SetPixel(x0 + x, y0 - y, val);  //    setPixel(x0 + x, y0 - y);
	    //lcd_grf_SetPixel(x0 - x, y0 - y, val);  //    setPixel(x0 - x, y0 - y);

	    lcd_grf_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, val);
	    //lcd_grf_SetPixel(x0 + y, y0 + x, val);  //    setPixel(x0 + y, y0 + x);
	    //lcd_grf_SetPixel(x0 - y, y0 + x, val);  //    setPixel(x0 - y, y0 + x);

	    lcd_grf_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, val);
	    //lcd_grf_SetPixel(x0 + y, y0 - x, val);  //    setPixel(x0 + y, y0 - x);
	    //lcd_grf_SetPixel(x0 - y, y0 - x, val);  //    setPixel(x0 - y, y0 - x);
	}
	return 0;
}
#endif




/*! ######################################################################
 * @brief		Zeichnet eine Zeichenkette
 *
 * @details		Zeichnet eine Zeichenkette. Die Zeichen koennen
 *              schwarz oder weiss sein. Das Zeichnen eine Zeichkette
 *              schreibt in den Grafikspeicher und ist langsamer als
 *              die normale Textausgabe!
 * 				Alle Plot-Befehle bearbeiten nur den Grafikspeicher. Die
 *              Aenderungen muessen mit lcd_grf_UpdateGraphicScreen()
 *              explizit sichtbar gemacht werden. Liegt ein Pixel ausserhalb
 *              des darstellbaren Bereiches, so wird nichts gezeichnet.
 *
 *
 * @see
 * @arg         lcd_grf_DrawCircle(), lcd_grf_DrawRect(), lcd_grf_UpdateGraphicScreen()
 *
 * @param		string : Zeiger auf eine Zeichenkette
 * @param		x0: x-Koordinate links oben 0..GRF_COLS_X-1
 * @param       y0: y-Koordinate links oben 0..GRF_ROWS_Y-1
 * @param       val: 0 zeichnet weiss, sonst schwarz
 *
 *
 * @return		0: ok
 * @return      -1: Grafikspeicher nicht initialisiert
 * ###################################################################### */
int lcd_grf_PutString( const char * string,
		                    unsigned char x0, unsigned char y0,
		                    unsigned char val )
{

	unsigned char i, x, font_byte_number, bit, invers_val;

	if(NULL==_grf_mem_pt)
	{ 	return -1;
	}

	// Zeichenfarbe
	if(val!=0)
	{	invers_val=0;
	}
	else
	{	invers_val=1;
	}

    x = x0;
	for ( i = 0; string[ i ]; i++ )
	{
        // das Zeichen besteht aus mehreren bytes
		for(font_byte_number=0; font_byte_number<fontWIDTH; font_byte_number++, x++)
		{   // alle bits dieses bytes untersuchen, ggf Pixel plotten
			for(bit=0; bit<8; bit++)
			{
				if((font[string[i]][font_byte_number] & (1<<bit)) != 0)
				{
					lcd_grf_SetPixel(x, y0+bit, val);
				}
				else
				{
					lcd_grf_SetPixel(x, y0+bit, invers_val);
				}
			}

		}
	}

	return 0;
}





