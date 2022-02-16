/*!
 ********************************************************************
   @file            cos_scheduler.h
   @par Project   : co-operative Scheduler renesas uC
   @par Module    : Task Liste und Scheduling Implementierung

   @brief  Renesas Version des 'co-operative' Tasking Systems (COS)

   Task-Wechsel werden durch spezielle Macros erreicht. Diese Macros
   duerfen nur aus der Task-Funktion direkt aufgerufen werden, nicht
   aus tiefer verschachtelten Funktionen heraus.

   @par Author    : E. Forgber (Fgb)
   @par Department: E14
   @par Company   : Fachhochschule Hannover - University of Applied
                    Sciences and Arts


 ********************************************************************

   @par History   :
   @verbatim
   Version | Date        | Author          | Change Description
   0.0     | 21.03. 2011 | Fgb             | First Version, Linux
   0.1     | 01.11. 2011 | E.Forgber (Fgb) | Migration to Atmel uC
   0.2     | 17.09. 2013 | Fgb             | nur noch Atmel, deutsche Doku.
   0.3     | 08.10. 2015 | Fgb             | Umbau auf renesas controller

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



#ifndef _cos_scheduler_h_
#define _cos_scheduler_h_


#include "cos_systime.h"
#include "cos_linear_task_list.h"


int8_t COS_InitTaskList(void);
CosTask_t* COS_CreateTask(uint8_t prio, void * pData, void (*func) (CosTask_t *));
int8_t COS_DeleteTask(CosTask_t* task_pt);
int8_t COS_SuspendTask(CosTask_t* task_pt);
int8_t COS_ResumeTask(CosTask_t* task_pt);
int8_t COS_SetTaskPrio(CosTask_t* task_pt,uint8_t taskPrio);
int8_t COS_RunScheduler(void);


void COS_PrintTaskList(void);
int8_t COS_GetCPULoadInPercent(void);


/*-------------- macros for task start, end, scheduling ------------*/

/*!
********************************************************************
  @par Beschreibung
  Dieses Macro muss am Anfang jeder Task-Funktion stehen.

@parameter pt - IN, Zeiger auf Task-Stuktur

@returns  nichts

@par Code-Beispiel::
@verbatim
void my_cos_task_func(CosTask_t* task_pt)
{
    COS_TASK_BEGIN(task_pt);

    while(1)
    {   ...
        COS_TASK_SCHEDULE(task_pt);
        ...
    }
    COS_TASK_END(task_pt);
}
@endverbatim
********************************************************************/
#define COS_TASK_BEGIN(pt) switch((pt)->lineCnt) {case 0:


/*!
********************************************************************
  @par Beschreibung
  Dieses Macro ist ein kooperativer Scheduling-Punkt der Task-Funktion.
  An dieser Stelle der Task-Funktion wird der Scheduler aufgerufen und
  es kann zu einem Task-Wechsel kommen.

@parameter pt - IN, Zeiger auf Task-Stuktur

@returns  nichts

@par Code-Beispiel::
@verbatim
void my_cos_task_func(CosTask_t* task_pt)
{
    COS_TASK_BEGIN(task_pt);

    while(1)
    {   ...
        COS_TASK_SCHEDULE(task_pt);
        ...
    }
    COS_TASK_END(task_pt);
}
@endverbatim
********************************************************************/
#define COS_TASK_SCHEDULE(pt) (pt)->sleepTime_Ticks=0;\
                          (pt)->lineCnt=__LINE__;\
                          return;\
                          case __LINE__:




/*!
********************************************************************
  @par Beschreibung
  Dieses Macro ist ein kooperativer Scheduling-Punkt der Task-Funktion.
  Die Task wird fuer eine gewisse Zeit blockieren, der Scheduler wird
  aufgerufen und startet eine andere Task-Funktion.

@parameter pt - IN, Zeiger auf Task-Stuktur

@returns  nichts

@par Code-Beispiel::
@verbatim
void my_cos_task_func(CosTask_t* task_pt)
{
    COS_TASK_BEGIN(task_pt);

    while(1)
    {   ...
        COS_TASK_SLEEP(task_pt,100);
        ...
    }
    COS_TASK_END(task_pt);
}
@endverbatim
********************************************************************/
#define COS_TASK_SLEEP(pt,t_Ticks) (pt)->sleepTime_Ticks=(t_Ticks);\
                          (pt)->lineCnt=__LINE__;\
                          return;\
                          case __LINE__:







/*!
********************************************************************
  @par Beschreibung
  Dieses Macro muss am Ende jeder Task-Funktion aufgerufen werden.
  Es terminiert die Task und loescht sie aus der Task-Liste. Der
  Speicher der Task wird freigegeben. Um die Task erneut zu benutzen,
  muss sie neu erzeugt werden.


@see
@arg  COS_CreateTask(), COS_DeleteTask()

@parameter pt - IN, Zeiger auf Task-Stuktur

@returns  nichts

@par Code-Beispiel::
@verbatim
void my_cos_task_func(CosTask_t* task_pt)
{
    COS_TASK_BEGIN(task_pt);

    while(1)
    {   ...
        COS_TASK_SCHEDULE(task_pt);
        ...
    }
    COS_TASK_END(task_pt);
}
@endverbatim
********************************************************************/
#define COS_TASK_END(pt) }; (pt)->lineCnt=0; COS_DeleteTask((pt)); return;




#endif

