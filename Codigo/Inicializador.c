#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "ManejarMemoria.h"
#include "ManejarArchivo.h"

/*Prototipos de funciones*/


int main(int argc, char *argv[]){
    if (argc != 2) {
        fprintf(stderr, "Error: Ingresar los parametros. Forma correcta: %s <cantLineas>\n", argv[0]);
        exit(1);
    }

    /*      Keys para reservar los segmentos de memoria     */
    key_t llaveDatos = 5432;
    key_t llaveTamano = 6543;
    key_t llaveBandera = 7654;
    
    int shmIdDatos;
    int shmIdTamano;
    int shmIdBandera;

    int *datos, *ptroDatos;
    int *tamano;
    int *bandera; 

    shmIdDatos = reservarMemoria(llaveDatos, atoi(argv[1]));
    datos = vincularMemoria(shmIdDatos);

    shmIdTamano = reservarMemoria(llaveTamano, 1);
    tamano = vincularMemoria(shmIdTamano);

    shmIdBandera = reservarMemoria(llaveBandera, 1);
    bandera = vincularMemoria(shmIdBandera);

    imprimirDatoMemoria(shmIdDatos, shmIdBandera, shmIdTamano, atoi(argv[1]));

    *bandera = 1;
    *tamano = atoi(argv[1]);

    crearArchivo("Bitacora.txt");

    modificarArchivo("Bitacora.txt", " Hola soy alex sanchez");

    return 0;
}