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
#include "ManejarSemaforo.h"
#include "Algoritmo.h" 

#define LIMINFLINEAS 1
#define LIMSUPLINEAS 10
#define LIMINFTIELIN 10
#define LIMSUPTIELIN 30
#define LIMINFTIEMPO 15
#define LIMSUPTIEMPO 30
#define NOMBREBITACORA "Bitacora.txt"
#define NOMBREBLOQUEADO "Bloqueado.txt"
#define TIEMPOSLEEP 20
#define MEMORIABLOQUEADO 500

/*      Prototipos de funciones         */
void producirHilos (int*);
void *CorrerHilo(void *);
int getRandom(int, int);

int ejecutaralgoritmo(int, int);

int escogerAlgoritmo();
void imprimirMenu();


char* obtenerTiempoActual();
void escribirAccionBitacora(int, char*, char*, int, int);
char* convertirIntAString(int );
struct flock crearSemarofoArchivo(char* pNombre);
void agregarDatos(char* pDatos);

void limpiarMemoria(int, int, int*);
void  recorrerMemoria(int*, int);
int procesoBloqueado(int);
void procesoDesBloqueado(int, int);
void obtieneAccesoMem(int, int*);
void liberaAccesoMem(int*);

/*      Semaforo de la escritura en la memoria      */
int semaforoId;
int semaforoIdBloqueado;
int semaforoIdLectura;
int semaforoIdAcceso;

/*      Datos de la memoria         */
int *tamano;
int *datos;
int *bloqueado;
int *accesoMemoria; 

/*      Tipo algoritmo del algoritmo    */
int tipoAlgoritmo = 0;

/*      Archivo de la bitacora           */
int archivoBitacora;

/*      Semarofo de el archivo      */
struct flock semarofoArchivo;

int main(int argc, char *argv[]){
    if (argc != 1) {
        fprintf(stderr, "Error: Ingresar los parametros. Forma correcta: ./%s\n", argv[0]);
        exit(1);
    }

    /*      Abre la bitacora         */
    archivoBitacora = open(NOMBREBITACORA, O_WRONLY|O_APPEND,S_IRUSR|S_IWUSR);

    /*      Crea el semaforo de la bitacora       */
    semarofoArchivo = crearSemarofoArchivo(NOMBREBITACORA);

    /*      Solicita el algoritmo que se quiere utilizar        */
    tipoAlgoritmo = escogerAlgoritmo();


    /*      Key del semaforo de la memoria      */
    key_t llaveSemarofo = 2032;
    key_t llaveSemarofoLectura = 1395;
    key_t llaveSemarofoBloqueado = 2186;
    key_t llaveSemarofoAcceso = 5068;

    /*      Keys para reservar los segmentos de memoria     */
    key_t llaveDatos = 5432;
    key_t llaveTamano = 6543;
    key_t llaveBandera = 7654;
    key_t llaveBloqueado = 6667;
    key_t llaveAccesoMem = 4565;
    
    int shmIdDatos;
    int shmIdTamano;
    int shmIdBandera;
    int shmIdBloqueado;
    int shmIdAccesoMem;

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

    /*      Solicita la memoria de los bloqueados       */
    shmIdBloqueado = reservarMemoria(llaveBloqueado, MEMORIABLOQUEADO);
    bloqueado = vincularMemoria(shmIdBloqueado);

    /*      Solicita la memoria de el proceso que acede a memoria       */
    shmIdAccesoMem = reservarMemoria(llaveAccesoMem, 1);
    accesoMemoria = vincularMemoria(shmIdAccesoMem);

    /*      Inicializamos los semaforo       */
    semaforoId = crearSemaforoMemoria(llaveSemarofo);
    semaforoIdBloqueado = crearSemaforoMemoria(llaveSemarofoBloqueado);
    semaforoIdLectura = crearSemaforoMemoria(llaveSemarofoLectura);
    semaforoIdAcceso = crearSemaforoMemoria(llaveSemarofoAcceso);

    /*      Crea los hilos      */
    producirHilos(bandera);

    /*      destruimos el semaforo       */
    destruirSemaforoMemoria(semaforoId);
    destruirSemaforoMemoria(llaveSemarofoBloqueado);
    destruirSemaforoMemoria(semaforoIdLectura);
    destruirSemaforoMemoria(semaforoIdAcceso);

    close(archivoBitacora);

    return 0;
}

