/*!
 ********************************************************************
   @file            cos_data_fifo.h
   @par Project   : co-operative scheduler (COS) renesea uC
   @par Module    : FIFO Mailbox fuer COS Scheduler

   @brief  Daten-FIFO fuer COS auf Atmel.
          Der FIFO benutzt dynamische Speicherverwaltung (malloc()).
          Es kann nur ein Sorte Daten gespeichert werden.
          Die maximale Anzahl von Speicherplaetzen (Slots) ist 255 wegen
          der Verwendung von 1 Byte Indexvariablen. Ein Slot kann maximal
          256 Byte Speichern.


   @par Author    : Fgb
   @par Department: E14
   @par Company   : Fachhochschule Hannover - University of Applied
                    Sciences and Arts

 ********************************************************************

   @par History   :
   @verbatim
   Version | Date        | Author        | Change Description
   0.0     | 07.09. 2011 | Fgb           | First Version, Linux
   0.1     | 17.09. 2013 | Fgb           | nur noch Atmel, deutsche Doku
   0.2     | 08.10. 2015 | Fgb           | Umbau auf renesas controller

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

#ifndef _cos_data_fifo_h_
#define _cos_data_fifo_h_



#include "cos_semaphore.h"

//#include <avr/io.h>
//#include <stdint.h>
#include "cos_types.h"

#ifndef NULL
    #define NULL 0  /* the null pointer value */
#endif


/***********************************************
 * FIFO data structure :
 ***********************************************/
typedef struct {
        char *buffer;        /*!< Queue Datenpuffer */
        uint8_t maxSlots;      /*!< Gesamtzahl der Slots in der Queue  */
        uint8_t slotSize;      /*!< Groesse eines Slot in Byte */
        uint8_t rIndex;        /*!< Lese-Index des Puffers */
        uint8_t wIndex;        /*!< Schrieb-Index des Puffers */
        uint8_t usedSlots;     /*!< Anzahl der benutzten Slots */
        uint8_t isInitialized; /*!< 0 falls noch nicht initialisiert */
        CosSema_t rSema;       /*!< wartet an diesem Semaphore beim Lesen */
        CosSema_t wSema;       /*!< wartet an diesem Semaphore beim Schreiben */
} CosFifo_t;




uint8_t COS_FifoCreate(CosFifo_t *q, uint8_t slotSize, uint8_t nSlots);
uint8_t COS_FifoDestroy(CosFifo_t *q);
int8_t COS_FifoIsEmpty(CosFifo_t *q);
int8_t COS_FifoIsFull(CosFifo_t *q);

uint8_t COS_FifoGetUsedSlots(CosFifo_t *q);
uint8_t COS_FifoGetMaxSlots(CosFifo_t *q);
uint8_t COS_FifoGetSlotSize(CosFifo_t *q);

int8_t _qWriteSingleSlot(CosFifo_t *q, const char *data);
int8_t _qReadSingleSlot(CosFifo_t *q, char *data);

// blockierende Macros

/*!
 **********************************************************************
 * @par Beschreibung:
    Dieses Macro versucht einen Slot des FIFO mit Daten zu fuellen.
    Die Anzahl der zu schreibenden Bytes ist 'slotSize', wie in der
    Funktion  COS_FifoCreate() angegeben.
    Falls kein freier Slot verfuegbar ist, schaltet das Macro die Task
    *pt in den Zustand TASK_STATE_BLOCKED. Eine andere Task, die
    einen Slot aus dem FIFO liest, wird die Blockierung wieder aufheben.
 *
 * @see
 * @arg  COS_FifoBlockingReadSingleSlot(), _qWriteSingleSlot()
 *
 * @par Macro Parameter: (CosTask_t *pt, CosFifo_t *q,  char *data)
 *
 * @param  pt              - IN/OUT, Zeiger auf Task struct
 * @param  q               - IN/OUT, Zeiger auf Queue struct
 * @param  data            - IN, Zeiger auf Datenquelle
 * @retval void
  * @par Example :
 *    A global FIFO with 5 slots for float variables and a second queue with 2
 *    slots for int arrays with 5 elements each  may be created, used
 *    and destroyed like this:
 * @verbatim
CosFifo_t q01, q02;
CosTask_t *p_Task_A, *p_Task_B;

void Task_A(CosTask_t *pt)
{   static float x,y;
    static int8_t a[5];

    COS_TASK_BEGIN(pt);
    ...
    COS_FifoBlockingWriteSingleSlot(pt, &q01,&x);
    COS_FifoBlockingWriteSingleSlot(pt, &q01,&y);
    COS_FifoBlockingWriteSingleSlot(pt, &q02, a);
    ...
    COS_TASK_END(pt);
}

void Task_B(CosTask_t *pt)
{   static float  z[2];
    static int8_t b[5];

    COS_TASK_BEGIN(pt);
    ...
    COS_FifoBlockingReadSingleSlot(pt, &q02, b);
    COS_FifoBlockingReadSingleSlot(pt, &q01,&z[0]);
    COS_FifoBlockingReadSingleSlot(pt, &q01,&z[1]);
    ...
    COS_TASK_END(pt);
}

int main(void)
{ ...
  COS_InitTaskList();
  p_Task_A = COS_CreateTask(5, NULL, Task_A);
  p_Task_B = COS_CreateTask(7, NULL, Task_B);
  ...
  if(0!= COS_FifoCreate(&q01, sizeof(float),5))
    serPuts("error creating queue");
  if(0!= COS_FifoCreate(&q02, 5*sizeof(int8_t),2))
    serPuts("error creating queue");
  ...
  COS_RunScheduler();
  while(1);    // this should not be reached...
  return 0;
}
  @endverbatim
 ************************************************************************/
#define COS_FifoBlockingWriteSingleSlot(pt, q,  data)  COS_SEM_WAIT(&((q)->wSema),(pt)); \
                                                       _qWriteSingleSlot((q), (char *)(data))



/*!
 **********************************************************************
 * @par Beschreibung:
    Dieses Macro versucht einen Slot Daten aus dem FIFO zu lesen.
    Die Anzahl der zu schreibenden Bytes ist 'slotSize', wie in der
    Funktion  COS_FifoCreate() angegeben.
    Falls das FIFO leer ist, schaltet das Macro die Task *pt in den
    Zustand TASK_STATE_BLOCKED. Eine andere Task, die in das FIFO
    schreibt, loest die Blockade wieder.
 *
 * @see
 * @arg  COS_FifoBlockingWriteSingleSlot(), _qReadSingleSlot()
 *
 * @par Macro Parameter: (CosTask_t *pt, CosFifo_t *q,  char *data)
 *
 * @param  pt              - IN/OUT, Zeiger auf Task struct
 * @param  q               - IN/OUT, Zeiger auf Queue struct
 * @param  daCOS_FifoBlockingWriteSingleSlotta            - IN/OUT, Zeiger auf Datenquelle
 * @retval void
  * @par Example :
  Siehe bei COS_FifoBlockingWriteSingleSlot()
 * @verbatim
  @endverbatim
 ************************************************************************/
#define COS_FifoBlockingReadSingleSlot(pt, q,  data)   COS_SEM_WAIT(&((q)->rSema),(pt)); \
                                                       _qReadSingleSlot((q), (char *)(data))

#endif



