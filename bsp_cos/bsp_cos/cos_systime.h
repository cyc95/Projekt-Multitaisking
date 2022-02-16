/*!
 ********************************************************************
   @file            cos_systime.h

   @brief  Dieses Modul benutzt CMT0 als Systemuhr auf renesas contoller.


   @par Author    : Ernst Forgber (Fgb)
 ********************************************************************

   @par History   :
   @verbatim
   Version | Date        | Author        | Change Description
   0.0     | 03.04. 2013 | Fgb           | First Version
   0.1     | 08.10. 2015 | Fgb           | Umbau auf renesas controller

   @endverbatim

 ********************************************************************/
/**************************************************************************
    Copyright 2014 Ernst Forgber

    Dieses Programm ist Freie Software: Sie k�nnen es unter den Bedingungen
    der GNU General Public License, wie von der Free Software Foundation,
    Version 3 der Lizenz oder (nach Ihrer Wahl) jeder neueren
    ver�ffentlichten Version, weiterverbreiten und/oder modifizieren.

    Dieses Programm wird in der Hoffnung, dass es n�tzlich sein wird, aber
    OHNE JEDE GEW�HRLEISTUNG, bereitgestellt; sogar ohne die implizite
    Gew�hrleistung der MARKTF�HIGKEIT oder EIGNUNG F�R EINEN BESTIMMTEN ZWECK.
    Siehe die GNU General Public License f�r weitere Details.

    Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
    Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.

*****************************************************************************/



#ifndef avr_systime_h_
#define avr_systime_h_

//#include <avr/io.h>
//#include <avr/interrupt.h>
//#include <stdint.h>
#include "cos_types.h"

#define set_bit(sfr,bit)    sfr |= (1<<(bit))
#define clear_bit(sfr,bit)  sfr &= ~(1<<(bit))


void     _initSystemTime(void);
uint16_t _microSecPerTick(void);
uint16_t _gettime_Ticks(void);
uint16_t _milliSecToTicks(uint16_t milliSec);


#endif




