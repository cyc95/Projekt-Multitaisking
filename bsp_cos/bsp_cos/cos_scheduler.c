/*!
 ********************************************************************
   @file            cos_scheduler.c
   @par Project   : co-operative scheduler (COS) renesas uC
   @par Module    : task list and scheduling implementation

   @brief  Dies ist die Version von COS fuer renesas-Controller


   @par Author    : Fgb
   @par Department: E14
   @par Company   : Fachhochschule Hannover - University of Applied
                    Sciences and Arts

 ********************************************************************

   @par History   :
   @verbatim
   Version | Date       | Author        | Change Description
   0.0     | 21.03.2011 | Fgb           | First Version, Linux
   0.1     | 18.09.2013 | Fgb           | nur noch Atmel, deutsche Doku
   0.2     | 08.10.2015 | Fgb           | umgebaut auf renesas controller
   0.3     | 22.10.2015 | Fgb           | Bugfix: Sleep-Zeit im Scheduler auf 0
                                        | zureuckgesetzt, siehe dort
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




/*! \mainpage Co-Operative Scheduler (COS)

  \section Einleitung

Dies ist die Implementierung eines einfachen
'co-operative' Tasking Systems.

Dieser Scheduler nutzt das 'co-routine' Muster um Tasks zu realisieren (siehe
R.Dunkels: 'protothreads' und P.Eckstrand: 'cocoOS' and Michael Dorin:
'Building "instant-up" real-time operating systems').

Eine 'co-routine' hat mehrere Ausgaenge und wird beim naechsten Start
dort weiterlaufen, wo sie aufghoert hat. Die Implementierung erfolgt
mittels Macros, die ein 'switch'-Statement mit Zeilennummern als 'case'
Anweisungen enthalten und die 'return'-Statements and den kooperative
Scheduling-Punkten der Task besitzen. Dadurch fuehrt die Task-Funktion
eine normal 'return'-Anweisung aus und vergisst dabei alle ihre lokalen
Variablen (ausser 'static' Variablen). Bevor jedoch die 'return'-Anweisung
ausgefuehrt wird, speichert die Task-Funktion die aktuelle Codezeilennummer
in der Task-Struktur um den naechsten Einstiegspunkt in die 'switch'-
Anweisung beim naechsten Aufruf der Task-Funktion zu finden.

'Co-operative scheduling' (COS) kann nur direkt in der Task-Funktion
erfolgen, nicht in tiefer geschachtelten Funktionsaufrufen! COS wird
ueber Macros implementiert, siehe weiter unten.

Diese Implementierung basiert auf einem speziellen Feature des
gcc-Compilers, siehe Macros. Der gcc-Compiler fuer Atmel und auch der
fuer Linux besitzen dieses benoetigten Feature der C-Programmiersprache.

Eine Task-Funktion ist eine callback-Funktion, die entweder bis zu ihrem
endgueltigen Funktionsende laeuft und dann aus der Task-Liste geloescht
wird (die Task verschwindet!), oder die bis zu einem kooperativen
Scheduling-Punkt laeuft (siehe Macros) und dann ein 'return' im Sinne
einer 'co-routine' ausfuehrt, wobei die Codezeile fuer den naechsten
Start gespeichert wird.

Eine Uhr ist noetig, um die callback-Funktionen zu festgelegten Zeiten
zu starten. Die Funktion _gettime_Ticks(void) ist an das Rechnersystem
anzupassen, sodass sie als Systemuhr mit der Einheit 'Tick' arbeitet.
Der 'Tick' kann bzpw. ueber einen Timer mit entsprechender Interrupt-
Routine generiert werden.

Der Scheduler ruft die callback-Funktionen auf, unter Beruecksichtigung
von Task-Zustand, -Prioritaet und dem Zeitpunkt der letzen Aktivierung.

Aus der Menge der callback-Funktionen im Zustand 'TASK_STATE_READY' wird
diejenige mit der haechsten Prioritaet aufgerufen, fuer die gilt:
timeNow-timeLastActivation > sleepTime_Ticks

Da hier mit Zeitdifferenzen gearbeitet wird, gibt es keine Probleme mit
dem Ueberlauf des Timers: die Differenz wird richtig berechnet, solange
sie kuerzer ist als ein kompletter Timer-Umlauf.

Bevor die Task-Funktion aufgerufen wird, setzt der Scheduler ihre
sleepTime_Ticks auf 0, d.h. die Task-Funktion muss explizit ihre
gewuenschte sleepTime_Ticks bei jedem Aufruf erneut selbst setzen,
siehe Beispiel weiter unten!

Es gibt immer mindestens eine Task: die idle-Task mit ihrer callback-
Funktion static void idleTask(CosTask_t *task_pt);

Alle anderen Task-Funktionen haben ebenfalls keinen Rueckgabewert und
besitzen den einzigen Parameter CosTask_t*.

Der Scheduler laeuft in einer Endlosschleife, die lineare Task-Liste
wird nicht geloescht und es gibt kein regulaeres Programmende (auf den
Atmel uC darf die main() Funktion nie enden).

Als erstes ist die Funktion int COS_InitTaskList(void); aufzurufen, die
die idle-Task in die Task-Liste eintraegt.

Der Scheduler hat zwei moegliche Modi: prioritaetsbasiert oder
'round-robin'.

Das prioritaetsbasierte Scheduling erfordert eine nach Prioritaeten
sortierte Task-Liste. Im prioritaetsbasierten Modus kann eine Schaetzung
der CPU-Auslastung mit zwei Tasks erreicht werden: Die cpu-load-Task
hat die maximale Prioritaet und setzt einen Zaehler auf den Wert 100.
Die idle-Task mit niedrigster Prioritaet dekrementiert den Zaehler.
Die Task-Perioden sind so einzustellen, das die cpu-load-Task einmal
laeuft, waehrend die idle-Task 100 Aufrufe schafft, dies ohne weitere
CPU-Last durch andere Tasks. Der Zaehlerstand ist dann ein Mass fuer
die Auslastung: bei wachsender Last kommt die idle-Task nicht oft genug
dran, um den Zaehler bis auf 0 zu verringern.

Wird der Scheduler im round-robin Modus betrieben, so funktioniert die
Lastabschaetzung nicht, sie richtet aber auch keinen Schaden an.

  @verbatim
  list of tasks:
                    node
    root          --------      --------           --------
    -----         |      |----->|      |-- ... --->|      |----> NULL
    |   |-------->|      |      |      |           |      |
    -----         --------      --------           --------
                     |             |                  |
                     \/            \/                 \/
                   -----         -----              -----
                   |   |         |   |              |   |
                   -----         -----              -----
                   task
  @endverbatim



 **********************************************************************/
