#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>

#include "ManejarMemoria.h"
#include "ManejarSemaforo.h"

void recorrerMemoria(int*, int);

char* convertirIntAString(int);
char* procesosMemoria (int*, int);
char* procesosMemoriaBloqueados (int*, int);
void espiar(int*, int*, int);
int encuentraLista (int[], int, int);


/*      Semaforo de la escritra en la memoria      */
int semaforoId;
int semaforoIdLectura;

int main(int argc, char *argv[]){

    /*      Key del semaforo de la memoria      */
    key_t llaveSemarofo = 2032;
    key_t llaveSemarofoLectura = 1395;

    /*      Keys para solicitar los segmentos de memoria     */
	key_t llaveDatos = 5432;
    key_t llaveTamano = 6543;
    key_t llaveBandera = 7654;
    key_t llaveBloqueado = 6667;
    
    int shmIdDatos;
    int shmIdTamano;
    int shmIdBandera;
    int shmIdBloqueado;

    int *datos;
    int *tamano;
    int *bandera;
    int *bloqueado;  

    /*      Solicita la memoria del tamano       */
    shmIdTamano = reservarMemoria(llaveTamano, 1);
    tamano = vincularMemoria(shmIdTamano);

     /*      Solicita la memoria de la bandera       */
    shmIdBandera = reservarMemoria(llaveBandera, 1);
    bandera = vincularMemoria(shmIdBandera);

    /*      Solicita la memoria de los datos       */
    shmIdDatos = reservarMemoria(llaveDatos, (int)*tamano);
    datos = vincularMemoria(shmIdDatos);

    /*      Solicita la memoria para la los  Procesos Bloqueados       */
    shmIdBloqueado = reservarMemoria(llaveBloqueado, 500);
    bloqueado = vincularMemoria(shmIdBloqueado);

    /*      Imprimimos todo lo que obtuvimos de arriba      */
    imprimirDatoMemoria(shmIdDatos, shmIdBandera, shmIdTamano, (int)*tamano);

    /*      Copiamos el semaforo       */
    semaforoId = copiarSemaforo(llaveSemarofo);
    semaforoIdLectura = copiarSemaforo(llaveSemarofoLectura);

    espiar(datos, bloqueado, (int)*tamano);

}

void  recorrerMemoria(int* pDatos, int pTamano){
	int contador;

    //obtenerSemaforoMemoria (semaforoId);

	for(contador=0; contador<pTamano;contador++){
		printf("linea %i # \t%d\t #\n", contador, *pDatos);
		pDatos ++;
	}

    //liberarSemaforoMemoria(semaforoId);

}

char* procesosMemoria (int* pDatos, int pTamano) {
	int contador;
    int indice;
    int listaProcesos[pTamano];

	char* strProcesos = (char *) malloc((1 +sizeof(char*)) * pTamano);

    strcpy(strProcesos, "Id de los procesos en ejecucion : \n");

    //obtenerSemaforoMemoria (semaforoId);
    indice = 0;

    /*      Limpia la lista         */
    for(contador = 0; contador < pTamano; contador++ ){
        listaProcesos[contador]=0;
    }

    /*      Agrega los procesos a la lista      */
	for(contador = 0; contador < pTamano; contador++){
		if(*pDatos != 0){
            if(encuentraLista(listaProcesos, pTamano, *pDatos) == 0){
                listaProcesos[indice] = *pDatos;
                indice++;
            }
		}
		pDatos++;
	}

    /*      Crea el string de los procesos      */
    for(contador = 0; contador < indice; contador++){
        strcat(strProcesos, "* \t");
        strcat(strProcesos, convertirIntAString(listaProcesos[contador])); 
        strcat(strProcesos, "\t *\n");
    }

    //liberarSemaforoMemoria(semaforoId);

    return strProcesos;
}

char* procesosMemoriaBloqueados (int* pDatos, int pTamano) {
	int contador;
	char* strProcesos = (char *) malloc((1 +sizeof(char*)) * pTamano);

    strcpy(strProcesos, "Id de los procesos en bloqueados : \n");

    //obtenerSemaforoMemoria (semaforoId);

	for(contador=0; contador<pTamano;contador++){
		if(*pDatos != 0){
			strcat(strProcesos, "* \t");
			strcat(strProcesos, convertirIntAString(*pDatos)); 
			strcat(strProcesos, "\t *\n");
		}

		pDatos ++;
	}

    //liberarSemaforoMemoria(semaforoId);

    return strProcesos;
}

char* convertirIntAString(int pInt){
    char *string = malloc(sizeof(char));

    sprintf(string, "%d", pInt);

    return string;
}

void espiar(int* pDatos, int* pBloqueado, int pTamano){
	char * procesosEjecutando;
	char * procesosBloqueados;

	obtenerSemaforoMemoria (semaforoIdLectura);

    procesosEjecutando = procesosMemoria(pDatos,pTamano);
    procesosBloqueados = procesosMemoriaBloqueados(pBloqueado, 500);

   	recorrerMemoria(pDatos, pTamano );
    //recorrerMemoria(pBloqueado, 20 );

    liberarSemaforoMemoria(semaforoIdLectura);

    printf (" %s ", procesosEjecutando);
    printf (" %s ", procesosBloqueados);

    //free (procesosEjecutando);
    //free (procesosBloqueados);

}

int encuentraLista (int pLista[], int pTamano, int pElemento){
    int contador;
    for(contador = 0; contador < pTamano; contador++){
        if(pLista[contador] == pElemento){
            return 1;
        }
    }
    return 0;
}

// GETPID -> ultimo proceso en utlizar el semaforo
// GETZCNT / GETNCNT-> cantidad de procesos bloqueados

/*
       GETNCNT   the value of semncnt.

       GETPID    the value of sempid.

       GETVAL    the value of semval.

       GETZCNT   the value of semzcnt.

*/