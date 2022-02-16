/*!
 * \file poll_serial_interface.h
 * \brief Routinen fuer die serielle Kommunikation auf renesas Controller
 *
 *   @par Author:     Ernst Forgber (Fgb)
 *
 ***********************************************************************
 *
 *   @par History       :
 *   @verbatim
 * Ver  Date        Author            Change Description
 * 0.0  13.10.2015  E. Forgber        - First Version
 *
 *   @endverbatim
 ****************************************************************************/
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
#ifndef POLL_SERIAL_INTERFACE_H_
#define POLL_SERIAL_INTERFACE_H_

#include "cos_types.h"

/*********************************************************************
 * Die Implementierung der Funktionen liegt in 'read.c'. Die ISR der
 * seriellen Schnittstelle bedient dort ein lokales flag, dass hier benutzt wird.
 */

void _initSerialInterface_RX_Interrupt(void);
int16_t _pollSerialInterface(void);


#endif /* POLL_SERIAL_INTERFACE_H_ */
