/*No se complique, viva feliz: cc productor.c -o productor -lpthread*/
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/sem.h>

#include "ManejarMemoria.h"
#include "ManejarArchivo.h"

#define LIMINFLINEAS 1
#define LIMSUPLINEAS 10
#define LIMINFTIELIN 20
#define LIMSUPTIELIN 60
#define LIMINFTIEMPO 30
#define LIMSUPTIEMPO 60

/*Prototipos de funciones*/
void producirHilos (int*);
void *CorrerHilo(void *);
int getRandom(int, int);
int escogerAlgoritmo();
void imprimirMenu();

void iniSemarofoMemoria();
void finiSemarofoMemoria();
char* obtenerTiempoActual();
void escribirAccionBitacora(int , char* , char* , int );
char* convertirIntAString(int );
struct flock crearSemarofoArchivo(char* pNombre);
void agregarDatos(char* pNombre, char* pDatos);

pthread_mutex_t semAccesoMemoria;

int main(int argc, char *argv[]){
    if (argc != 1) {
        fprintf(stderr, "Error: Ingresar los parametros. Forma correcta: ./%s\n", argv[0]);
        exit(1);
    }
    iniSemarofoMemoria();

    key_t llaveDatos = 5432;
    key_t llaveTamano = 6543;
    key_t llaveBandera = 7654;
    
    int shmIdDatos;
    int shmIdTamano;
    int shmIdBandera;

    int *datos;
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

    /*      Crea los hilos      */
   producirHilos(bandera);

    finiSemarofoMemoria();

    escribirAccionBitacora(50,"char","char",4);

    return 0;
}

void producirHilos (int* pBandera){
    int contHilo = 0;

    pthread_t hiloId;

    while((int)*pBandera == 1){
        //creamos el hilo
        if(pthread_create(&hiloId, NULL,  CorrerHilo, (void *) &contHilo) < 0)
            errorFatal("Error: No sve pudo crear el hilo");
        //dormimos un poco hasta que se cree el proximo
        sleep(getRandom(LIMINFLINEAS, LIMSUPLINEAS));
        contHilo++;
    }

    printf("Salio del while por que la bandera cambio\n");

}

void *CorrerHilo(void *pIdHilo){
    int idHilo = *(int*)pIdHilo;
    int encontroEspacio = 0;
    int tiempoEspera = getRandom(LIMINFLINEAS, LIMSUPLINEAS);

    pthread_mutex_lock (&semAccesoMemoria);
        printf("El proceso %i bloqueo la memoria\n", idHilo);   

        /*   Usa el algoritmo para buscar */

        printf("El proceso %i des-bloqueo la memoria\n", idHilo);
    pthread_mutex_unlock (&semAccesoMemoria);

    if (encontroEspacio == 1){
        sleep(tiempoEspera);

        pthread_mutex_lock (&semAccesoMemoria);
            printf("El proceso %i bloqueo la memoria\n", idHilo);   

            /*   Desocupa la memoria */

            printf("El proceso %i des-bloqueo la memoria\n", idHilo);
        pthread_mutex_unlock (&semAccesoMemoria);
    }else
        printf("El proceso %i no encontro espacio en la memoria\n",idHilo);

    pthread_exit(NULL);  
}

int getRandom(int pLimInf, int pLimSup){
	int r = (rand() % (pLimSup+1-pLimInf))+pLimInf;
	return r;
}

int escogerAlgoritmo(){
	int opcion = 0;
	char str[10];
	while(opcion < 1 || opcion > 3){
		imprimirMenu();
		scanf("%s", str);
		opcion = atoi(str);
	}
	return opcion;
}

void imprimirMenu(){
	printf("---------------------------\n");
	printf("Por favor ingrese uno de los\nsiguientes numeros para escoger\nel algoritmo:\n");
	printf("1. First Fit\n");
	printf("2. Best Fit\n");
	printf("3. Worst Fit\n");
	printf("---------------------------\nOpcion: ");
}

void iniSemarofoMemoria (){
    pthread_mutex_init(&semAccesoMemoria, NULL);
}

void finiSemarofoMemoria(){
    pthread_mutex_destroy(&semAccesoMemoria);
}

void escribirAccionBitacora(int pProcesoId, char* pAccion, char* pHora, int pLineasAsigandas){
    char *buffId = convertirIntAString(pProcesoId);
    char *buffLinea = convertirIntAString(pLineasAsigandas);

    /*      Crea l string de datos      */
    char * datos = (char *) malloc(1 +sizeof(char*) * (strlen(pAccion)+ strlen(pHora)) + sizeof(int) * 2);
    strcpy(datos, "Identificador: ");
    strcat(datos, buffId); 
    strcat(datos, "\nAccion: ");
    strcat(datos, pAccion);
    strcat(datos, "\nHora: ");
    strcat(datos, pHora);
    strcat(datos, "\nLineas asigandas: ");
    strcat(datos, buffLinea);
    strcat(datos, "\n");

    free(buffId);
    free(buffLinea);

    agregarDatos("Bitacora.txt", datos);
}

char* convertirIntAString(int pInt){
    char *string = malloc(sizeof(char));

    sprintf(string, "%d", pInt);

    return string;
}

char* obtenerTiempoActual(){
    time_t rawtime;
    struct tm *timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    return  asctime(timeinfo) ;
}

struct flock crearSemarofoArchivo(char* pNombre){
                                 /* l_type   l_whence  l_start  l_len  l_pid   */
    struct flock semarofoArchivo = {F_WRLCK, SEEK_SET,   0,      0,     0 };
    
    /*      Verifica que si el arhivo esta vacio el valor se cambia a 0*/
    int tamano = obtenerTamanoArchivo(pNombre)-1;
    if(tamano<0)
        tamano=0;

    /*      Se modifica los valores del objeto      */
    semarofoArchivo.l_pid = getpid();
    semarofoArchivo.l_len = tamano;

    return semarofoArchivo;
}


void agregarDatos(char* pNombre, char* pDatos){

    int archivo = open(pNombre, O_WRONLY|O_APPEND,S_IRUSR|S_IWUSR);

    struct flock semarofoArchivo = crearSemarofoArchivo(pNombre);

    /*      Se abre el archivo      */
    if (archivo == -1) {
        printf("Error: no se pudo abrir el archivo.\n");
        return;
    }

    /*      Se solicita el semaforo        */
    if (fcntl(archivo, F_SETLKW, &semarofoArchivo) == -1) {
        printf("Error: no se pudo obtener el semaforo del archivo.\n");
        return;
    }

    write(archivo, pDatos, strlen(pDatos));

    /*      se libera el semaforo       */
    semarofoArchivo.l_type = F_UNLCK;  

    if (fcntl(archivo, F_SETLK, &semarofoArchivo) == -1) {
        printf("Error: no se pudo liberar el semaforo del archivo.\n");
        return;
    }

    close(archivo);
}




