#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>


/*Prototipos de funciones*/
void errorFatal(char *);
int reservarMemoria (int, int);
int* vincularMemoria(int);
void desvincularMemoria (int, int*);
void imprimirDatoMemoria();

/*Rutinas de funciones*/
void errorFatal(char* pMensaje){
    printf("Error: %s .", pMensaje);
    exit(1);
}

int reservarMemoria (int pLlave, int pCantidad){
    int shmid;
    printf("pcantidad: %d\n", pCantidad);
    if ((shmid = shmget(pLlave, (sizeof(shmid) * pCantidad), 0777 | IPC_CREAT)) == -1)
        errorFatal("No se pudo reservar la memoria");
    return shmid;
}

int* vincularMemoria(int pShmId){
    int* shm;
    if ((shm = shmat(pShmId, NULL, 0)) == (int *) -1) 
        errorFatal("No se pudo vincular la memoria");
    return shm;
}

void desvincularMemoria (int pShmId, int* pShmDatos){
    if (shmdt(pShmDatos) == -1) 
        errorFatal("No se pudo desvincular la memoria");
    /*  Marca la memoria para ser removida  */
    shmctl(pShmId, IPC_RMID, NULL);
}

void imprimirDatoMemoria(int pShmIdDatos, int pShmIdBandera, int pShmIdTamano, int pCantidadLineas){
    printf("---------------------------\n");
    printf("+ Id memoria compartida: %d\n", pShmIdDatos);
    printf("+ Id bandera compartida: %d\n", pShmIdBandera);
    printf("+ Id tamanio compartida: %d\n", pShmIdTamano);
    printf("+ Total lineas reservadas: %d\n", pCantidadLineas);
    printf("+ Total bytes reservados: %d\n", (sizeof(int) * pCantidadLineas));
    printf("---------------------------\n");
}