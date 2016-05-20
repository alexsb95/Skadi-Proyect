#include <sys/stat.h>
#include <fcntl.h>

/*Prototipos de funciones*/
void crearArchivo (char*);
void modificarArchivo(char*, char* );

/*Rutinas de funciones*/
void crearArchivo (char* pNombre){
    int archivo = open(pNombre, O_TRUNC|O_CREAT,S_IRUSR|S_IWUSR);
    if(archivo >= 0)
    	close(archivo);
}

void modificarArchivo(char* pNombre, char* pDatos){

    int archivo = open(pNombre, O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR);

    if(archivo>=0){
        write(archivo, pDatos, strlen(pDatos));
        close(archivo);
    }else{
        printf("Error: no se pudo arbrir el archivo.\n");           
    }

}
