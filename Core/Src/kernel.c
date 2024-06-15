#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "main.h"
#include "kernel.h"

extern void runFirstThread(void);

static thread thread1;
static uint32_t *msp_init = NULL;
static uint32_t *final_addr = NULL;
static uint32_t *last_stack_init = NULL;

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
      __set_PSP((uint32_t)thread1.sp);
      runFirstThread();
      break;
    default:
      break;
  }
}

uint32_t * alloc_thread(void)
{
  uint32_t *candidate_addr = (uint32_t*)((uint32_t)last_stack_init - THREAD_STACK_SIZE);
  if(candidate_addr >= final_addr)
  {
    return candidate_addr;
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
  last_stack_init = stack_ptr;

  *(--stack_ptr) = 1<<24;
  *(--stack_ptr) = (uint32_t)thread_function;
  for(int i = 0; i < 14; ++i)
  {
    *(--stack_ptr) = 0xA;
  }

  thread1.sp = stack_ptr;
  thread1.thread_function = thread_function;
  return true;
}

void osKernelInitialize(void)
{
  msp_init = *(uint32_t**)0x0;
  final_addr = (uint32_t*)(((uint32_t)msp_init - MAX_STACK_SIZE) + THREAD_STACK_SIZE);
  last_stack_init = msp_init;
}

void osKernelStart(void)
{
  __asm("SVC #0");
}
