/*
 * cos_types.h
 *
 *  Created on: 08.10.2015
 *      Author: Forgber
 */


/*!
 ********************************************************************
   @file            cos_types.h
   @par Project   : co-operative scheduler (COS) renesas uC
   @par Module    : Typdefinitionen COS Scheduler

   @brief  Typdefinitionen fuer COS auf renesas RX63N.


   @par Author    : Fgb
   @par Department: E14
   @par Company   : Fachhochschule Hannover - University of Applied
                    Sciences and Arts


   @par Beschreibung
   Dieses Modul stellt die Grunddatentypen mit besser lesbaren Namen zur Verfügung

   @par History   :
   @verbatim
   Version | Date        | Author        | Change Description
   0.0     | 08.10. 2015 | Fgb           | erzeugt auf renesas controller
   @endverbatim

 ********************************************************************/
/**************************************************************************
    Copyright 2014 Ernst Forgber

    Dieses Programm ist Freie Software: Sie können es unter den Bedingungen
    der GNU General Public License, wie von der Free Software Foundation,
    Version 3 der Lizenz oder (nach Ihrer Wahl) jeder neueren
    veröffentlichten Version, weiterverbreiten und/oder modifizieren.

    Dieses Programm wird in der Hoffnung, dass es nützlich sein wird, aber
    OHNE JEDE GEW�HRLEISTUNG, bereitgestellt; sogar ohne die implizite
    Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
    Siehe die GNU General Public License fÜr weitere Details.

    Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
    Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.

*****************************************************************************/







#ifndef COS_TYPES_H_
#define COS_TYPES_H_



/* for compatibility with gcc types: */
#ifndef int8_t
  #define int8_t signed char
#endif
#ifndef uint8_t
  #define uint8_t unsigned char
#endif
#ifndef int16_t
  #define int16_t signed short
#endif
#ifndef uint16_t
  #define uint16_t unsigned short
#endif
#ifndef int32_t
  #define int32_t signed long
#endif
#ifndef uint32_t
  #define uint32_t unsigned long
#endif




#endif /* COS_TYPES_H_ */
