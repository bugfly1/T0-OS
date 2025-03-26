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



// Esto lo saque de las tareas de EDD, los amo ayudantes de EDD
/* Retorna true si ambos strings son iguales */
static bool string_equals(char *string1, char *string2) {
  return !strcmp(string1, string2);
}


typedef struct child_process
{
  pid_t pid;
  char* exec_name;
  time_t init;
  time_t end;
  int exit_code;
  int signal_value;
} ch_p;

// execv(path, argv)

int main(int argc, char const *argv[])
{
  printf("Ta corriendo\n");

  while (1)
  {
    char** input = read_user_input();
    char** args = &input[2];
    
    // start <executable> <arg1> <arg2> ... <argn>
    if (string_equals(input[0], "start")){

      // checkear si existe
      struct stat buffer;
      if (stat(input[1], &buffer) == -1)
      {
    
        printf("no existe\n");
        continue;
      }

      
      pid_t pid = fork();
      perror("Failed\n");

      if (pid == 0)
      {
        execv(input[1], args);
      }
    }



    // info
    if (string_equals(input[0], "info")){
      printf("funciona\n");
    }


    
    // timeout <time>
    if (string_equals(input[0], "timeout")){
      int time = input[1];
      // Validar si existen procesos en ejecucion

      // Si no, informar
        // printf("No hay procesos en ejecucion. Timeout no se puede ejecutar");
        // continue;

      // Si hay
        // wait(time)
        // Si no finaliza imprimir wea
      
    }

    // quit
    if (string_equals(input[0], "quit")){
      printf("bye bye\n");

      // Enviar SIGINT
      break;
    }

    free_user_input(input);
  }
  

  
}