#include "cos_scheduler.h"
#include <stdlib.h>
#include "cos_ser.h"



/*! 0 fuer round robin Tasking, 1 fuer prio-basierten Scheduler */
#define PRIO_BASED_SCHEDULING 1

#define IDLE_TASK_PERIOD_TICKS      _milliSecToTicks(10)
#define IDLE_TASK_PRIO              0
#define LOAD_MEASURE_TASK_PERIOD_TICKS (100*IDLE_TASK_PERIOD_TICKS)
#define LOAD_MEASURE_TASK_PRIO      255




/*------------- DEBUGGING ---------------------------------*/
#define DEBUG_MODULE 0

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

static void _toggle_PD(uint8_t bit)
{
    static uint8_t i=0;

    if(i)
    {   //set_bit(PORTD, bit);
    }
    else
    {   //clear_bit(PORTD, bit);
    }
    i=!i;
}
#endif
/*---------------------------------------------------------------*/






/* private types */


/****************************************************************/
/* private module variables */
/****************************************************************/
static Node_t *root_g=NULL;           /*! root Pointer der Task-Liste */
static uint8_t cpuLoadPerCent_g=100;  /*! fuer CPU-Lastmessung */
static uint8_t cpuLoadCounter_g=100;  /*! fuer CPU-Lastmessung */
/****************************************************************/

/****************************************************************/
/* private function prototypes */
/****************************************************************/

static void _idleTask(CosTask_t *pt);
static void _cpuLoadMeasureTask(CosTask_t *pt);
static CosTask_t *_idleTask_pt_g = NULL;
static CosTask_t *_cpuLoadMeasureTask_pt_g = NULL;


/****************************************************************/




/****************************************************************/
/* private functions */
/****************************************************************/


