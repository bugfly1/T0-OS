#pragma once
#include "../input_manager/manager.h"


#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */ 
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <sys/wait.h>   /* Wait for Process Termination */
#include <stdlib.h>     /* General Utilities */

#include <sys/stat.h>
#include <signal.h>

#include <stdbool.h>
#include <string.h>

#include <time.h>



typedef struct child_process
{
  struct child_process* next;
  pid_t pid;
  char* exec_name;
  time_t tiempo_inicial;
  time_t tiempo_final;
  time_t inicio_cuenta_regresiva;
  int exit_code;
  int signal_value;
  bool senal_enviada;
} ch_p;

ch_p* child_init(pid_t pid, char* exec_name);
void append_child(ch_p* child, ch_p* new_child);
void print_childs(ch_p* child);
void destroy_child(ch_p* child);
void sigterm_childs(ch_p* child);

