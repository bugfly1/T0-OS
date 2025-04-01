#include "child.h"

// Variables globales para usar en sigchld handler
ch_p* child = NULL; // Guardaremos la información del hijo en ese struct
ch_p* new_child; // Para ir agregando nuevos hijos

// Variables globales para ctrl C handler
bool loop = true;
bool closeShell = false;


void sigchld_handler(int signal){
  printf("Ejecutando sigchld handler\n");
  while (1){
    int status;
    pid_t pid = waitpid(-1, &status, WNOHANG);
    if (pid <= 0){
      break;
    }
    if (WIFEXITED(status) || WIFSIGNALED(status)){
      printf("Se terminó el proceso %d con term signal %d\n", pid, WTERMSIG(status));
      if (child != NULL){
        // Recorremos hasta encontrar el hijo que tiene este pid
        ch_p* temp = child;
        while (temp != NULL){
          if (temp->pid == pid){
            temp->tiempo_final = time(NULL);
            temp->exit_code = WEXITSTATUS(status);
            temp->signal_value = WTERMSIG(status);
          }
          temp = temp->next;
        }
      }
      else{
        printf("Esto no debería pasar\n");
      }
    }
  }
}


// Esto lo saque de las tareas de EDD, los amo ayudantes de EDD
/* Retorna true si ambos strings son iguales */
static bool string_equals(char *string1, char *string2) {
  return !strcmp(string1, string2);
}

// Simplemente deja que pase por la parte de quit y que salga
void sigctrlC_handler(int sig){
  loop = false;
  closeShell = true;
}

int main(int argc, char const *argv[])
{
  signal(SIGCHLD, sigchld_handler);
  signal(SIGINT, sigctrlC_handler);
  
  printf("Ta corriendo\n");
  printf("Proceso con id %d\n", getpid());
  int cantidad_hijos = 0;
  pid_t pid;
  ch_p* temp;
  int kill_ok;

  int time_max = -1;

  if (argc == 2) // Se agrego el parametro time_max
  {
    time_max = atoi(argv[1]);
  }

  
  while (loop)
  {

    // Revisar el timemax
    if (time_max != -1){
      printf("Time max %i\n", time_max);
      temp = child;
      time_t tiempo_f;
      time_t tiempo_proceso;
      while (temp != NULL){
        if (temp->exit_code == -1){
          tiempo_f = time(NULL);
        }
        else{
          tiempo_f = temp->tiempo_final;
        }
        tiempo_proceso = tiempo_f - temp->tiempo_inicial;
        // Revisamos que el tiempo de proceso sea mayor Y que no haya terminado
        if (tiempo_proceso > time_max && temp->exit_code == -1){
          if (!temp->senal_enviada){
            // Significa que aún no se le ha enviado una señal para que termine
            printf("El proceso %i lleva %li segundos!!\n", temp->pid, tiempo_proceso);
            printf("Usando SIGTERM en el proceso %d\n", temp->pid);
            kill_ok = kill(SIGTERM, temp->pid);
            if (kill_ok == 0){
              printf("Funcionó\n");
            }
            else{
              printf("Error\n");
            }
            temp->senal_enviada = true;
            temp->inicio_cuenta_regresiva = time(NULL);
          }
          else{
            // Significa que ya se le envió la señal y no ha terminado
            if (time(NULL) - temp->inicio_cuenta_regresiva > 5){
              printf("El proceso %i aún no termina y pasaron %li segundos\n", temp->pid, time(NULL) - temp->inicio_cuenta_regresiva);
              printf("Usando SIGKILL\n");
              kill_ok = kill(SIGKILL, temp->pid);
              if (kill_ok == 0){
                printf("Funcionó\n");
              }
              else{
                printf("Error\n");
              }
            }
          }
        }
        temp = temp->next;
      }
    }
    
    
    char** input = read_user_input();
    char* path = input[1];
    
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
        execv(path, &input[1]);
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
      if (cantidad_hijos == 0){
        printf("No hay hijos\n");
        continue;
      }
      printf("Mostrando los hijos del proceso %d que tiene %d hijos\n", getpid(), cantidad_hijos);
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

    }
      
    // quit
    if (string_equals(input[0], "quit") || closeShell){
      printf("bye bye\n");
      
      // Enviar SIGINT
      destroy_child(child);
      free_user_input(input);
      break;
    }

    free_user_input(input);
  }
  

  
}