/*---------------------------------------------------------------*/
/*!
 ********************************************************************
  @par Beschreibung
       Die Task-Ringliste besteht aus mindestens aus der idle-Task.
       Diese Task hat die niedrigste Prioritaet.
       Sie dekrementiert den internen Zaehler fuer die CPU-Lastmessung.
       Bei geringer Auslastung der CPU laeuft sie 100 mal bevor die
       cpu-load Task den Zaehler auf 100 zuruecksetzt. Der Zaehler kann
       also nur bis auf 0 heruntergezaehlt werden, falls genug Zeit fuer
       die idle-Task vorhanden ist.

  @see
  @arg

  @param  keine
  @retval keine
 ********************************************************************/
static void _idleTask(CosTask_t *pt)
{
    COS_TASK_BEGIN(pt);
    while(1)
    {     if(cpuLoadCounter_g > 0)
          {   cpuLoadCounter_g--;
          }
          //DebugCode(_toggle_PD(5););
          COS_TASK_SLEEP(pt,IDLE_TASK_PERIOD_TICKS);
    }
    COS_TASK_END(pt);
}
/*---------------------------------------------------------------*/
/*!
 ********************************************************************
  @par Beschreibung
       Diese Task ist optional. Sie hat die hoechste Prioritaet und
       setzt den internen Zaehler auf 100. Der Zaehler wird durch die
       idle-Task dekrementiert. Wenn die Auslastung gering ist, schafft
       es die idle-Task 100 zu laufen bevor diese Task erneut laeuft.
       Wenn der Rechner ausgelastet ist, kommt die idle-Task nie dran
       und der Zaehler bleibt bei 100 stehen: die Last ist 100 Prozent.

  @see  _idleTask(), COS_GetCPULoadInPercent()
  @arg

  @param  keine

  @retval keine
 ********************************************************************/
static void _cpuLoadMeasureTask(CosTask_t *pt)
{
    COS_TASK_BEGIN(pt);
    while(1)
    {     cpuLoadPerCent_g = cpuLoadCounter_g;  // remains constant for the period
          cpuLoadCounter_g = 100;
          COS_TASK_SLEEP(pt,LOAD_MEASURE_TASK_PERIOD_TICKS);
    }
    COS_TASK_END(pt);
}
/*---------------------------------------------------------------*/





/****************************************************************/
/* exported module functions */
/****************************************************************/


/*!
 ********************************************************************
  @par Beschreibung
       Initialisiert die Task-Liste. Tasks werden in einer linearen
       Liste
       gehalten, die mindestens aus der idle-Task besteht. Die Liste
       wird nach Prioritaet sortiert. Die Task mit der hoechsten
       Prioritaet steht vorne in der Liste.

  @see
  @arg

  @param  keine
  @retval 0 fuer ok, negativ bei Fehler
  @par Code-Beispiel:

  @verbatim
int main(void)
{   ...
    if(0!=COS_InitTaskList())
    {   serPuts("COS_InitScheduler has crashed...");
    }
    COS_PrintTaskList();
    ...
    if(0!=COS_RunScheduler())
    {   serPuts("COS_RunScheduler has crashed...");
    }
    while(1);
}
  @endverbatim
 ********************************************************************/
int8_t COS_InitTaskList(void)
{
    //DebugCode(_msg("InitTaskList\r\n"););
    root_g = NULL;  /* empty task list */
    /* task functions are kept in a linear list, that always has at least
       one element: the idle task.
    */

    _idleTask_pt_g = COS_CreateTask(IDLE_TASK_PRIO, NULL, _idleTask);

    /* add optional cpu load estimation task to list */
    _cpuLoadMeasureTask_pt_g = COS_CreateTask(LOAD_MEASURE_TASK_PRIO, NULL, _cpuLoadMeasureTask);

    _sortLinearListPrio(root_g);
    return 0;
}


/*---------------------------------------------------------------*/

