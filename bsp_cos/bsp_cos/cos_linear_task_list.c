/*!
 ********************************************************************
   @file            cos_linear_task_list.c
   @par Project   : 'co-operative' Scheduler renesas uC
   @par Module    : lineare Task-Liste des 'co-operative' Schedulers

   @brief  Dies ist die renesas Version der COS Task-Liste

   @par Author    : Fgb
   @par Department: E14
   @par Company   : Fachhochschule Hannover - University of Applied
                    Sciences and Arts

 ********************************************************************

   @par History   :
   @verbatim
   Version | Date        | Author        | Change Description
   0.0     | 21.03. 2011 | Fgb           | First Version, Linux
   0.1     | 17.09. 2013 | Fgb           | nur noch Atmel, deutsche Doku
   0.2     | 08.10. 2015 | Fgb           | umgeschrieben für renesas
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



#include <stdlib.h>
//#include <avr/io.h>
//#include <stdint.h>
#include "cos_ser.h"
#include <stdio.h>  // fuer degugging am Terminal

#include "cos_linear_task_list.h"



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
********************************************************************
  @par Beschreibung
  Erzeugt einen neuen Lisenknoten, verbindet die Task mit diesem Knoten und
  fuegt den Konten als erstes Element in die Liste ein. D.h. der root-Zeiger
  der Liste aendert sich! Der neue root-Zeiger wird mit return zurueckgegeben.
  Die Task-Struktur, auf die task-Pt zeigt, muss vor dem Aufruf dieser Funktion
  initialisiert sein.

@see _unlinkTaskFromTaskList(), _newNode()
@arg

@param root_pt - IN, Zeiger auf das erste Listenelement
@param task_pt - IN, Zeiger auf initialisierte Task-Struktur

@retval neuer Zeiger auf das erste Listenelement

@par Code-Beispiel::
@verbatim
int main(void)
{   Node_t *root_pt=NULL;
    CosTask_t *pt=NULL;

    ...
    pt = _newTask(...);
    root_pt = _addTaskAtBeginningOfTaskList(root_pt, pt);
    ...

    return 0;
}
@endverbatim
********************************************************************/
Node_t *_addTaskAtBeginningOfTaskList(Node_t *root_pt, CosTask_t *task_pt)
{   Node_t *pt=NULL;

    pt = _newNode(task_pt);    /* init node, connect to existing task */
    pt->next_pt = root_pt;      /* append old list to new node: new first element */
    return pt;                  /* new root pointer: address of first element */
}
/*---------------------------------------------------------------*/



/*!
********************************************************************
  @par Beschreibung
  Diese Routine durchsucht die Task-Liste nach dem Knoten, der auf dieselbe
  Task zeigt, wie task_pt. Der Knoten wird aus der Liste geloescht, und sein
  Speicher wird freigegeben. Die Task-Struktur wird aber nicht geloescht, da
  sie noch Teil einer anderen Liste sein koennte. (Bsp: Auch Semaphoren besitzen eine
  lineare Liste der an ihnen wartenden Tasks) Der root-Zeiger der Liste
  wird geaendert, falls der erste Konten der Liste geloescht wurde. Die
  Funktion gibt den (ggf.) neuen root-Zeiger mit return zurueck.

@see _addTaskAtBeginningOfTaskList(), _newNode()
@arg

@param root_pt - IN, Zeiger auf das erste Listenelement
@param task_pt - IN, Zeiger auf initialisierte Task-Struktur

@retval neuer Zeiger auf das erste Listenelement

@par Code-Beispiel::
@verbatim
int main(void)
{   Node_t *root_pt=NULL;
    CosTask_t *pt=NULL;

    ...
    pt = _newTask(...);
    root_pt = _addTaskAtBeginningOfTaskList(root_pt, pt);
    ...
    root_pt = _unlinkTaskFromTaskList(root_pt, pt);
    ...


    return 0;
}
@endverbatim
********************************************************************/
 Node_t *_unlinkTaskFromTaskList(Node_t *root_pt, CosTask_t *task_pt)
{   Node_t *pt=NULL;
    Node_t *predecessor_pt=NULL;

    pt = _searchTaskInList(root_pt, task_pt);
    if(NULL == pt)
    {   DebugCode(_msg("_unlink...task not found"););
        return root_pt;  /* don't change the list! */
    }
    /* is it the first node in the list? root_pt has to be changed... */
    if(pt == root_pt)
    {   pt = pt->next_pt; /* second node in list */
        free(root_pt); /* free the first node, don't touch the task! */
        return pt;    /* the old second element now is the first */
    }
    /* node exists, is not the first list element, should have a predecessor... */
    predecessor_pt = _searchPredecessorTaskInList(root_pt, task_pt);
    if(NULL == predecessor_pt)
    {   DebugCode(_msg("_unlink...pred.task not found"););
        return root_pt;  /* don't change the list! */
    }
    /* task node and its predecessor have been found. Unlink task node: */
    predecessor_pt->next_pt = pt->next_pt;
    free(pt); /* free the node, don't touch the task! */
    return root_pt; /* old first list element still is the first */
}

