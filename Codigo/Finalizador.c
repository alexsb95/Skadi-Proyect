/*Este es el programa productor byEli*/
/*No se complique, viva feliz: cc productor.c -o productor -lpthread*/
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>

#include "ManejarMemoria.h"

#define MEMORIABLOQUEADO 500

int main(int argc, char *argv[]){
    if (argc != 1) {
        fprintf(stderr, "Error: Ingresar los parametros. Forma correcta: ./%s\n", argv[0]);
        exit(1);
    }
    key_t llaveDatos = 5432;
    key_t llaveTamano = 6543;
    key_t llaveBandera = 7654;
    key_t llaveBloqueado = 6667;
    key_t llaveAccesoMem = 4565;
    
    int shmIdDatos;
    int shmIdTamano;
    int shmIdBandera;
    int shmIdBloqueado;
    int shmIdAccesoMem;

    int *datos;
    int *tamano;
    int *bandera;
    int *bloqueado;
    int *accesoMemoria;

    /*      Solicita la memoria del tamano       */
    shmIdTamano = reservarMemoria(llaveTamano, 1);
    tamano = vincularMemoria(shmIdTamano);

     /*      Solicita la memoria de la bandera       */
    shmIdBandera = reservarMemoria(llaveBandera, 1);
    bandera = vincularMemoria(shmIdBandera);

    /*      Solicita la memoria de los datos       */
    shmIdDatos = reservarMemoria(llaveDatos, (int)*tamano);
    datos = vincularMemoria(shmIdDatos);

    /*      Solicita la memoria de los bloqueados       */
    shmIdBloqueado = reservarMemoria(llaveBloqueado, MEMORIABLOQUEADO);
    bloqueado = vincularMemoria(shmIdBloqueado);

    /*      Solicita la memoria de el proceso que acede a memoria       */
    shmIdAccesoMem = reservarMemoria(llaveAccesoMem, 1);
    accesoMemoria = vincularMemoria(shmIdAccesoMem);

    *bandera = 0;

    desvincularMemoria(shmIdDatos, datos);
    desvincularMemoria(shmIdBandera, bandera);
    desvincularMemoria(shmIdTamano, tamano);
    desvincularMemoria(shmIdBloqueado, bloqueado);
    desvincularMemoria(shmIdBloqueado, accesoMemoria);

    return 0;
}