/*!
 ********************************************************************
  @par Beschreibung
       Alloziert und initialisiert eine Task-Struktur alloziert einen
       neuen Knoten der Task-Liste, h�ngt den Knoten in die Liste und
       soritiert die Liste neu nach Prioritaeten.

  @see COS_DeleteTask(), COS_SuspendTask(), COS_ResumeTask(), COS_SetTaskPrio(),

  @arg

  @param  prio     - IN, Task-Prioritaet. 1 ist minimal, 254 ist maximal.
                      0 und 255 sind reserviert
  @param  pData    - IN, Zeiger auf Nutzerdaten-struct, lokale Task Daten
  @param  func       IN, Name der Task-Funktion

  @retval Zeiger auf Task struct oder NULL bei Fehler

  @par Code Beispiel:
  @verbatim
#include <stdio.h>
#include "avr_cos_scheduler.h"

void COS_Task_TogglePD6(CosTask_t* task_pt)
{   static uint8_t on=0;

    COS_TASK_BEGIN(task_pt);

    while(1)
    {
        if(!on)
        {   set_bit(PORTD,6);
            on=1;
        }
        else
        {   clear_bit(PORTD,6);
            on=0;
        }
        COS_TASK_SLEEP(task_pt,100);
    }
    COS_TASK_END(task_pt);
}

CosTask_t *t_TogglePD6;

int main(void)
{   _initPortDirections();
    _initPorts();
    _initSystemTime();
    sei();                  // alle INT freigeben

    serInit(CPU_CLOCK, BAUD);
    _startupMessageRS232();

    if(0!=COS_InitTaskList())
    {   serPuts("COS_InitScheduler has crashed...");
    }
    // Task erzeugen
    t_TogglePD6 = COS_CreateTask(5, NULL, COS_Task_TogglePD6);
    COS_PrintTaskList();
    if(0!=COS_RunScheduler())
    {   serPuts("COS_RunScheduler has crashed...");
    }
    while(1);    // this should not be reached...
    return 0;
}

  @endverbatim
 ********************************************************************/
CosTask_t* COS_CreateTask(uint8_t prio, void * pData, void (*func) (CosTask_t *))
{
    CosTask_t *t_pt= NULL;

    //DebugCode(_msg("CreateTask\r\n"););

    /* allocate and init task struct */
    t_pt = _newTask(prio, pData, func);
    if(t_pt==NULL)
    {   DebugCode(_msg("CreateTask:_newTask!\r\n"););
        return NULL;
    }

    root_g = _addTaskAtBeginningOfTaskList(root_g, t_pt);
    _sortLinearListPrio(root_g);

    return t_pt;  /* pointer to task struct */
}
/*---------------------------------------------------------------*/






/*!
 ********************************************************************
  @par Beschreibung
       Loescht eine Task aus der Task-Liste und gibt dynamischen
       Speicher frei. Die Task-Liste enthaelt mindestens eine Task:
       die idle-Task darf nicht geloescht werden.

  @see COS_CreateTask()
  @arg

  @param  task_pt -     IN, Pointer auf die Task-Struktur.

  @retval 0 fuer ok, negativ bei Fehler
 ********************************************************************/
int8_t COS_DeleteTask(CosTask_t* task_pt)
{
    /* remove from list, i.e. free the corresponding node,
       task struct will not be freed here */
    root_g = _unlinkTaskFromTaskList(root_g, task_pt);

    /* free memory of task struct */
    free(task_pt);
    return 0;
}



/*!
 ********************************************************************
  @par Beschreibung
       Setzt den Task-Zustand auf  TASK_STATE_SUSPENDED.
       Die Task wird nicht geloescht und kann durch COS_ResumeTask()
       reaktiviert werden.


  @see COS_ResumeTask()
  @arg

  @param  task_pt -     IN, Pointer auf Task-Struktur.

  @retval 0 fuer ok, negativ bei Fehler
 ********************************************************************/
/*---------------------------------------------------------------*/
int8_t COS_SuspendTask(CosTask_t* task_pt)
{
    Node_t *pt=NULL;

    pt = _searchTaskInList(root_g, task_pt);
    if(NULL == pt)
    {   DebugCode(_msg("Suspend:task not found\r\n"););
        return -1;
    }
    pt->task_pt->state = TASK_STATE_SUSPENDED;
    return 0;
}
/*---------------------------------------------------------------*/



/*!
 ********************************************************************
  @par Beschreibung
       Setzt den Task-Zustand auf  TASK_STATE_READY.
       Die Task nimmt dadurch wieder am Scheduling teil.

  @see COS_SuspendTask()
  @arg

  @param  task_pt -     IN, Pointer auf Task-Struktur.

  @retval 0 fuer ok, negativ bei Fehler
 ********************************************************************/