/*---------------------------------------------------------------*/


/*!
********************************************************************
  @par Beschreibung
  Gegeben ist der Zeiger auf eine Task. Diese Funktion durchsucht die
  lineare Liste nach dem Knoten, der mit der Task verbunden ist. Die
  Funktion gibt einen Zeiger auf diesen Knoten zurueck, oder NULL, falls
  nichts gefunden wurde.

@see _unlinkTaskFromTaskList()
@arg

@param root_pt - IN, Zeiger auf das erste Listenelement
@param task_pt - IN, Zeiger auf initialisierte Task-Struktur

@retval Zeiger auf den Konten, der mit Task *task_pt verbunden ist

@par Code-Beispiel::
@verbatim
int main(void)
{   Node_t *root_pt=NULL;
    Node_t *npt=NULL;
    CosTask_t *pt=NULL;

    ...
    pt = _newTask(...);
    root_pt = _addTaskAtBeginningOfTaskList(root_pt, pt);
    ...
    npt = _searchTaskInList(root_pt, pt);
    ...

    return 0;
}
@endverbatim
********************************************************************/
Node_t *_searchTaskInList(Node_t *root_pt, CosTask_t *task_pt)
{   Node_t *pt=NULL;

    pt = root_pt;
    while(pt != NULL)
    {   if((pt->task_pt) == task_pt)
        {   return pt;
        }
        pt = pt->next_pt;
    }
    DebugCode(_msg("Task not found"););
    return NULL;
}
/*---------------------------------------------------------------*/

/*!
********************************************************************
  @par Beschreibung
  Gegeben ist der Zeiger auf eine Task. Diese Funktion durchsucht die Liste
  nach dem Vorgaengerknoten der Task. Die Funktion gibt den Zeiger auf den
  Vorgaengerknoten zurueck, oder NULL, falls nichts gefunden wurde.

@see _searchTaskInList(), _unlinkTaskFromTaskList()
@arg

@param root_pt - IN, Zeiger auf das erste Listenelement
@param task_pt - IN, Zeiger auf initialisierte Task-Struktur

@retval Zeiger auf den Vorgaengerknoten der Task

@par Code-Beispiel::
@verbatim
int main(void)
{   Node_t *root_pt=NULL;
    Node_t *npt=NULL;
    CosTask_t *pt=NULL;

    ...
    pt = _newTask(...);
    root_pt = _addTaskAtBeginningOfTaskList(root_pt, pt);
    ...
    npt = _searchPredecessorTaskInList(root_pt, pt);
    ...

    return 0;
}
@endverbatim
********************************************************************/
Node_t *_searchPredecessorTaskInList(Node_t *root_pt, CosTask_t *task_pt)
{   Node_t *pt=NULL;

    /* list empty? */
    if(NULL==root_pt)
    {   DebugCode(_msg("_searchPred...empty list"););
        return NULL;
    }
    /* task connected to first node?  no predecessor...*/
    pt = root_pt;
    if(pt->task_pt == task_pt)
    {   DebugCode(_msg("_searchPred...nothing found"););
        return NULL;
    }
    /* ok, list has at least one node */
    while(pt->next_pt != NULL)
    {   if((pt->next_pt->task_pt) == task_pt) return pt;  /* the predecessor! */
        pt = pt->next_pt;
    }
    DebugCode(_msg("Pred. Task not found"););
    return NULL;
}
/*---------------------------------------------------------------*/


