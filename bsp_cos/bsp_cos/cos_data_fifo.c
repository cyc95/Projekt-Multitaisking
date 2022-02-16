/*!
 ********************************************************************
   @file            cos_data_fifo.c
   @par Project   : co-operative scheduler (COS) renesas uC
   @par Module    : FIFO Mailbox fuer COS Scheduler

   @brief  Daten-FIFO fuer COS auf renesas RX63N.


   @par Author    : Fgb
   @par Department: E14
   @par Company   : Fachhochschule Hannover - University of Applied
                    Sciences and Arts


   @par Beschreibung
   Dieses Modul stellt ein einfaches FIFO zum Datentransfer fuer Tasks des
   COS Tasking-Systems bereit. Ein FIFO besteht aus mehreren Daten Slots
   mit je maximal 255 Byte.

   @verbatim

 |---------|            -----------            |---------|
 | producer|-----------> |  |  |  | ---------->| consumer|
 |---------|            -----------            |---------|
                           FIFO
   @endverbatim
 ********************************************************************

   @par History   :
   @verbatim
   Version | Date        | Author        | Change Description
   0.0     | 08.09. 2011 | Fgb           | First Version, Linux
   0.1     | 17.09. 2013 | Fgb           | nur noch Atmel, deutsche Doku.
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



#include <stdlib.h>
#include <string.h>  // for memcpy()
#include "cos_ser.h"
#include "cos_data_fifo.h"




/*------------- DEBUGGING ---------------------------------*/
#define DEBUG_MODULE 1

#if DEBUG_MODULE
  #define DebugCode( code_fragment ) { code_fragment }
#else
  #define DebugCode( code_fragment )
#endif

#if DEBUG_MODULE
static void _msg(char *msg)
{
    DebugCode(serPuts(msg););
}
#endif
/*---------------------------------------------------------------*/



/*!
 **********************************************************************
 * @par Beschreibung:
    Diese Funktion erzeugt ein FIFO fuer den Datentransfer zwischen Tasks.
    Das FIFO hat nSlots Daten-Slots, jeweils slotSize Byte gross.
 *
 * @see
 * @arg  COS_FifoDestroy()
 *
 *
 * @param  q               - IN/OUT, Zeiger auf FIFO Struktur
 * @param  slotSize        - IN, Groesse Daten-Slot (1..255) in Byte
 * @param  nSlots          - IN, Anzahl Slots (1..255) im FIFO
 *
 * @retval 0               - kein Fehler
 * @retval negative        - Fehler
 *
 * @par Code-Beispiel :
 *    A global FIFO with 5 slots for float variables and a second queue with 2
 *    slots for int arrays with 5 elements each  may be created
 *    and destroyed like this:
 * @verbatim
CosFifo_t q01, q02;

int main(void)
{
  ...
  if(0!= COS_FifoCreate(&q01, sizeof(float),5))
    serPuts("error creating queue");
  if(0!= COS_FifoCreate(&q02, 5*sizeof(int8_t),2))
    serPuts("error creating queue");
  ...
  COS_FifoDestroy(&q01);
  COS_FifoDestroy(&q02);
  ...
  return 0;
}
  @endverbatim
 ************************************************************************/
uint8_t COS_FifoCreate(CosFifo_t *q, uint8_t slotSize, uint8_t nSlots)
{
  /* create buffer */
  q->buffer = (char *) malloc(slotSize * nSlots * sizeof(char));
  if(NULL == q->buffer)
  { DebugCode(_msg("FifoCreate:malloc!"););
    return -1;
  }
  q->maxSlots  = nSlots;
  q->slotSize  = slotSize;
  q->rIndex    = 0;         /* empty queue */
  q->wIndex    = 0;
  q->usedSlots = 0;
  if(0!= COS_SemCreate(&(q->rSema), 0))  // nothing to read yet
  {  DebugCode(_msg("FifoCreate:SemCreate!"););
     return -1;
  }
  if(0!= COS_SemCreate(&(q->wSema), nSlots)) // all slots are still free
  {  DebugCode(_msg("FifoCreate:SemCreate!"););
     return -1;
  }
  q->isInitialized = 1;
  return 0;
}



