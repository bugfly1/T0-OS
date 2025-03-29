#include "child.h"


ch_p* child_init(pid_t pid, char* exec_name){
    char* copy = calloc(64, sizeof(char));
    strcpy(copy, exec_name);

    ch_p* child = malloc(sizeof(ch_p));
    child->pid = pid;
    child->exec_name = copy;
    child->tiempo_inicial = time(NULL);
    child->tiempo_final = time(NULL);
    child->inicio_cuenta_regresiva = time(NULL);
    child->senal_enviada = false;
    child->exit_code = -1;
    child->signal_value = -1;
    child->next = NULL;


    // printf("Se creó un hijo con pid = %d\n", child->pid);
    printf("El nombre del ejecutable es %s\n", child->exec_name);
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
    time_t tiempo_f;
    while (temp != NULL){       
        if (temp->exit_code == -1){
        tiempo_f = time(NULL);
        }
        else{
        tiempo_f = temp->tiempo_final;
        }

        
        // Los headers son de tamano
        // 7|12|11|11

        printf("| %-5d | %-10s |    %-6ld |    %-6d |      %-7d |\n", 
            temp->pid, temp->exec_name, tiempo_f - temp->tiempo_inicial, temp->exit_code, temp->signal_value);
        
        
        temp = temp->next;
        
    }
    return;
}

void destroy_child(ch_p* child){
    ch_p* temp;
    while (child != NULL){
        temp = child->next;
        printf("Padre: Liberando memoria del hijo de pid: %d\n", child->pid);
        free(child->exec_name);
        free(child);
        child = temp;
        }
    return;
}

void sigterm_childs(ch_p* child){
    ch_p* temp;
    int result;
    int status;
    while (child != NULL){
        temp = child->next;
        if (child->exit_code == -1){
        kill(child->pid, SIGTERM);
        result = waitpid(child->pid, &status, WNOHANG);
        child->exit_code = WEXITSTATUS(status);
        child->tiempo_final = time(NULL);
        if (WIFSIGNALED(status)) child->signal_value = WTERMSIG(status);

        printf("PID: %d nombre: %s tiempo: %ld exit_code: %d signal_value: %d\n", child->pid, child->exec_name, time(NULL)-child->tiempo_inicial, child->exit_code, child->signal_value);
        }
        child = temp;
    }
}


  