void producirHilos (int* pBandera){
    int contHilo = 1;

    pthread_t hiloId;

    while((int)*pBandera == 1){
        //creamos el hilo
        if(pthread_create(&hiloId, NULL,  CorrerHilo, (void *) &contHilo) < 0)
            errorFatal("Error: No se pudo crear el hilo");
        //dormimos un poco hasta que se cree el proximo
        sleep(getRandom(LIMINFLINEAS, LIMSUPTIEMPO));
        contHilo++;
    }
}

/*      Maneja memoria      */

void  recorrerMemoria(int* pDatos, int pTamano){
    int contador;

    for(contador=0; contador<pTamano;contador++){
        printf("linea %i # %d #\n",contador,*pDatos );
        pDatos ++;
    }

}

void limpiarMemoria(int pPosicionInicial, int pCantLineas, int *pPunteroMemoria){
    int contador;

    pPunteroMemoria=pPunteroMemoria+pPosicionInicial;

    for(contador=0; contador<=pCantLineas;contador++){
        if(contador !=0 || pPosicionInicial != 0)
            pPunteroMemoria++;
        *pPunteroMemoria = 0;

    }
}

void *CorrerHilo(void *pIdHilo){
    int idHilo = *(int*)pIdHilo;
    int posicion = 0;
    int posicionBloqueado;
    int tiempoEspera = getRandom(LIMINFTIELIN, LIMSUPTIELIN);
    int cantidadLineas = getRandom(LIMINFLINEAS, LIMSUPLINEAS);
    union semun arg;

    printf(" Se creo el proceso %i ...\n",idHilo);

    /*		Marca el proceso como bloqueado		*/
    posicionBloqueado = procesoBloqueado(idHilo);

    /*      Solicita el semaforo de memoria         */
    obtenerSemaforoMemoria (semaforoId);

	printf("El proceso %i bloqueo la memoria\n", idHilo);

	procesoDesBloqueado(idHilo, posicionBloqueado);

    /*		Espera si esta en modo lectura 		*/
    while(semctl(semaforoIdLectura, 0, GETVAL, arg) == 0){
    	printf(" Esperando lectura\n");
    	sleep(1);
    }

    /*  Marca el proceso que tiene acceso a la memoria      */
    obtieneAccesoMem(idHilo, accesoMemoria);

    /*      Asigana el proceso a memoria        */
	posicion = ejecutaralgoritmo(idHilo,cantidadLineas);

    /*      Verifica si encontro memoria       */
	if (posicion != -1){

        /*      Registra la accion en memoria       */
		escribirAccionBitacora(idHilo, "Asigna Memoria", obtenerTiempoActual(), posicion, cantidadLineas);

		printf(" posicion %i - cantidadLineas %i\n",posicion, cantidadLineas );

	}else
		printf("El proceso %i no encontro espacio en la memoria\n",idHilo);

	recorrerMemoria(datos,(int)*tamano);

    sleep(TIEMPOSLEEP);

    liberaAccesoMem(accesoMemoria);

	printf("El proceso %i des-bloqueo la memoria\n", idHilo);

    liberarSemaforoMemoria(semaforoId);

    /*       Verifica que si encontro espacio        */
    if (posicion != -1){
        printf("El proceso %i entro en espera\n", idHilo);
        sleep(tiempoEspera);

        printf(" Sacando proceso %i ...\n", idHilo); 

        obtenerSemaforoMemoria (semaforoId);
        printf("El proceso %i bloqueo la memoria\n", idHilo);   

        /*      Espera si esta en modo lectura      */
        while(semctl(semaforoIdLectura, 0, GETVAL, arg) == 0){
            printf(" Esperando lectura\n");
            sleep(1);
        }

        obtieneAccesoMem(idHilo, accesoMemoria);

        /*      Libera la memria que ocupaba el proceso         */
        limpiarMemoria(posicion, cantidadLineas, datos);

        escribirAccionBitacora(idHilo, "Desasigna Memoria",obtenerTiempoActual(),posicion,cantidadLineas);

        liberaAccesoMem(accesoMemoria);

        printf("El proceso %i des-bloqueo la memoria\n", idHilo);

        liberarSemaforoMemoria(semaforoId);
    }

    pthread_exit(NULL);  
}

/*      Manejo de algoritmos        */

int getRandom(int pLimInf, int pLimSup){
	int r = (rand() % (pLimSup+1-pLimInf))+pLimInf;
	return r;
}

