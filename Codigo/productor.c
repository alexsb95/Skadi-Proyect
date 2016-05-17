/*Este es el programa productor byEli*/
/*No se complique, viva feliz: cc productor.c -o productor -lpthread*/
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define LIMINFLINEAS 1
#define LIMSUPLINEAS 10
#define LIMINFTIELIN 20
#define LIMSUPTIELIN 60
#define LIMINFTIEMPO 30
#define LIMSUPTIEMPO 60

/*Prototipos de funciones*/
int reservarMemoria (int, int);
int* vincularMemoria(int);
void desvincularMemoria (int, int*);
int getRandom(int, int);
int EscogerAlgoritmo();
void ImprimirMenu();
void *CorrerHilo(void *);
void errorFatal(char *);


void iniSemarofoMemoria();
void finiSemarofoMemoria();
void accederMemoria(int, int, int);

#define BUFFSIZE     5
pthread_mutex_t semaforoMemoria[BUFFSIZE];
int memoria[BUFFSIZE];

int main(int argc, char *argv[]){
    if (argc != 1) {
        fprintf(stderr, "Error: Ingresar los parametros. Forma correcta: ./%s\n", argv[0]);
        exit(1);
    }
    /*
    key_t llave = 5432;
    int shmId;
    int *datos;
    shmId = reservarMemoria(llave, atoi(argv[1]));
    datos = vincularMemoria(shmId);
    desvincularMemoria(shmId, datos);*/

    iniSemarofoMemoria();

    int contHilo = 0;
    pthread_t hiloId;
    while(1){
    	//creamos el hilo
    	if(pthread_create(&hiloId, NULL,  CorrerHilo, (void *) &contHilo) < 0)
            errorFatal("Error: No sve pudo crear el hilo");
        //dormimos un poco hasta que se cree el proximo
        sleep(getRandom(LIMINFLINEAS, LIMSUPLINEAS));
        contHilo++;
    }

    return 0;
}


void *CorrerHilo(void *pIdHilo){
    int idHilo = *(int*)pIdHilo;
    /*
	aqui se realiza el siguiente algoritmo:
	- se revisa si hay campo, si lo hay lo mete y se duerme para luego morir cuando termina, 
	-- sino hay campo, simpelmente muere tragicamente:(

	vamos a supone que se duerme el hilo y luego imprime un msj de que va a morir
    */

    int tiempoEspera = getRandom(LIMINFLINEAS, LIMSUPLINEAS);

    /*   Usa el algoritmo para buscar / represntado por -> */   int poscision = rand()%(BUFFSIZE-1);

    accederMemoria(idHilo, poscision, tiempoEspera);

    pthread_exit(NULL);
}

int getRandom(int pLimInf, int pLimSup){
	int r = (rand() % (pLimSup+1-pLimInf))+pLimInf;
	return r;
}

int EscogerAlgoritmo(){
	int opcion = 0;
	char str[10];
	while(opcion < 1 || opcion > 3){
		ImprimirMenu();
		scanf("%s", str);
		opcion = atoi(str);
	}
	return opcion;
}

void ImprimirMenu(){
	printf("---------------------------\n");
	printf("Por favor ingrese uno de los\nsiguientes numeros para escoger\nel algoritmo:\n");
	printf("1. First Fit\n");
	printf("2. Best Fit\n");
	printf("3. Worst Fit\n");
	printf("---------------------------\nOpcion: ");
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


void iniSemarofoMemoria (){

    int indice;
    for(indice=0; indice<BUFFSIZE; indice++){
         pthread_mutex_init(&semaforoMemoria[indice], NULL);
    }

     for(indice=0; indice<BUFFSIZE; indice++){
         memoria[indice]=0;
    }
}

void finiSemarofoMemoria(){
    int indice;
    for(indice=0; indice<BUFFSIZE; indice++){
        pthread_mutex_destroy(&semaforoMemoria[indice]);
    }

}
void accederMemoria(int pId, int pPoscision, int pTiempo){

    if(pthread_mutex_trylock (&semaforoMemoria[pPoscision])==0){
        
        printf("Se bloqueo el campo %i del buffer por el proceso %i \n", pPoscision, pId);

        // Modifica el valor a su id
        memoria[pPoscision]=pId;

        // Se duerme
        sleep(pTiempo);

        printf("Se des-bloqueo el campo %i del buffer por el proceso %i\n", pPoscision, pId);

        pthread_mutex_unlock (&semaforoMemoria[pPoscision]);

    }else
        printf(" No se pudo bloquear el campo %i del buffer por el proceso %i\n", pPoscision, pId);   

}