#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
/* include synch.h for semaphore 
   we have to check if semaphore works or not.
   for now, it seems like that it works.*/
#include "threads/synch.h"
#include "filesys/file.h"
#include "filesys/inode.h"
static struct semaphore syscall_sema;

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  sema_init (&syscall_sema, 1);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f) 
{
  int syscall_number;
  void *argument_1;
  void *argument_2;
  void *argument_3;
  /* Number of arguments that are used depends on syscall number.
     Max number of arguments is 3. */
  
  syscall_number = *(int *)(f->esp);
  /* treat all syscall region as a critical section 
     we have to sema_up if the handler returns in the switch case
     (maybe later we make all syscalls, we may need only one sema_up in the last*/
  sema_down (&syscall_sema);
  printf ("system call! syscall number is : %d, ", syscall_number);
  switch(syscall_number)
  {
    case SYS_HALT:
      printf("SYS_HALT\n");
      shutdown_power_off ();
      break;
    case SYS_EXIT:
      printf("SYS_EXIT\n");
      argument_1 = (f->esp)+4;
      f->eax = *(int *)argument_1;
      /* 
      waits for all of thread_current () 's child processes.
      it can be implemented with list traversal. */
      /*
      search thread_current () from (parent_thread -> child_list) 
      and modify the exit status. because parent needs it. 
      maybe parent_thread's semaphore up. */
      process_exit ();
      sema_up (&syscall_sema);
      return;
      break;
    case SYS_EXEC:
      printf("SYS_EXEC\n");
      argument_1 = (f->esp)+4;
      printf("exec function with argument '%s'\n", *(char **)argument_1); 
      process_execute (*(char **)argument_1);
      sema_up (&syscall_sema);
      return;
      break;
    case SYS_WAIT:
      printf("SYS_WAIT\n");
      /*  if input tid is not child, return -1
          if already waited tid, return -1
          if already exited tid, return -1 *
      /*  PINTOS : A THREAD PER PROCESS 
          we can treat a thread as a process.
          so we can add a semaphore that makes it wait in thread's struct 
          initialize it in thread_init, and we can sema_down () for wait.
          in here, sema_down (thread_sema)
          and we can make sema_up whenever the child process exits. */
      break;
    case SYS_CREATE:
      printf("SYS_CREATE\n");
      break;
    case SYS_REMOVE:
      printf("SYS_REMOVE\n");
      break;
    case SYS_OPEN:
      printf("SYS_OPEN\n");
      break;
    case SYS_FILESIZE:
      printf("SYS_FILESIZE\n");
      break;
    case SYS_READ:
      printf("SYS_READ\n");
      break;
    case SYS_WRITE:
      printf("SYS_WRITE\n");
      argument_1 = (f->esp)+4;
      argument_2 = (f->esp)+8;
      argument_3 = (f->esp)+12;
      if(*(int *)argument_1 == 1)
      {
      /* Fd 1 writes to the console. 
         breaks down the buffer by 256 bytes.*/
        while(*(size_t *)argument_3 > 256)
        {
            putbuf (*(char **)argument_2, 256);
            *(char **)argument_2 += 256;
            *(size_t *)argument_3 -= 256;
        }
        putbuf (*(char **)argument_2, *(size_t *)argument_3);
      }
      sema_up (&syscall_sema);
      return;
      break;
    case SYS_SEEK:
      printf("SYS_SEEK\n");
      break;
    case SYS_TELL:
      printf("SYS_TELL\n");
      break;
    case SYS_CLOSE:
      printf("SYS_CLOSE\n");
      break;
  }
  /* treat all syscall region as a critical section */
  sema_up (&syscall_sema);
  thread_exit ();
}
