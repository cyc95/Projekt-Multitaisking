/*!
 ********************************************************************
   @file            cos_linear_task_list.h
   @par Project   : co-operative scheduler  renesas uC
   @par Module    : linear task list implementation

   @brief  Dies ist die renesas  Version des  'co-operative' Schedulers


   @par Author    : Fgb
   @par Department: E14
   @par Company   : Fachhochschule Hannover - University of Applied
                    Sciences and Arts

 ********************************************************************

   @par History   :
   @verbatim
   Version | Date        | Author        | Change Description
   0.0     | 21.03. 2011 | Fgb           | First Version, Linux
   0.1     | 17.09. 2013 | Fgb           | nur noch Atmel, deutsche Doku.
   0.2     | 09.10.2015  | Fgb           | umgestiegen auf renesas controller
   @endverbatim

Routinen zur Verwaltung einer linearen Task-Liste:

  @verbatim

                          node
           root          --------      --------           --------
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



#ifndef linear_task_list_h_
#define linear_task_list_h_


#include "cos_systime.h"
#include <stdlib.h>


#define TASK_STATE_READY         0
#define TASK_STATE_SUSPENDED     1
#define TASK_STATE_BLOCKED       2



/*!
 ********************************************************************
  @par Beschreibung
  Task-Sturktur des 'co-operative' Schedulers. Der Scheduler startet die
  Funktion 'func', aktualisiert 'lastActivationTime_Ticks' auf die
  aktuelle Zeit in 'Ticks' und setzt
  'nextActivationTime_Ticks = lastActivationTime_Ticks'. Dadurch wird
  der Scheduler veranlasst, die Task so schnell wie moeglich wieder zu
  starten, ohne weitere 'sleep' Zeit. Falls die Task erst nach einer
  definierten 'sleep' Zeit erneut laufen soll, muss sie das 'sleep'-Macro
  explizit aufrufen. Dadurch wird die neue Startzeit gesetzt auf:
  'nextActivationTime_Ticks = lastActivationTime_Ticks + sleepTime_Ticks'.

@par Task Zustaende
Eine Task kann im Zustand 'ready' sein, man kann sie mit einem speziellen
Kommando in den Zustand 'suspended' versetzen oder mit einem Semphor in
den Zustand 'blocked' bringen.

@par Re-entry Punkt
Der 'co-operative' Scheduler startet die Task-Funktio, die ihre Arbeit
in der Programmzeile 'lineCnt' wieder aufnimmt. Das Anspringen dieser
Programmzeile wird mit Hilfe von Macros erreicht.

@par User data: private Daten der Task
Eine Task kann lokale Daten in 'static' Variablen speichern, die ihre
Werte bis zum naechsten Funktionsaufruf behalten. Dies ist einfach, aber
die Task-Funktion kann dann nur in einer einzigen Instanz laufen, denn
static Variable sind der Funktion zugeordnet, nicht der Task-Struktur.
Wenn man mehrere Tasks erzeugen will, die alle dieselbe Task-Funktion
'func' in mehreren Instanzen nutzen, muss man die lokalen Daten der
Task-Stuktur zuordnen, nicht dem Funktionscode. Der Zeiger 'pData' kann
benutzt werden, um auf Nutzerdaten der Task zu zeigen. Diese Daten sind
dann privat fuer jede Task (Instanz).


  @see
 ********************************************************************/
typedef struct CosTask_t CosTask_t;
struct CosTask_t
{   uint16_t lastActivationTime_Ticks; /*!< letzter Startzeitpunkt in Ticks */
    uint16_t sleepTime_Ticks;          /*!< laesst die Task blockieren.
                                            0 == sleepTime_Ticks bedeutet:
                                            sofort starten */
    uint8_t  state;     /*!< Task Zustaende:  TASK_STATE_READY,
                             TASK_STATE_SUSPENDED, TASK_STATE_BLOCKED */
    uint8_t  prio;      /*!< 1 ist minimal, 254 ist maximal. 0 und 255 sind reserviert */
    uint16_t lineCnt;   /*!< speichert Programmzeile fuer re-entry */
    void * pData;       /*!< Zeiger auf Nutzer-Daten, moelicher Speicherplatz fuer
                             lokale Task-Variable */
    void (*func)(CosTask_t*); /*!< Name der Task-Funktion */
};



typedef struct Node_t Node_t;
struct Node_t {
    CosTask_t *task_pt;  /*!<  Pointer auf Task-Struktur */
    Node_t *next_pt;  /*!<  Pointer auf naechsten Knoten der Ringliste */
};



Node_t *_addTaskAtBeginningOfTaskList(Node_t *root_pt, CosTask_t *task_pt);
Node_t *_unlinkTaskFromTaskList(Node_t *root_pt, CosTask_t *task_pt);
Node_t *_searchTaskInList(Node_t *root_pt, CosTask_t *task_pt);
Node_t *_searchPredecessorTaskInList(Node_t *root_pt, CosTask_t *task_pt);
Node_t *_newNode(CosTask_t *task_pt);
void _sortLinearListPrio(Node_t *root_pt);
CosTask_t *_newTask(uint8_t prio, void * pData, void (*func) (CosTask_t *));



#endif

