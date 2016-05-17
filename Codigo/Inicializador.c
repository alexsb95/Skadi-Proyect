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
    key_t llaveDatos = 5432;
    key_t llaveTamano = 6543;
    key_t llaveBandera = 7654;
    
    int shmIdDatos;
    int shmIdTamano;
    int shmIdBandera;

    int *datos;
    int *tamano;
    int *bandera; 

    shmIdDatos = reservarMemoria(llaveDatos, atoi(argv[1]));
    datos = vincularMemoria(shmIdDatos);

    shmIdTamano = reservarMemoria(llaveTamano, 1);
    tamano = vincularMemoria(shmIdTamano);

    shmIdBandera = reservarMemoria(llaveBandera, 1);
    bandera = vincularMemoria(shmIdBandera);

    printf("---------------------------\n");
    printf("+ Id memoria compartida: %d\n", shmIdDatos);
    printf("+ Id bandera compartida: %d\n", shmIdBandera);
    printf("+ Id tamanio compartida: %d\n", shmIdTamano);
    printf("+ Total lineas reservadas: %s\n", argv[1]);
    printf("+ Total bytes reservados: %d\n", (sizeof(int) * atoi(argv[1])));
    printf("---------------------------\n");

    *bandera = 1;
    *tamano = atoi(argv[1]);

     while((int)*bandera == 1){
        sleep(1);
    }

    desvincularMemoria(shmIdDatos, datos);
    desvincularMemoria(shmIdBandera, tamano);
    desvincularMemoria(shmIdTamano, bandera );
    return 0;
}

void errorFatal(char* pMensaje){
        printf("Error: %s .", pMensaje);
        exit(1);
}

int reservarMemoria (int pLlave, int pCantidad){
    int shmid;
    if ((shmid = shmget(pLlave, (sizeof(shmid) * pCantidad), 0644 | IPC_CREAT)) == -1)
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