/*!
 **********************************************************************
 * @par Beschreibung:
    Diese Funktion loescht ein FIFO und gibt den Speicher frei.
 *
 * @see
 * @arg  COS_FifoCreate()
 *
 *
 * @param  q               - IN/OUT, Zeiger auf FIFO struct
 *
 * @retval 0               - kein Fehler
 * @retval negative        - Fehler
 *
 * @par Code-Beispiel :
 *   @see
 *   @arg COS_FifoCreate()
 *
 * @verbatim
  @endverbatim
 ************************************************************************/
uint8_t COS_FifoDestroy(CosFifo_t *q)
{
  if(q->isInitialized == 0)
  { DebugCode(_msg("FifoDestroy:not init."););
    return -1;
  }
  /* delete buffer */
  if(q->buffer != NULL)
  { free(q->buffer);
    q->buffer = NULL;
  }
  q->isInitialized = 0;
  COS_SemDestroy(&(q->rSema));
  COS_SemDestroy(&(q->wSema));
  return 0;
}



/*!
 **********************************************************************
 * @par Beschreibung:
    Diese Funktion ueberprueft, ob ein FIFO leer ist.
 *
 * @see
 * @arg  COS_FifoIsFull()
 *
 *
 * @param  q               - IN/OUT, Zeiger auf FIFO Struktur
 *
 * @retval 0               - FIFO nicht leer
 * @retval 1               - FIFO ist leer
 * @retval negative        - Fehler
 *
 * @par Code-Beispiel :
 * @verbatim
CosFifo_t q01;

void Task_A(CosTask_t *pt)
{
    COS_TASK_BEGIN(pt);
    ...
    if(!COS_FifoIsEmpty(&q01))
    {  // FIFO ist nicht leer
       ...
    }
    ...
    COS_TASK_END(pt);
}
  @endverbatim
 ************************************************************************/
int8_t COS_FifoIsEmpty(CosFifo_t *q)
{ int8_t retval=0;

  if(q->isInitialized == 0)
  { DebugCode(_msg("FifoIsEmpty:not init"););
    return -1;
  }
  if(q->usedSlots == 0)
    retval = 1;
  else
    retval = 0;
  return retval;
}



/*!
 **********************************************************************
 * @par Beschreibung:
    Diese Funktion prueft, ob ein FIFO voll ist.
 *
 * @see
 * @arg  COS_FifoIsEmpty()
 *
 *
 * @param  q               - IN/OUT, Zeiger auf FIFO Struktur
 *
 * @retval 0               - FIFO ist nicht voll
 * @retval 1               - FIFO ist voll
 * @retval negative        - Fehler
 *
 * @par Code-Beispiel :
 * @verbatim
CosFifo_t q01;

void Task_A(CosTask_t *pt)
{
    COS_TASK_BEGIN(pt);
    ...
    if(COS_FifoIsFull(&q01))
    {  // FIFO ist voll
       ...
    }
    ...
    COS_TASK_END(pt);
}

  @endverbatim
 ************************************************************************/
int8_t COS_FifoIsFull(CosFifo_t *q)
{ int8_t retval=0;

  if(q->isInitialized == 0)
  { DebugCode(_msg("FifoIsFull:not init"););
    return -1;
  }
  if(q->usedSlots == q->maxSlots)
    retval = 1;
  else
    retval = 0;
  return retval;
}





/*!
 **********************************************************************
 * @par Beschreibung:
    Diese Funktion kopiert einen Slot Daten ins FIFO.
    Die Anzahl der zu schreibenden Bytes ist 'slotSize', wie in der
    Funktion  COS_FifoCreate() angegeben. Sie wird vom Macro
    COS_FifoBlockingWriteSingleSlot() verwendet, im Anwenderprogramm
    SOLLTE SIE NUR UEBER DIESES MACRO genutzt werden!
    Falls das FIFO voll ist, wird nichts geschrieben.
 *
 * @see
 * @arg  _qReadSingleSlot(), COS_FifoBlockingWriteSingleSlot()
 *
 *
 * @param  q               - IN/OUT, Zeiger auf FIFO Struktur
 * @param  data            - IN, Zeiger auf Daten
 *
 * @retval Anzahl der geschriebenen Slots, 0 falls FIFO voll
 * @retval -1              - Fehler
 ************************************************************************/
