#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>

#include "ManejarMemoria.h"

void recorrerMemoria(int*, int);

int main(int argc, char *argv[]){

	key_t llaveDatos = 5432;
    key_t llaveTamano = 6543;
    key_t llaveBandera = 7654;
    
    int shmIdDatos;
    int shmIdTamano;
    int shmIdBandera;

    int *datos, *ptroDatos;
    int *tamano;
    int *bandera; 

    /*      Solicita la memoria del tamano       */
    shmIdTamano = reservarMemoria(llaveTamano, 1);
    tamano = vincularMemoria(shmIdTamano);

     /*      Solicita la memoria de la bandera       */
    shmIdBandera = reservarMemoria(llaveBandera, 1);
    bandera = vincularMemoria(shmIdBandera);

    /*      Solicita la memoria de los datos       */
    shmIdDatos = reservarMemoria(llaveDatos, (int)*tamano);
    datos = vincularMemoria(shmIdDatos);


    /*      Imprimimos todo lo que obtuvimos de arriba      */
    imprimirDatoMemoria(shmIdDatos, shmIdBandera, shmIdTamano, (int)*tamano);

    recorrerMemoria(datos,(int)*tamano);
}

void  recorrerMemoria(int* pDatos, int pTamano){
	int contador;

	for(contador=0; contador<pTamano;contador++){
		printf("#------------------#\n");
		printf("# \t%d\t #\n",*pDatos );
		printf("#------------------#\n");
		pDatos ++;
	}

}