int8_t COS_ResumeTask(CosTask_t* task_pt)
{
    Node_t *pt=NULL;

    pt = _searchTaskInList(root_g, task_pt);
    if(NULL == pt)
    {   DebugCode(_msg("Resume:task not found\r\n"););
        return -1;
    }
    pt->task_pt->state = TASK_STATE_READY;
    return 0;
}
/*---------------------------------------------------------------*/
/*!
 ********************************************************************
  @par Beschreibung
       Setzt die Prioritaet einer Task auf einen neuen Wert, 1 ist
       minimal, 254 maximal, 0 und 255 sind reserviert.

  @see
  @arg

  @param  task_pt -     IN, Pointer auf Task-Struktur.
  @param  taskPrio -  IN, neue Task-Prioritaet

@retval 0 fuer ok, negativ bei Fehler
********************************************************************/
int8_t COS_SetTaskPrio(CosTask_t* task_pt,uint8_t taskPrio)
{
    Node_t *pt=NULL;

    pt = _searchTaskInList(root_g, task_pt);
    if(NULL == pt)
    {   DebugCode(_msg("SetTaskPrio:task not found\r\n"););
        return -1;
    }
    pt->task_pt->prio = taskPrio;
    _sortLinearListPrio(root_g);
    return 0;
}
/*---------------------------------------------------------------*/

/*---------------------------------------------------------------*/
/*---------------------------------------------------------------*/





/**********************************************************
 * Bugfix 22.10.2015: Beim Sleep wird eine Wartezeit gesetzt, die nur durch
 * ein neues COS_TASK_SLEEP() oder COS_TASK_SCHEDULE() geaendert wurde.
 * Vor jedem Neustart der einer Task wird jetzt die Sleep-Zeit auf 0
 * zurueckgesetzt. Die Task muss ihre Sleep-Zeit bei jedem Lauf neu setzen mit
 * COS_TASK_SLEEP(), COS_TASK_SCHEDULE() (setzt die Sleep-Time auf 0). Semaphoren
 * aendern die Sleep-Time nicht, sondern aendern die Task-State.
 *
 */


#if PRIO_BASED_SCHEDULING
/*!
 ********************************************************************
  @par Beschreibung
       Prioritaetsbasierter Scheduler. Die Task-Liste ist nach
       Prioritaet sortiert, der Scheduler laesst die erste Task-Funktion
       der Liste laufen, deren Zustand TASK_STATE_READY ist und fuer die
       gilt: timeNow-timeLastActivation > sleepTime_Ticks.
       Der Scheduler laeuft in einer Endlosschleife.

  @see
  @arg

  @param  keine

  @retval 0 fuer ok, negativ bei Fehler (sollte nie zurueck kommen,
          Endlosschleife)
  @par Code-Beispiel:

  @verbatim
int main(void)
{   ...
    if(0!=COS_InitTaskList())
    {   serPuts("COS_InitScheduler has crashed...");
    }
    COS_PrintTaskList();
    ...
    if(0!=COS_RunScheduler())
    {   serPuts("COS_RunScheduler has crashed...");
    }
    getchar();
    return 0;
}
  @endverbatim
 ********************************************************************/
