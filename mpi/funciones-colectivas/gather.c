#include <stdio.h>
#include <mpi.h>

#define N 10

int main(int argc, char* argv[]){

	//Variables de mi programa
	int size, rank, i, from, to, ndat, part, tag, VA[N], dataSize;
	MPI_Status info;
	
	//Inicializo MPI
	MPI_Init(&argc, &argv);
	//Calculo cuántos procesos hay
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	//Calculo qué proceso soy
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	//Inicializo vector a 0
	for (i=0; i<N; i++) {
		VA[i] = 0;
	}
	//Si soy maestro, inicializo los elementos del vector para que guarden su posición (i)
	if (rank == 0){
		for (i=1; i<N; i++) {
			VA[i] = i;
		}
	}
	printf("Proceso %d: VA antes de recibir datos: \n",rank);
	//Para todos: imprimo el vector
	for (i = 0; i < N; i++){
		printf("%4d", VA[i]);
	}
	printf("\n\n");
	//Calculamos cuántos datos corresponden a cada proceso
	dataSize = N/size;
	//Dividimos el vector en partes, y las distribuimos entre los procesos
	// 0 es el que envía
	MPI_Scatter(&VA[0], dataSize, MPI_INT, &VA[0], dataSize, MPI_INT, 0, MPI_COMM_WORLD);
	//Cada proceso multiplica por 2 su parte del vector
	for (i=0; i<dataSize; i++) {
		VA[i] = VA[i]*2;
	}
	printf("Proceso %d: VA después de recibir los datos y multiplicarlos por 2: \n", rank);
	//Cada proceso imprime su parte del vector
	for (i = 0; i < dataSize; i++){
		printf("%4d", VA[i]);
	}
	printf("\n\n");
	//Uso gather para recolectar los valores nuevos en un nuevo vector
	// Llevamos los nuevos valores de todos los esclavos al maestro (0)
	MPI_Gather(&VA[0], dataSize, MPI_INT, &VA[0], dataSize, MPI_INT, 0, MPI_COMM_WORLD);
	//El maestro imprime el vector final
	if (rank == 0){
		printf("Proceso %d: VA después de recibir los datos: \n", rank);
		for (i=0; i<N; i++) {
			printf("%4d", VA[i]);
		}	
	}
	printf("\n\n");		
	//Cierro MPI
	MPI_Finalize();
	return 0;
}