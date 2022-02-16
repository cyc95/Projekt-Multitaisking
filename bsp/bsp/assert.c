/*!
 *******************************************************************************
 *
 * @file		assert.c
 *
 * @brief		Implementierung der Funktion
 * 				<tt>
 * 				void
 * 				__assert( const char * file , int line , const char * expr )
 * 				</tt>.
 *
 * @author		Christopher Zink
 *
 * @date		Juni 2015
 *
 * @version		1.0
 *
 *******************************************************************************
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "bsp.h"

#if USED_LIB == OPTLIB
/*!
 * @brief		Wird von assert( ) aufgerufen, wenn
 * 				die Assertion fehlschlägt.
 *
 * @details		Diese Funktion habe ich überschrieben,
 * 				da das Original printf( ) oder fprintf( )
 * 				(an die Bibliotheksquellen von optlib kommt man
 * 				nicht ran) und Formatspecifier
 * 				verwendet, was (auf Grund der variablen
 * 				Parameterliste?) zu eigenartigem
 * 				Verhalten und vor allem unlesbaren
 * 				Ausgaben führte. Außerdem kann man
 * 				so bestimmen, was im Falle einer
 * 				fehlgeschlagenen Assertion passieren soll.
 *
 * @param		file		Name der Datei, in der die
 * 							Assertion fehlschlug
 * 							(das ist <tt>__FILE__</tt>).
 * @param		line		Zeilennummer der Datei, in
 * 							der die Assertion fehlschlug
 * 							(das ist <tt>__LINE__</tt>).
 * @param		expr		Die fehlgeschlagene Assertion
 * 							als Zeichenkette.
 *
 * @see			<Pfad-Zu-GNURX>/rx-elf/ex-elf/rx-elf/optlibinc/assert.h
 */
void __assert( const char * file, int line, const char * expr )
{

	int powOf10;
	int digit;
	int firstNonZeroDigitPassed;

	puts( "Assertion failed\r" );
	puts( "================\r" );
    puts( "\r" );

	puts( "File\r" );
	puts( file ? file : "NULL" );
	puts( "\r" );

	puts( "Line\r" );
	/* Ich denke nicht, dass __assert( ) mit line == 0
	   oder gar line < 0 aufgerufen wird. Dann müsste
	   __LINE__ == 0 || __LINE__ < 0 || __LINE__ > 2147483647
	   mal wahr gewesen sein!? */
	for (
		  powOf10 = 1000000000, digit = line / powOf10, firstNonZeroDigitPassed = 0;
          powOf10 >= 1;
          line -= digit * powOf10, powOf10 /= 10, digit = line / powOf10
		)
	{
		if ( !firstNonZeroDigitPassed && digit != 0 )
		{
			firstNonZeroDigitPassed = 1;
		}
		if ( firstNonZeroDigitPassed )
		{
			putchar( '0' + digit );
		}
	}
	puts( "\r\r\n" );

	puts( "Failed expression\r" );
	puts( expr ? expr : "NULL" );
	putchar( '\r' );

	exit( EXIT_FAILURE );

}
#endif

/* -------------------------------------------------------------------------- */