int8_t COS_RunScheduler(void)
{


    Node_t *pt=NULL;
    uint16_t t_Ticks;

    //DebugCode(_msg("RunScheduler,prio based\r\n"););

    pt = root_g; /* first task, highest prio */
    while(1) /* loop forever */
    {   /* time to run? */
        t_Ticks = _gettime_Ticks();
        /* time wrap around is ok, time difference will be right... */
        if(((uint16_t)(t_Ticks - pt->task_pt->lastActivationTime_Ticks) >=
             pt->task_pt->sleepTime_Ticks)&&
            (pt->task_pt->state == TASK_STATE_READY))
        {  pt->task_pt->lastActivationTime_Ticks = t_Ticks;
           /*  when the task function runs to its very end, the task will be deleted:
               it will be removed from the list, and the task struct will be freed,
               i.e. pt->task_pt is no longer valid.
           */
           pt->task_pt->sleepTime_Ticks = 0;  // Bugfix 22.10.2015: must be specified by task!
           pt->task_pt->func(pt->task_pt);  /* call task function, must not block! */
           pt = root_g; /* next: check task with highest prio */
        }
        else
        {  pt = pt->next_pt;  /* check next task in list */
           if(NULL==pt)
           {  pt = root_g;  /* treat linear list as ring list */
           }
        }
    }return 0;
}
#else
/*!
 ********************************************************************
  @par Beschreibung
       Round-robin Scheduler. Die Task-Liste ist nach
       Prioritaet sortiert, aber Prioritaeten werden von diesem
       Scheduler ignoriert. Der Scheduler laesst der Reihe nach alle Tasks
       laufen, deren Zustand TASK_STATE_READY ist und fuer die
       gilt: timeNow-timeLastActivation > sleepTime_Ticks.
       Der Scheduler laeuft in einer Endlosschleife.

  @see
  @arg

  @param  keine

  @retval 0 fuer ok, negativ bei Fehler (sollte nie zurueck kommen,
          Endlosschleife)
  @par Code-Beispiel:

  @verbatim
int main(void)
{   ...
    if(0!=COS_InitTaskList())
    {   serPuts("COS_InitScheduler has crashed...");
    }
    COS_PrintTaskList();
    ...
    if(0!=COS_RunScheduler())
    {   serPuts("COS_RunScheduler has crashed...");
    }
    getchar();
    return 0;
}
  @endverbatim
 ********************************************************************/
int8_t COS_RunScheduler(void)
{   Node_t *pt=NULL;
    uint16_t t_Ticks;

    //DebugCode(_msg("RunScheduler, round robin\r\n"););

    pt = root_g; /* first task */
    while(1) /* run forever */
    {   /* time to run? */
        t_Ticks = _gettime_Ticks();
        if(((uint16_t)(t_Ticks - pt->task_pt->lastActivationTime_Ticks) >=
             pt->task_pt->sleepTime_Ticks)&&
            (pt->task_pt->state == TASK_STATE_READY))
        {  pt->task_pt->lastActivationTime_Ticks = t_Ticks;
           /*  when the task function runs to its very end, the task will be deleted:
               it will be removed from the list, and the task struct will be freed,
               i.e. pt->task_pt is no longer valid.
           */
           pt->task_pt->sleepTime_Ticks = 0;  // Bugfix 22.10.2015: must be specified by task!
           pt->task_pt->func(pt->task_pt);  /* call task function, must not block! */
        }
        pt = pt->next_pt;  /* next task in list */
        if(NULL==pt)
        {   pt = root_g;  /* use task list as ring list */
        }
    }return 0;
}
#endif
/*---------------------------------------------------------------*/



/*!
 ********************************************************************
  @par Beschreibung
       Diese Funktion wird fuer Testzwecke benutzt.
       Sie gibt die aktuelle Task-Liste am Terminal aus.
  @see
  @arg

  @param  nichts
  @retval nichts
 ********************************************************************/
void COS_PrintTaskList(void)
{
    Node_t *pt=root_g;

    while(NULL != pt)
    {   serPuts("\r\ntask:");  serOutUint32Hex((uint32_t) pt->task_pt);
        serPuts("\r\nState:"); serOutUint8Hex(pt->task_pt->state);
        serPuts("\r\nPrio:");  serOutUint8Hex(pt->task_pt->prio);
        pt = pt->next_pt;
    }
}
/*---------------------------------------------------------------*/
/*!
 ********************************************************************
  @par Beschreibung
       Eine einfache Lastmessung benutzt zwei Tasks, die automatisch
       in der Funktion COS_InitTaskList() erzeugt werden. Die Task
       _cpuLoadMeasureTask() hat maximale Prioritaet und setzt einen
       Zaehler auf 100, der durch die idle-Task dekrementiert wird.
       Der Zaehlerstand kann als Mass fuer die CPU-Auslastung dienen.

  @see
  @arg

  @param  keine

  @retval cpu Load in Prozent
 ********************************************************************/
int8_t COS_GetCPULoadInPercent(void)
{   return cpuLoadPerCent_g;
}
/*---------------------------------------------------------------*/
/*---------------------------------------------------------------*/
/*---------------------------------------------------------------*/
/*---------------------------------------------------------------*/
/*---------------------------------------------------------------*/

/****************************************************************/




