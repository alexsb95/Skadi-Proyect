/*Este es el programa productor byEli*/
/*No se complique, viva feliz: cc productor.c -o productor -lpthread*/
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>

/*Prototipos de funciones*/
int reservarMemoria (int, int);
int* vincularMemoria(int);
void desvincularMemoria (int, int*);
void errorFatal(char *);

int main(int argc, char *argv[]){
    if (argc != 1) {
        fprintf(stderr, "Error: Ingresar los parametros. Forma correcta: ./%s\n", argv[0]);
        exit(1);
    }
    key_t llaveDatos = 5432;
    key_t llaveTamano = 6543;
    key_t llaveBandera = 7654;
    
    int shmIdDatos;
    int shmIdTamano;
    int shmIdBandera;

    int *datos, *ptroDatos;
    int *tamano;
    int *bandera; 

    shmIdTamano = reservarMemoria(llaveTamano, 1);
    tamano = vincularMemoria(shmIdTamano);

    shmIdBandera = reservarMemoria(llaveBandera, 1);
    bandera = vincularMemoria(shmIdBandera);

    shmIdDatos = reservarMemoria(llaveDatos, (int)*tamano);
    datos = vincularMemoria(shmIdDatos);

    *bandera = 0;

    desvincularMemoria(shmIdDatos, datos);
    desvincularMemoria(shmIdBandera, bandera);
    desvincularMemoria(shmIdTamano, tamano);

    return 0;
}

void errorFatal(char* pMensaje){
        printf("Error: %s .\n", pMensaje);
        exit(1);
}

int reservarMemoria (int pLlave, int pCantidad){
    int shmid;
    if ((shmid = shmget(pLlave, (sizeof(shmid) * pCantidad), 0644 | IPC_CREAT)) == -1)
        errorFatal("No se pudo reservar la memoria\n");
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