#include <errno.h>
#include <sys/sem.h>

union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
};

/*Prototipos de funciones*/
int crearSemaforoMemoria (key_t);
void destruirSemaforoMemoria(int);
void obtenerSemaforoMemoria (int);
void liberarSemaforoMemoria (int);
int copiarSemaforo(key_t);
int ultimoProceso(int);

/*Rutinas de funciones*/
int crearSemaforoMemoria (key_t pKey){
    int semaforoId;
    struct sembuf semConfiguracion;

   	semConfiguracion.sem_op = 1; 
   	semConfiguracion.sem_num = 0;
   	semConfiguracion.sem_flg = 0;

   	/*		Se crea el semaforo 		*/
	semaforoId = semget(pKey, 1, 0666 | IPC_CREAT);

	if (semaforoId >= 0){
		/*		Intenta de liberar el semaforo 		*/
		if (semop(semaforoId, &semConfiguracion, 1) == -1) {
		    printf("Error: No se pudo inicializar el semaforo. %i \n",semaforoId);
		    
		    /*		Elimina el semaforo		*/
		    semctl(semaforoId, 0, IPC_RMID); 
		    return -1;
		}
		return semaforoId;
	}else
		printf("Error: No se pudo crear el semaforo.\n");
}

void obtenerSemaforoMemoria (int pSemaforoId){

	struct sembuf semConfiguracion;
    
    semConfiguracion.sem_num = 0;
    semConfiguracion.sem_op = -1;  /* set to allocate resource */
    semConfiguracion.sem_flg = SEM_UNDO;

	if (semop(pSemaforoId, &semConfiguracion, 1) == -1) {
        printf("Error: No se pudo bloquear el semarofo\n");
    }

}

void liberarSemaforoMemoria (int pSemaforoId){

	struct sembuf semConfiguracion;
    
    semConfiguracion.sem_num = 0;
    semConfiguracion.sem_op = 1; /* free resource */
    semConfiguracion.sem_flg = SEM_UNDO;

    if (semop(pSemaforoId, &semConfiguracion, 1) == -1) {
        printf("Error: No se pudo desbloquear el semarofo\n");
    }
}


void destruirSemaforoMemoria(int pSemaforoId){
	union semun arg;
        
        /* remove it: */
    if (semctl(pSemaforoId, 0, IPC_RMID, arg) == -1) {
        printf("Error: No se pudo destruir el semarofo\n");
    }
}

int copiarSemaforo(key_t pKey){
    /*      Obtener un semaforo existente       */
    int semaforoId=0;
    if ((semaforoId = semget(pKey, 1, 0)) == -1) {
        printf("Error: No se pudo obtener el semaforo. %i \n",semaforoId);
        return -1;
    }
    return semaforoId;
}

int ultimoProceso(int pSemaforoId){
    union semun arg;
    
    arg.val = 0;

    return  semctl(pSemaforoId, 0, GETPID, arg);
}