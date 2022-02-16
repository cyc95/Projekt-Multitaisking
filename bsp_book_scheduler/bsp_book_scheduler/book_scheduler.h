/*
 * book_scheduler.h
 *
 *  Created on: 01.10.2015
 *      Author: Forgber
 */

#ifndef BOOK_SCHEDULER_H_
#define BOOK_SCHEDULER_H_

/*
 * uses INT_Excep_TMR0_CMIA0 for timer interrupt!
 */

void Init_Book_Scheduler(void);
int  Add_Task(void (*task)(void), int time, int priority);
void Remove_Task(int task_number);
void Enable_Task(int task_number);
void Disable_Task(int task_number);
void Set_Task_Period(int task_number, int new_timer_val);
void Create_Idle_Task(void);
void Run_Book_Scheduler(void);

#endif /* BOOK_SCHEDULER_H_ */
