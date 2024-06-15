/*
 * kernel.h
 *
 *  Created on: Jun 11, 2024
 *      Author: Derek Tang
 */

#ifndef INC_KERNEL_H_
#define INC_KERNEL_H_

#include <stdbool.h>

// defines
#define MAX_STACK_SIZE 0x4000
#define THREAD_STACK_SIZE 0x400
#define RUN_FIRST_THREAD 0x00

// thread context
typedef struct k_thread{
  uint32_t* sp;
  void (*thread_function)(void*);
} thread;

// function prototypes
bool osCreateThread(void (*thread_function)(void*));
void osKernelInitialize(void);
void osKernelStart(void);

#endif /* INC_KERNEL_H_ */
