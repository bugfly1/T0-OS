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



// Esto lo saque de las tareas de EDD, los amo ayudantes de EDD
/* Retorna true si ambos strings son iguales */
static bool string_equals(char *string1, char *string2) {
  return !strcmp(string1, string2);
}


typedef struct child_process
{
  struct child_process* next;
  pid_t pid;
  char* exec_name;
  clock_t init;
  time_t end;
  int exit_code;
  int signal_value;
} ch_p;

ch_p* child_init(pid_t pid, char* exec_name, clock_t init){
  ch_p* child = malloc(sizeof(ch_p));
  child->pid = pid;
  child->exec_name = exec_name;
  child->init = init;
  child->next = NULL;
  // printf("Se creó un hijo con pid = %d\n", child->pid);
  // printf("El nombre del ejecutable es %s\n", child->exec_name);
  return child;
} 

void append_child(ch_p* child, ch_p* new_child){
  ch_p* temp = child;
  while (temp->next != NULL){
    temp = temp->next;
  }
  temp->next = new_child;
  return;
}

void print_childs(ch_p* child){
  ch_p* temp = child;
  double total_t;
  while (temp != NULL){
    printf("PID: %d\n", temp->pid);
    printf("Nombre del ejecutable: %s\n", temp->exec_name);
    total_t = (double) (clock() - temp->init) / CLOCKS_PER_SEC;
    printf("Tiempo de ejecución: %f\n", total_t);
    temp = temp->next;
  }
  return;
}


void destroy_child(ch_p* child){
  ch_p* temp;
  while (child != NULL){
    temp = child->next;
    printf("Padre: Liberando el hijo de pid: %d\n", child->pid);
    kill(child->pid, SIGTERM);
    free(child);
    child = temp;
    }
  return;
}


// execv(path, argv)

int main(int argc, char const *argv[])
{
  printf("Ta corriendo\n");
  printf("Proceso con id %d\n", getpid());
  ch_p* child; // Guardaremos la información del hijo en ese struct
  ch_p* new_child; // Para ir agregando nuevos hijos
  int cantidad_hijos = 0;

  while (1)
  {
    char** input = read_user_input();
    char** args = &input[2];
    
    // start <executable> <arg1> <arg2> ... <argn>
    if (string_equals(input[0], "start")){
      // printf("Recibió los args %s\n", *args);

      // checkear si existe
      struct stat buffer;
      if (stat(input[1], &buffer) == -1)
      {
        printf("no existe\n");
        continue;
      }
      
      pid_t pid = fork();
      
      perror("start");

      if (pid == 0){
        execv(input[1], args);
      }
      else if (pid > 0){
        // Se guardan los elementos del hijo
        if (cantidad_hijos == 0){
          child = child_init(pid, input[1], clock());
        }
        else{
          new_child = child_init(pid, input[1], clock());
          append_child(child, new_child);
        }
        cantidad_hijos += 1;
      }
    }


    // info
    if (string_equals(input[0], "info")){
      printf("funciona\n");
      printf("Mostrando los hijos del proceso %d que tiene %d hijos\n", getpid(), cantidad_hijos);
      print_childs(child);
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
        destroy_child(child);
      break;
    }

    free_user_input(input);
  }
  

  
}

