/*!
 *******************************************************************************
 *
 * @file			lcd.h
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
   1.1     | 19.10.2016 | Fgb           | Grafik-Routinen angefangen
                                        | zureuckgesetzt, siehe dort
   @endverbatim
 *
 * @version			1.1
 *
 *
 *******************************************************************************
 */






/* Mehrfachinkludschutz */
#ifndef __LCD__
#define __LCD__










/*
################################################################################
#                                Modulsteuerung                                #
################################################################################
*/

/* Ein Teil der Modulsteuerung liegt in lcd.c um sie zu verstecken
   (bspw. soll nicht jeder der lcd.h einbindet lcdPID sehen). */










/*
################################################################################
#                                   Konstanten                                 #
################################################################################
*/

/*!
 * Das ist die Breite des LCD in Pixel.
 * Der Treiber ST7579 hat 102 Pixel pro
 * Zeile. Die Breite ist die Breite,
 * wenn die MAC-Adresse rechts neben
 * dem LCD liegt.
 */
#define lcdWIDTH							96

/*!
 * Das ist die Höhe des LCD in Pixel.
 * Der Treiber ST7579 hat 68 Pixel pro
 * Spalte. Die Höhe ist die Höhe,
 * wenn die MAC-Adresse rechts neben
 * dem LCD liegt.
 */
#define lcdHEIGHT							64

/*!
 * Diesen Wert gibt eine Modulfunktion zurück,
 * wenn sie ihre Aufgabe erfolgreich ausgeführt
 * hat.
 */
#define lcdERROR_NONE						0

/*!
 * Diesen Wert gibt eine Modulfunktion zurück,
 * die nicht vollständig implementiert ist.
 */
#define lcdERROR_UNSUPPORTED_OPERATION		-1

/*!
 * Diesen Wert gibt lcdInitialize( ) zurück,
 * wenn das Initialisieren des genutzten
 * SPI-Kanals fehlschlug.
 */
#define lcdERROR_SPI_INIT_FAILED			-2

/*!
 * Diesen Wert gibt lcdFinalize( ) zurück,
 * wenn das Deinitialisieren des genutzten
 * SPI-Kanals fehlschlug.
 */
#define lcdERROR_SPI_FINALIZE_FAILED		-3

/*!
 * Diesen Wert geben Modulfunktionen zurück,
 * die einen vorangegangenen Aufruf von
 * lcdInitialize( ) benötigen, dieser aber
 * nocht nicht ausgeführt wurde.
 */
#define lcdERROR_NOT_INITIALIZED			-4

/*!
 * Diesen Wert gibt eine Modulfunktion
 * zurück, wenn sie mit einem ungültigen
 * Argument aufgerufen wurde.
 */
#define lcdERROR_INVALID_ARGUMENT			-5










/*
################################################################################
#                   Prototypen der exportierten Funktionen                     #
################################################################################
*/

/* Diese Funktionen sind alle in lcd.c
   implementiert und kommentiert/dokumentiert. */

int lcdInitialize( void );
int lcdFinalize( void );
int lcdSetBackLight( int );
int lcdPrintTestPattern( void );
int lcdPutString( const char * );
int lcdClearScreen( void );

int lcdSetTextPosition(unsigned char row, unsigned char col);


int lcdPrintTestPattern_V2( void );


//#####  Grafik #########

int lcd_grf_Initialize( void );
int lcd_grf_Finalize( void );
int lcd_grf_PlotTestPattern( void );
int lcd_grf_PlotString( const char * str);
int lcd_grf_ClearGraphicScreen( void );
int lcd_grf_UpdateGraphicScreen( void );

int lcd_grf_SetPixel(unsigned char x, unsigned char y, unsigned char val);
int lcd_grf_DrawLine(unsigned char xstart, unsigned char ystart,
		             unsigned char xend,   unsigned char yend,
		             unsigned char val);
int lcd_grf_DrawRect(unsigned char x0, unsigned char y0,
		             unsigned char width, unsigned char height,
		             unsigned char val);
int lcd_grf_DrawRectFilled(unsigned char x0, unsigned char y0,
		             unsigned char width, unsigned char height,
		             unsigned char val);
int lcd_grf_DrawCircle(unsigned char x0, unsigned char y0,
		               unsigned char radius, unsigned char val);
int lcd_grf_DrawCircleFilled(unsigned char x0, unsigned char y0,
		               unsigned char radius, unsigned char val);
int lcd_grf_PutString( const char * string,
		               unsigned char x0, unsigned char y0,
		               unsigned char val );



#endif /* ifndef __LCD__ */
