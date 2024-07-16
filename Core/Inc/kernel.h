/*
 * kernel.h
 *
 *  Created on: Jun 11, 2024
 *      Author: Derek Tang
 */

#ifndef INC_KERNEL_H_
#define INC_KERNEL_H_

#include <stdbool.h>

// registers
#define SHPR2 *(uint32_t*)0xE000ED1C //for setting SVC priority, bits 31-24
#define SHPR3 *(uint32_t*)0xE000ED20 // PendSV is bits 23-16
#define _ICSR *(uint32_t*)0xE000ED04 //This lets us trigger PendSV

// stack management
#define MAX_STACK_SIZE 0x4000
#define THREAD_STACK_SIZE 0x400

// SVC numbers
#define RUN_FIRST_THREAD 0x00
#define YIELD 0x01

// thread context
typedef struct k_thread{
  uint32_t timeslice;
  uint32_t runtime;
  uint32_t* sp;
  void (*thread_function)(void*);
} thread;

// global variables
extern uint32_t thread_idx;
extern thread *threads;

// function prototypes
bool osCreateThread(void (*thread_function)(void*), void *args);
bool osCreateThreadWithDeadline(void (*thread_function)(void*), void *args, uint32_t timeslice);
void osKernelInitialize(void);
void osKernelStart(void);
void osYield(void);

#endif /* INC_KERNEL_H_ */
