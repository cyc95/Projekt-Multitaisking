/*!
 *******************************************************************************
 *
 * @file		font.h
 *
 * @brief		C-Headerdatei des Moduls font. Eine Schriftart für das
 * 				Modul lcd.
 *
 * @author		Christopher Zink
 *
 * @date		Juni 2015
 *
 * @version		1.0
 *
 * @par			Installation einer weiteren Schriftart
 * 				Um eine weitere Schriftart zu installieren müssen
 * 				Sie in dieser Datei (font.h) einen weiteren define
 * 				a la <tt>#define fontUSE_FONT_<here-goes-your-font-name></tt>
 * 				vorsehen und die übrigen auskommentieren. Anschließend müssen
 * 				Sie die Konstante #fontWIDTH auf die Breite ihrer
 * 				Schriftart ändern und in font.c die Daten der Schriftart
 * 				in #font[][] hinterlegen. Beachten Sie hierbei, dass
 * 				eine Schriftart, die höher als 8 Pixel ist ohne weitere
 * 				Änderungen zu Problemen führt. Beachten Sie bei der
 * 				Datenhinterlegung in #font[][] außerdem die
 * 				Pixeladressierung des LCD. Eine Möglichkeit diese
 * 				nachzuvollziehen ist der Aufruf von #lcdPrintTestPattern( ).
 *
 *******************************************************************************
 */










/* Mehrfachinkludeschutz */
#ifndef __FONT__
#define __FONT__










/*
################################################################################
#                                Modulsteuerung                                #
################################################################################
 */

/*!
 * Mit diesem/n <tt>define</tt> können Sie
 * steuern welche Schriftart verwendet
 * werden soll.
 *
 * @note	Stellen Sie stets sicher, dass immer genau einer der
 * 			der Defines nicht auskommentiert ist.
 */
//#define fontUSE_FONT_4X6
//#define fontUSE_FONT_5X8
//#define fontUSE_FONT_6X8
#define fontUSE_FONT_8X8










/*
################################################################################
#                          Validierung der Einstellungen                       #
################################################################################
 */

/* Das keine Schriftart definiert ist, muss hier abgefangen werden.
   Die fälschliche Definition mehr als einer Schriftart ist durch
   die Formulierung des Codes bereits abgefangen. Es wird immer der
   erste in der Aufzählung ab Zeile 68 die übrigen verdrängen. */

#if !defined( fontUSE_FONT_4X6 ) && !defined( fontUSE_FONT_5X8 ) && \
	!defined( fontUSE_FONT_6X8 ) && !defined( fontUSE_FONT_8X8 )
#error "You must select (exact) one font in font.h!"
#endif










/*
################################################################################
#                                  Konstanten                                  #
################################################################################
 */

/*!
 * Das ist die Breite der aktuell
 * verwendeten Schriftart in Pixeln.
 */
#if   defined( fontUSE_FONT_4X6 )
#define fontWIDTH		4
#elif defined( fontUSE_FONT_5X8 )
#define fontWIDTH		5
#elif defined( fontUSE_FONT_6X8 )
#define fontWIDTH		6
#elif defined( fontUSE_FONT_8X8 )
#define fontWIDTH		8
#endif










/*
################################################################################
#                              exportierte Daten                               #
################################################################################
 */

/* In font.c implementiert und dokumentiert/ kommentiert. */

extern const unsigned char font[ 256 ][ fontWIDTH ];










#endif /* ifndef __FONT__ */
