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

    int *datos;
    int *tamano;
    int *bandera; 

    shmIdTamano = reservarMemoria(llaveTamano, 1);
    tamano = vincularMemoria(shmIdTamano);

    shmIdBandera = reservarMemoria(llaveBandera, 1);
    bandera = vincularMemoria(shmIdBandera);

    printf("Tamano: %d\n", (int)*tamano);
    printf("Bandera: %d\n", (int)*bandera);

    shmIdDatos = reservarMemoria(llaveDatos, (int)*tamano);
    datos = vincularMemoria(shmIdDatos);

    /*Imprimimos todo lo que obtuvimos de arriba*/
    printf("---------------------------\n");
    printf("+ Id memoria compartida: %d\n", shmIdDatos);
    printf("+ Id bandera compartida: %d\n", shmIdBandera);
    printf("+ Id tamanio compartida: %d\n", shmIdTamano);
    printf("+ Total lineas reservadas: %d\n", (int)*tamano);
    printf("---------------------------\n");

    int contHilo = 0;
    pthread_t hiloId;
    while((int)*bandera == 1){
    	//creamos el hilo
    	if(pthread_create(&hiloId, NULL,  CorrerHilo, (void *) &contHilo) < 0)
            errorFatal("Error: No sve pudo crear el hilo");
        //dormimos un poco hasta que se cree el proximo
        sleep(getRandom(LIMINFLINEAS, LIMSUPLINEAS));
        contHilo++;
    }
    printf("Salio del while por que la bandera cambio\n");
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
    int x = getRandom(LIMINFLINEAS, LIMSUPLINEAS);
    printf("Estoy vivo! soy el hilo %d y voy a dormir %d segundos antes de morir\n", idHilo, x);
    sleep(x);
    printf("Estoy despierto! soy el hilo %d y me llego la hora... adios mundo cruel :(\n", idHilo);
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