/*!
 ********************************************************************
   @file            cos_semaphore.c
   @par Project   : co-operative Scheduler renesas uC
   @par Module    : Counting Semaphore fuer co-operative Scheduler (COS)

   @brief  Dies ist die Atmel Version der COS-Semaphoren


   @par Author    : Fgb
   @par Department: E14
   @par Company   : Fachhochschule Hannover - University of Applied
                    Sciences and Arts

 ********************************************************************

   @par History   :
   @verbatim
   Version | Date        | Author        | Change Description
   0.0     | 29.04. 2011 | Fgb           | First Version, Linux
   0.1     | 17.09. 2013 | Fgb           | nur noch Atmel, deutsche Doku.
   0.2     | 08.10. 2015 | Fgb           | renesas controller
   @endverbatim

@section Prinzip
Die Task blockiert nie, wenn COS_SEM_WAIT() zur Blockade fuehren soll,
dann muss die Taskfunktion verlassen werden und das naechste mal an
derselben Stelle weiterlaufen.
Der Zustand der Task wird dann auf TASK_STATE_BLOCKED gesetzt, der
Scheduler wird diese Task nicht mehr starten. Die Task wird in die
Liste der an diesem Sema wartenden Tasks eingetragen.

Eventuell sollte man die Liste nach Task-Prio sortieren.
Zunaechst wird das Problem von
Prioritaetsinversionen hier komplett ignoriert.


Erst wenn eine andere Task mit COS_SEM_SIGNAL()
ein Ereignis in den Semaphor legt, wird die erste Task in der Liste
der am Sema wartenden Tasks in den Zustand TASK_STATE_READY versetzt
und aus der Liste der wartenden Tasks entfernt.

  @verbatim

                          node
           root_pt       --------      --------           --------
           -----         |      |----->|      |-- ... --->|      |----> NULL
           |   |-------->|......|      |......|           |......|
           -----         |      |      |      |           |      |
                         --------      --------           --------
                            |             |                  |
                            \/            \/                 \/
                          -----         -----              -----
                          |   |         |   |              |   |
                          -----         -----              -----
                          task

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




#include "cos_semaphore.h"



/*!
********************************************************************
  @par Beschreibung
  Initialisiet den Semaphor mit dem gegebenen Zaehlerstand, erzeugt
  eine noch leere Liste von Tasks, die an diesem Semaphor warten.

@see
@arg

@param s       - IN/OUT, Zeiger auf Semaphore
@param n_start - IN, Zaehlerstand des Semaphore nach Initialisierung

@retval 0 fuer ok, negativ bei Fehler.

@par Code-Beispiel:
@verbatim
CosSema_t sema_1;
...
int main(void)
{   ...
    serPuts("hello");

    if(0!=COS_InitTaskList())
    {   serPuts("COS_InitScheduler has crashed...");
    }
    COS_PrintTaskList();

    COS_SemCreate(&sema_1, 0);


    tA = COS_CreateTask(1, NULL, myTask);
    tB = COS_CreateTask(2, NULL, myTask2);
    tC = COS_CreateTask(3, NULL, myTask3);

    if(0!=COS_RunScheduler())
    {   serPuts("COS_RunScheduler has crashed...");
    }

    getchar();
    return 0;
}
@endverbatim
********************************************************************/
uint8_t COS_SemCreate(CosSema_t *s, int8_t n_start)
{
    s->count = n_start;
    s->root_pt = NULL;
    return 0;
}







/*!
********************************************************************
  @par Beschreibung
  Loescht die Task-Liste dieses Semaphor. Die Tasks werden natuerlich
  nicht gelosecht!

@see
@arg

@param s       - IN/OUT, Zeiger auf Semaphore

@retval 0 fuer ok, negativ bei Fehler

@par Code-Beispiel:
@verbatim
CosSema_t sema_1;
...
int main(void)
{   ...
    serPuts("hello");

    if(0!=COS_InitTaskList())
    {   serPuts("COS_InitScheduler has crashed...");
    }
    COS_SemCreate(&sema_1, 0);
    ...
    COS_SemDestroy(&sema_1);

    getchar();
    return 0;
}


@endverbatim
********************************************************************/
uint8_t COS_SemDestroy(CosSema_t *s)
{
    /* free memory of all list elements, but don't destroy the tasks! */
    Node_t *pt = s->root_pt;

    while(s->root_pt != NULL)
    {   pt = s->root_pt;                    // node to be freed
        s->root_pt = s->root_pt->next_pt;  // next node in list
        free(pt);
    }
    return 0;
}




/*!
********************************************************************
  @par Beschreibung
  Inkrementiert den Zaehler des Semaphor. Die Liste der wartenden Tasks wird
  untersucht und die erste Task in der Liste wird in den Zustand
  TASK_STATE_READY gesetzt und aus der Liste der wartenden Tasks geloescht.

@see
@arg

@param s       - IN/OUT, Zeiger auf Semaphore

@retval none

@par Code-Beispiel::
@verbatim
CosSema_t sema_1;
...
void myTask2(CosTask_t *pt)
{   static count =0;
    COS_TASK_BEGIN(pt);

    serPuts("myTask2: started...\n");
    while(count < 100)
    {    serPuts("Task2\n");
         count++;
         COS_TASK_SLEEP(pt,100);
    }

    COS_SEM_SIGNAL(&sema_1);

    serPuts("myTask: terminates...\n");

    COS_TASK_END(pt);
}
@endverbatim
********************************************************************/
void COS_SEM_SIGNAL(CosSema_t *s)
{
  CosTask_t *task_pt=NULL;  /*!<  pointer to task structure */

  (s->count)++;
  if(s->root_pt != NULL)  // any task waiting on this sema?
  { task_pt = s->root_pt->task_pt;  // first waiting task
    task_pt->state = TASK_STATE_READY;  // make it ready to run
    s->root_pt = _unlinkTaskFromTaskList(s->root_pt, task_pt); // remove it from sema-list
  }

}
