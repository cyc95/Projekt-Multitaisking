/*!
 ********************************************************************
   @file            cos_semaphore.h
   @par Project   : co-operative Scheduler renesas uC
   @par Module    : Counting Semaphore fuer co-operative Scheduler (COS)

   @brief  Dies ist die renesas Version der COS-Semaphoren


   @par Author    : Fgb
   @par Department: E14
   @par Company   : Fachhochschule Hannover - University of Applied
                    Sciences and Arts

   @section Wie funktioniert ein Semaphor in COS?
                Es sind nur zaehlende Semaphoren implementiert.
                Ein Semaphor in COS besteht aus einem Zaehler und einer Liste
                von Tasks, die an diesem Semaphor blockierend warten. Das
                Macro COS_SEM_WAIT(s,pt) versucht den Zaehler des Semaphor
                zu verringern.
                Ist der Zaehler >0, so wird er dekrementiert und die Task
                laeuft bei der naechsten Aktivierung mit dem naechsten Befehl
                nach dem Macro weiter.
                Ist der Zaehler <=0 , so ist der Semaphor
                bereits belegt und die Task muss 'blockieren'. Da
                es ein wirkliches Blockieren einer Task in COS nicht geben
                darf (DEADLOCK!), wird hier nur der Zustand der Task auf
                TASK_STATE_BLOCKED gesetzt und die Task in die Warteliste
                des Semaphor eingetragen. Der Zaehler wird dekrementiert
                und ist ggf. negativ. Ist der Zaehler <=0 , so zeigt der
                Betrag des Zaehlers an, wieviele Tasks in der Warteliste stehen.
                Solange der  Zustand der Task TASK_STATE_BLOCKED ist,
                wird der Scheduler sie nicht aktivieren, dies simuliert
                das 'Blockieren'.
                Erst wenn sich der Zustand auf TASK_STATE_READY geaendert
                hat, wird die Task erneut aktiviert und laeuft mit dem naechsten
                Befehl nach dem Macro weiter.
                Die Zustandsaenderung auf TASK_STATE_READY erfolgt durch die
                Funktion COS_SEM_SIGNAL(CosSema_t *s). Hier wird der Zaehler
                des Semaphor inkrementiert und, falls die Warteliste nicht
                leer ist, wird die erste Task in der Warteliste in den
                Zustand  TASK_STATE_READY geschaltet.



 ********************************************************************

   @par History   :
   @verbatim
   Version | Date        | Author        | Change Description
   0.0     | 29.04. 2011 | Fgb           | First Version, Linux
   0.1     | 17.09. 2013 | Fgb           | nur noch Atmel, deutsche Doku.
   0.2     | 08.10. 2015 | Fgb           | Umbau auf renesas controller
   0.3     | 22.10.2015  | Fgb           | Bugfix in COS_SEM_WAIT(), siehe dort

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


#ifndef _cos_semaphore_h_
#define _cos_semaphore_h_


#include "cos_scheduler.h"
#include "cos_linear_task_list.h"


/***********************************************
 * FIFO data structure :
 ***********************************************/
typedef struct {
        int8_t count;     /*!< Anzahl der Ereignisse, Vorzeichen wird intern genutzt */
        Node_t *root_pt;  /*!< Zeiger auf naechste Knoten in der Liste der wartenden Tasks */
} CosSema_t;




uint8_t COS_SemCreate(CosSema_t *s, int8_t n_start);
uint8_t COS_SemDestroy(CosSema_t *s);




/*!
********************************************************************
  @par Beschreibung
Dieses Macro laesst eine Task an einem Semaphor warten.
Die Task blockiert nie, wenn COS_SEM_WAIT() zur Blockade fuehren soll, dann muss
die Taskfunktion verlassen werden und das naechste mal an der naechsten Befehlszeile
weiterlaufen.
Der Zustand der Task wird dann auf TASK_STATE_BLOCKED gesetzt, der Scheduler
wird diese Task nicht mehr starten. Die Task wird in die Liste der an diesem
Sema wartenden Tasks eingetragen.

Eventuell sollte man die Liste nach Task-Prio sortieren. Aber auch wenn man
die Warteliste nach Prioritaet sortiert, ist das Problem von
Prioritaetsinversionen nicht geloest.



@see
@arg  COS_SemCreate(), COS_SemDestroy()

@param s - IN, Zeiger auf Semaphore struct: CosSema_t
@param pt - IN, Zeiger auf Task struct: CosTask_t

@retval nichts

@par Code Beispiel:
@verbatim
CosSema_t sema_1;
...
void myTask(CosTask_t *pt)
{
    COS_TASK_BEGIN(pt);

    serPuts("myTask: started...\n");
    serPuts("myTask: step0\n");

    COS_SEM_WAIT(&sema_1,pt);

    serPuts("myTask: step1\n");
    COS_TASK_SCHEDULE(pt);
    serPuts("mserPutsyTask: step2\n");
    COS_TASK_SCHEDULE(pt);
    serPuts("myTask: step3\n");
    COS_TASK_SCHEDULE(pt);
    serPuts("myTask: step4\n");
    COS_TASK_SCHEDULE(pt);
    serPuts("myTask: terminates...\n");

    COS_TASK_END(pt);
}
@endverbatim
********************************************************************/
#define COS_SEM_WAIT(s,pt)  (pt)->lineCnt=__LINE__;\
                            if((s)->count <= 0) {  \
                              (pt)->state = TASK_STATE_BLOCKED; \
                              (s)->root_pt=_addTaskAtBeginningOfTaskList((s)->root_pt,(pt)); \
                            } \
                            ((s)->count)--; \
                            return;\
                            case __LINE__:



void COS_SEM_SIGNAL(CosSema_t *s);



#endif


