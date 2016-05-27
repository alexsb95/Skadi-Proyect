#include <stdio.h>
#include <stdlib.h>


int FirstFit(int, int, int*, int);
int BestFit(int, int, int*, int);
int WorstFit(int, int, int*, int);

/*          Algoritmos          */

int FirstFit(int pIdProceso, int pTamanoProceso, int* ptrDatos, int tamanio){

 int index = 0; int size = 0; int entra = 0;
 int *ini = ptrDatos;
 int *fin = ptrDatos;

    while(index < tamanio && entra == 0){
        if(*ini == 0){
           fin = ini;
           while(*fin == 0){
                size++; fin++; index++;
                if(size >= pTamanoProceso){
                    entra = 1;
                    break;
                }
                else if(index == tamanio){
                    entra = -1;
                    break;
                }
            }
           if(entra == 0){
                ini = fin;
                size = 0;
            }
            else break;
        }else{
            size = 0;
            ini++;
            index++;
        }
    }
    
    if(entra == 1){
        while(ini != fin){
            *ini = pIdProceso;
            ini++;
        }
        return index - pTamanoProceso;
    }
    else
      return -1;
}

int BestFit(int pIdProceso, int pTamanoProceso, int* ptrDatos, int tamanio){
 
    int index = 0; int size = 0;
    int *ini = ptrDatos;
    int *fin = ptrDatos;
    int entra = 0; int *ptrIndice = ini; int diferencia = 10000; int indexReturn = 0; int indexInicio = 0;

    while(index < tamanio){
        if(*ini == 0){
            indexInicio = index;
            fin = ini;
            while(*fin == 0){
                size++; fin++; index++;
                if(index == tamanio)
                    break;
            }
            if((size - pTamanoProceso) >= 0){
                if(diferencia > (size - pTamanoProceso)){

                    ptrIndice = ini;
                    entra = 1;
                    diferencia = (size - pTamanoProceso);
                    indexReturn = indexInicio;
                }
                else if(diferencia == (size - pTamanoProceso)) entra = 1;
            }
            ini = fin;
            size = 0;

        }
        else{
            size = 0;
            ini++;
            index++;
        }
    }
    
    if(entra == 1){
        while(pTamanoProceso > 0){
            *ptrIndice = pIdProceso;
            ptrIndice++;
            pTamanoProceso--;   
        }
        return indexInicio;
    }
    else return -1;
}

int WorstFit(int pIdProceso, int pTamanoProceso, int* ptrDatos, int tamanio){

    int index = 0; int size = 0;
    int *ini = ptrDatos;
    int *fin = ptrDatos;
    int entra = 0; int *ptrIndice = ini; int diferencia = -1; int indexReturn = 0; int indexInicio = 0;

    while(index < tamanio){
        if(*ini == 0){
            indexInicio = index;
            fin = ini;
            while(*fin == 0){
                size++; fin++; index++;
                if(index == tamanio)
                    break;
            }
            if((size - pTamanoProceso) >= 0){
                if(diferencia < (size - pTamanoProceso)){
                    ptrIndice = ini;
                    entra = 1;
                    diferencia = (size - pTamanoProceso);
                    indexReturn = indexInicio;
                }
                else if(diferencia == (size - pTamanoProceso)) entra = 1;
            }
            ini = fin;
            size = 0;

        }
        else{
            size = 0;
            ini++;
            index++;
        }
    }
    
    if(entra == 1){
        while(pTamanoProceso > 0){
            *ptrIndice = pIdProceso;
            ptrIndice++;
            pTamanoProceso--;   
        }
        return indexInicio;
    }
    else return -1;
}