#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <stdio.h>

#include "main.h"
#include "kernel.h"

extern void runFirstThread(void);

static uint32_t thread_idx = 0;
static uint32_t thread_count = 0;
static uint32_t thread_count_max = 0;

static uint32_t *msp_init = NULL;
static uint32_t *last_stack_init = NULL;
static thread *threads;

void SVC_Handler_Main( unsigned int *svc_args )
{
  unsigned int svc_number;
  /*
  * Stack contains:
  * r0, r1, r2, r3, r12, r14, the return address and xPSR
  * First argument (r0) is svc_args[0]
  */
  svc_number = ( ( char * )svc_args[ 6 ] )[ -2 ] ;
  switch( svc_number )
  {
    case RUN_FIRST_THREAD:
      __set_PSP((uint32_t)threads[thread_idx].sp);
      runFirstThread();
      break;

    case YIELD:
      //Pend an interrupt to do the context switch
      _ICSR |= 1<<28;
      __asm("isb");
      break;

    default:
      break;
  }
}

void osSched()
{
  threads[thread_idx].sp = (uint32_t*)(__get_PSP() - 8*4);
  thread_idx = (thread_idx + 1) % thread_count;
  __set_PSP((uint32_t)threads[thread_idx].sp);
}

uint32_t * alloc_thread(void)
{
  if(thread_count < thread_count_max)
  {
    thread_count++;
    return (uint32_t*)((uint32_t)last_stack_init - THREAD_STACK_SIZE);
  }
  else
  {
    return NULL;
  }
}

bool osCreateThread(void (*thread_function)(void*))
{
  uint32_t *stack_ptr = alloc_thread();
  if(stack_ptr == NULL)
  {
    return false;
  }

  uint32_t *tmp = stack_ptr;
  last_stack_init = stack_ptr;

  *(--tmp) = 1<<24;
  *(--tmp) = (uint32_t)thread_function;
  for(int i = 0; i < 14; ++i)
  {
    *(--tmp) = 0xA;
  }

  thread tmp_context = {tmp, thread_function};
  threads[thread_count - 1] = tmp_context;
  return true;
}

void osKernelInitialize(void)
{
  SHPR3 |= 0xFE << 16; //shift the constant 0xFE 16 bits to set PendSV priority
  SHPR2 |= 0xFDU << 24; //Set the priority of SVC higher than PendSV

  thread_count_max = (MAX_STACK_SIZE / THREAD_STACK_SIZE) - 1;
  threads = malloc(sizeof(thread) * thread_count_max);

  msp_init = *(uint32_t**)0x0;
  last_stack_init = msp_init;
}

void osKernelStart(void)
{
  __asm("SVC #0");
}

void osYield(void)
{
  __asm("SVC #1");
}