/*!
********************************************************************
  @par Beschreibung
  Gegeben ist der Zeiger auf eine Task. Diese Funktion erzeugt einen neuen
  Knoten und verbindet ihn mit der Task. Der Zeiger auf den neuen Knoten
  wird zurueck gegeben.

@see _addTaskAtBeginningOfTaskList()
@arg

@param task_pt - IN, Zeiger auf initialisierte Task-Struktur

@retval Zeiger auf den neuen Knoten

@par Code-Beispiel::
@verbatim
int main(void)
{   Node_t *npt=NULL;
    CosTask_t *pt=NULL;

    ...
    pt  = _newTask(...);
    npt = _newNode(pt);
    ...

    return 0;
}
@endverbatim
********************************************************************/
Node_t *_newNode(CosTask_t *task_pt)
{   Node_t *pt;
    pt = (Node_t *) malloc(sizeof(Node_t));
    if(pt!=NULL)
    {   pt->task_pt = task_pt;
        pt->next_pt = NULL;
    }
    return pt;
}
/*---------------------------------------------------------------*/

/*!
********************************************************************
  @par Beschreibung
  Alloziert Speicher fuer eine Task-Struktur und initialisiert ihn.
  Der Zeiger auf die Struktur wird zurueck gegeben.

@see _addTaskAtBeginningOfTaskList(), _newNode()
@arg

@param  prio - IN Prioritaet der Task (min) 1..254 (max)
@param  pData -IN Zeiger auf private Nutzerdaten der Task
@param  func  -IN Name der Task-Funktion


@retval Zeiger auf die neue Task-Struktur oder NULL bei Fehler

@par Code-Beispiel::
@verbatim
int main(void)
{   Node_t *npt=NULL;
    CosTask_t *pt=NULL;

    ...
    pt  = _newTask(...);
    npt = _newNode(pt);
    ...

    return 0;
}
@endverbatim
********************************************************************/
CosTask_t *_newTask(uint8_t prio, void * pData, void (*func) (CosTask_t *))
{  CosTask_t *pt;
   pt = (CosTask_t *)malloc(sizeof(CosTask_t));

   if(pt!=NULL)
   {  pt->lastActivationTime_Ticks  = _gettime_Ticks();
      pt->sleepTime_Ticks           = 0;  /* run asap */
      pt->state                     = TASK_STATE_READY;
      pt->prio                      = prio;
      pt->lineCnt                   = 0;    /* re-entry at start of function */
      pt->pData                     = pData;
      pt->func                      = func;
   }
   return pt;
}

/*---------------------------------------------------------------*/


/*!
********************************************************************
  @par Beschreibung
  Sortiert die lineare Liste nach Task Prioritaet. Die Task mit der hoechsten
  Prioritaet steht vorne in der Liste. Der root-Zeiger der Liste bleibt
  unveraendert, da hier nur die 'Nutzlast' der Knoten getauscht wird, und nicht
  die Knotenreihenfolge geaendert wird.

@see
@arg

@param root_pt - IN, Zeiger auf das erste Listenelement

@retval none

@par Code-Beispiel::
@verbatim
int main(void)
{   ...
    _sortLinearListPrio(root_pt);
    ...

    return 0;
}
@endverbatim
********************************************************************/
void _sortLinearListPrio(Node_t *root_pt)
{

    int noOfSwaps;
    CosTask_t *temp_pt;
    Node_t *pt;

    if (root_pt==NULL) return;  /* empty list */

    /* bubblesort */
    do
    {   noOfSwaps=0;
        pt = root_pt;
        while(pt->next_pt != NULL)  // run through linear list
        {   if((pt->task_pt)->prio < ((pt->next_pt)->task_pt)->prio)
            {   /* swap the 'payloads' of the nodes*/
                noOfSwaps++;
                temp_pt = pt->task_pt;  /* swap payload of nodes */
                pt->task_pt = (pt->next_pt)->task_pt;
                (pt->next_pt)->task_pt = temp_pt;
            }
            pt = pt->next_pt;
        }
    } while(noOfSwaps >0);
}
