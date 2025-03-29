#include "child.h"

// Esto lo saque de las tareas de EDD, los amo ayudantes de EDD
/* Retorna true si ambos strings son iguales */
static bool string_equals(char *string1, char *string2) {
  return !strcmp(string1, string2);
}


int main(int argc, char const *argv[])
{
  printf("Ta corriendo\n");
  printf("Proceso con id %d\n", getpid());
  ch_p* child = NULL; // Guardaremos la información del hijo en ese struct
  ch_p* new_child; // Para ir agregando nuevos hijos
  int cantidad_hijos = 0;
  pid_t pid;

  int time_max = -1;

  if (argc == 2) // Se agrego el parametro time_max
  {
    time_max = atoi(argv[1]);
  }
  
  while (1)
  {
    // Se espera a que el hijo termine
    int result;
    int status;
    if (child != NULL){
      ch_p* temp = child;
      printf("-------------------\n");
      while (temp != NULL){
        result = waitpid(temp->pid, &status, WNOHANG);
        if (result != 0){
          printf("Padre: El proceso pid  %i terminó, exit code %d\n", temp->pid, WEXITSTATUS(status));
          temp->exit_code = WEXITSTATUS(status);
        }
        else{
          printf("Padre: El proceso pdi %i aún no termina\n", temp->pid);
          temp->exit_code = -1;
        }
        temp = temp->next;
      }
      printf("-------------------\n");
    }
    
    
    char** input = read_user_input();
    char* path = input[1];
    char** args = &input[2];

    //printf("%s\n", path);
    
    // start <executable> <arg1> <arg2> ... <argn>
    if (string_equals(input[0], "start")){
      // checkear si existe
      
      struct stat buffer;
      if (stat(path, &buffer) != 0){
        printf("No se encontró el programa %s\n", path);
        continue;
      }      
      pid = fork();
      
      // perror("start");
      if (pid == 0){
        execv(path, args);
      }
      else if (pid > 0){
        // Se guardan los elementos del hijo
        if (cantidad_hijos == 0){
          child = child_init(pid, input[1]);
          if (child != NULL){
          }
        }
        else{
          new_child = child_init(pid, input[1]);
          append_child(child, new_child);
        }
        cantidad_hijos += 1;
      }
    }

    // info
    if (string_equals(input[0], "info")){
      // if (cantidad_hijos == 0){
      //   printf("No hay hijos\n");
      //   continue;
      // }
      
      printf("Mostrando los hijos del proceso %d que tiene %d hijos\n", getpid(), cantidad_hijos);
      printf("|  PID  | Executable | Exec Time | Exit code | Signal Value |\n");
      print_childs(child);
    }
    
    
    // timeout <time>
    if (string_equals(input[0], "timeout")){
      char* time_string = input[1];
      float time = atof(time_string);
      clock_t time_i = clock();
      clock_t tiempo_transcurrido;
      float tiempo_seg = 0;
      // Validar si existen procesos en ejecucion
      if (cantidad_hijos == 0){
        printf("No existen procesos en ejecución. Timeout no se puede ejecutar\n");
      }
      else{
        printf("Vamos a esperar %f segundos\n", time);
        while(tiempo_seg < time){
          tiempo_transcurrido = clock() - time_i;
          tiempo_seg = (float) tiempo_transcurrido/CLOCKS_PER_SEC;
        }
        printf("Tiempo cumplido! (pasaron %f segundos)\n", tiempo_seg);
        sigterm_childs(child);

      }

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