int8_t _qWriteSingleSlot(CosFifo_t *q, const char *data)
{ int8_t retval = 0;
  if(q->isInitialized == 0)
  { DebugCode(_msg("_qWriteSingleSlot:not init"););
    return -1;
  }
  /* if FIFO is full, return 0 without writing any data */
  if(q->usedSlots >= q->maxSlots)
  { retval = 0;  /* FIFO is full, don't write */
  }
  else  /* at least one slot is free, write data */
  { retval = 1;
    memcpy(&(q->buffer[q->wIndex]), data, q->slotSize); /* copy to FIFO */
    q->wIndex += q->slotSize;                  /* next slot */
    q->wIndex %= (q->maxSlots * q->slotSize);  /* circular buffer */
    q->usedSlots  += 1;
    COS_SEM_SIGNAL(&(q->rSema));  // unblock tasks that wait for reading,
  }
  return retval;
}










/*!
 **********************************************************************
 * @par Beschreibung:
    Diese Funktion liest einen Slot aus dem FIFO.
    Die Anzahl der zu lesenden Bytes ist 'slotSize', wie in der
    Funktion  COS_FifoCreate() angegeben. Sie wird vom Macro
    COS_FifoBlockingReadSingleSlot() verwendet, im Anwenderprogramm
    SOLLTE SIE NUR UEBER DIESES MACRO genutzt werden.
    Falls das FIFO leer ist, wird nichts gelesen.
 *
 * @see
 * @arg  _qWriteSingleSlot(), COS_FifoBlockingReadSingleSlot()
 *
 *
 * @param  q               - IN/OUT, Zeiger auf FIFO Struktur
 * @param  data            - IN/OUT, Zeiger auf Daten
 *
 * @retval Anzahl der gelesenen Slots,  0 wenn FIFO leer
 * @retval -1              - Fehler
 ************************************************************************/
int8_t _qReadSingleSlot(CosFifo_t *q, char *data)
{ int8_t retval =0;
  if(q->isInitialized == 0)
  { DebugCode(_msg("_qReadSingleSlot:not init"););
    return -1;
  }
  /* if FIFO is empty, nothing is read */
  if (q->usedSlots <=0)
  {   retval = 0;
  }
  else /* at least one slot has data */
  {   retval = 1;
       memcpy(data, &(q->buffer[q->rIndex]), q->slotSize); /* read from queue */
       q->rIndex += q->slotSize;                  /* next slot to read */
       q->rIndex %= (q->maxSlots * q->slotSize);  /* circular buffer */
       q->usedSlots  -= 1;
       COS_SEM_SIGNAL(&(q->wSema));  // unblock tasks that wait for writing
  }
  return retval;
}






/*!
 **********************************************************************
 * @par Beschreibung:
    Diese Funktion gibt die Anzahl der gefuellten Slots des FIFO zurueck.

 *
 * @see COS_FifoGetMaxSlots(), COS_FifoGetSlotSize()
 * @arg  -
 *
 *
 * @param  q               - IN/OUT, Zeiger auf FIFO Struktur
 *
 * @retval Anzahl der belegten Slots
 ************************************************************************/
uint8_t COS_FifoGetUsedSlots(CosFifo_t *q)
{   return q->usedSlots;
}




/*!
 **********************************************************************
 * @par Beschreibung:
    Diese Funktion gibt die Anzahl der Slots des FIFO zurueck.
 *
 * @see  COS_FifoGetUsedSlots(), COS_FifoGetSlotSize()
 * @arg  -
 *
 *
 * @param  q               - IN/OUT, Zeiger auf FIFO Struktur
 *
 * @retval Anzahl der Slots im  FIFO
 ************************************************************************/
uint8_t COS_FifoGetMaxSlots(CosFifo_t *q)
{   return q->maxSlots;
}


/*!
 **********************************************************************
 * @par Beschreibung:
    Diese Funktion gibt die Groesse eines Slots des FIFO zurueck.
 *
 * @see
 * @arg  -
 *
 *
 * @param  q               - IN/OUT, Zeiger auf FIFO Struktur
 *
 * @retval Slot Groesse in Byte
 ************************************************************************/
uint8_t COS_FifoGetSlotSize(CosFifo_t *q)
{
  return q->slotSize;
}
















