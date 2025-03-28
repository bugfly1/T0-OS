#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv){
    for (int i = 0; i < argc; i++){
        // printf("Argumento %i: %s\n", i, argv[i]);
    }
    // printf("Ejecutando while...\n");
    // while(1){
    // }
    clock_t start_time = clock();
    clock_t tiempo_transcurrido;
    float tiempo_seg = 0;
    while (tiempo_seg < 60){
        tiempo_transcurrido = clock() - start_time;
        tiempo_seg = tiempo_transcurrido/CLOCKS_PER_SEC;
    }
    printf("Terminé\n");
    return 0;
}