int ejecutaralgoritmo(int pIdProceso, int pTamanoProceso ){
    switch (tipoAlgoritmo){
        case 1 : return FirstFit(pIdProceso, pTamanoProceso, datos, *tamano);
        case 2 : return BestFit(pIdProceso, pTamanoProceso, datos, *tamano);
        case 3 : return WorstFit(pIdProceso, pTamanoProceso, datos, *tamano);
    }
}

int escogerAlgoritmo(){
	char str[10];
    int opcion = 0;
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

/*      Manejo de bitacora      */

void escribirAccionBitacora(int pProcesoId, char* pAccion, char* pHora, int pLineasAsigandas, int pCantidadAsignadas){
    char *buffId = convertirIntAString(pProcesoId);
    char **buffLinea = (char **) malloc(sizeof(char*)*(pCantidadAsignadas+1));
    int contador;

    /*      Calcula las lineas ocupadas por el hilo         */
    for(contador=0; contador<pCantidadAsignadas; contador++){
            buffLinea[contador] = convertirIntAString(pLineasAsigandas+contador);
    }

    /*      Crea el string de datos      */
    char * datosStr = (char *) malloc(1 +sizeof(char*) * (strlen(pAccion)+ strlen(pHora)) + sizeof(int) * 2);
    strcpy(datosStr, "Identificador: ");
    strcat(datosStr, buffId); 
    strcat(datosStr, "\nAccion: ");
    strcat(datosStr, pAccion);
    strcat(datosStr, "\nHora: ");
    strcat(datosStr, pHora);
    strcat(datosStr, "Lineas afectadas: ");
    for (contador = 0; contador < pCantidadAsignadas; contador++)
    {
        strcat(datosStr, buffLinea[contador]);
        strcat(datosStr, " - ");
    }
    strcat(datosStr, "\n");

    free(buffId);
     printf(" \nBitacora: \n%s \n",datosStr);
    for ( contador = 0; contador < pCantidadAsignadas; contador++ )
    	free( buffLinea[contador] );
 
    free(buffLinea);
    

    agregarDatos(datosStr);
}

struct flock crearSemarofoArchivo(char* pNombre){
                                 /* l_type   l_whence  l_start  l_len  l_pid   */
    struct flock semarofoArchivo = {F_WRLCK, SEEK_SET,   0,      0,     0 };
    
    /*      Verifica que si el arhivo esta vacio el valor se cambia a 0     */
    int tamanoArchivo = obtenerTamanoArchivo(pNombre)-1;

    if(tamanoArchivo<0)
        tamanoArchivo=0;

    /*      Se modifica los valores del objeto      */
    semarofoArchivo.l_pid = getpid();
    semarofoArchivo.l_len = tamanoArchivo;

    return semarofoArchivo;
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

void agregarDatos(char* pDatos){

    /*      Se abre el archivo      */
    if (archivoBitacora == -1) {
        printf("Error: no se pudo abrir el archivo.\n");
        return;
    }

    /*      Se solicita el semaforo        */
    if (fcntl(archivoBitacora, F_SETLKW, &semarofoArchivo) == -1) {
        printf("Error: no se pudo obtener el semaforo del archivo.\n");
        return;
    }

    write(archivoBitacora, pDatos, strlen(pDatos));

    /*      se libera el semaforo       */
    semarofoArchivo.l_type = F_UNLCK;  

    if (fcntl(archivoBitacora, F_SETLK, &semarofoArchivo) == -1) {
        printf("Error: no se pudo liberar el semaforo del archivo.\n");
        return;
    }
}

int procesoBloqueado(int pIdHilo){
    int posicion = 0;

    obtenerSemaforoMemoria (semaforoIdBloqueado);

    posicion = FirstFit(pIdHilo, 1, bloqueado, MEMORIABLOQUEADO);

    liberarSemaforoMemoria(semaforoIdBloqueado);

    return posicion;
}

void procesoDesBloqueado(int pIdHilo, int pPosicion){

    obtenerSemaforoMemoria (semaforoIdBloqueado);

    limpiarMemoria(pPosicion, 0, bloqueado);

    liberarSemaforoMemoria(semaforoIdBloqueado);
}

void obtieneAccesoMem(int pIdHilo, int *pMemoria){

    obtenerSemaforoMemoria (semaforoIdAcceso);

    *pMemoria = pIdHilo;

    liberarSemaforoMemoria(semaforoIdAcceso);

}

void liberaAccesoMem(int *pMemoria){

    obtenerSemaforoMemoria (semaforoIdAcceso);

    *pMemoria = 0;

    liberarSemaforoMemoria(semaforoIdAcceso);
}