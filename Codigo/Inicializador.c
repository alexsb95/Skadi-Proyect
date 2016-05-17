/*Inicializador byEli*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/*Prototipos de funciones*/
int reservarMemoria (int, int);
int* vincularMemoria(int);
void desvincularMemoria (int, int*);
void errorFatal(char *);

int main(int argc, char *argv[]){
    if (argc != 2) {
        fprintf(stderr, "Error: Ingresar los parametros. Forma correcta: %s <cantLineas>\n", argv[0]);
        exit(1);
    }
    key_t llave = 5432; 
    int shmId;

    int *datos;
    int *tamano;
    char *bandera; 

    shmId = reservarMemoria(llave, atoi(argv[1]));
    datos = vincularMemoria(shmId);
    printf("---------------------------\n");
    printf("+ Id memoria compartida: %d\n", shmId);
    printf("+ Id llave compartida: %d\n", llave);
    printf("+ Total lineas reservadas: %s\n", argv[1]);
    printf("+ Total bytes reservados: %d\n", (sizeof(int) * (atoi(argv[1]) + 1)));
    printf("---------------------------\n");

    desvincularMemoria(shmId, datos);
    return 0;
}

void errorFatal(char* pMensaje){
        printf("Error: %s .", pMensaje);
        exit(1);
}

int reservarMemoria (int pLlave, int pCantidad){
    int shmid;
    if ((shmid = shmget(pLlave, (sizeof(shmid) * (pCantidad + 1)), 0644 | IPC_CREAT)) == -1)
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