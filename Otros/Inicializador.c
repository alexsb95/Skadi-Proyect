#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define DATA_SIZE 8  /* make it a 1K shared memory segment */

int reservarMemoria (int, int);
char* vincularMemoria(int);
void desvincularMemoria (int, char*);

int main(int argc, char *argv[])
{
    key_t llave = 2032;
    int shmId;
    char *datos;


    if (argc != 2) {
        fprintf(stderr, "Error: Ingresar la cantidad de lineas\n");
        exit(1);
    }

    shmId = reservarMemoria(llave, atoi(argv[1]));

    datos = vincularMemoria(shmId);

    desvincularMemoria(shmId, datos);

    return 0;
}

void errorFatal(char* pMensaje){
        printf("Error: %s .", pMensaje);
        exit(1);
}

int reservarMemoria (int pLlave, int pCantidad){
    int shmid;
    if ((shmid = shmget(pLlave, DATA_SIZE*pCantidad, 0644 | IPC_CREAT)) == -1) {
        errorFatal("No se pudo reservar la memoria");
    }
    return shmid;
}

char* vincularMemoria(int pShmId){
    char* shm;
    if ((shm = shmat(pShmId, NULL, 0)) == (char *) -1) {
        errorFatal("No se pudo vincular la memoria");
    }
    return shm;
}

void desvincularMemoria (int pShmId, char* pShmDatos){
    if (shmdt(pShmDatos) == -1) {
        errorFatal("No se pudo desvincular la memoria");
    }

    /*  Marca la memoria para ser removida  */
    shmctl(pShmId, IPC_RMID, NULL);
}