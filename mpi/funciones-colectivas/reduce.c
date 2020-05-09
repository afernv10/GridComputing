#include <stdio.h>
#include <mpi.h>

#define N 10

int main(int argc, char* argv[]){

	//Variables de mi programa
	int size, rank, i, from, to, ndat, part, tag, VA[N], dataSize, sumaParcial = 0, resultado = 0;
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
	// Sincronizamos aquí los procesos
	MPI_Barrier(MPI_COMM_WORLD);
	//Cada proceso multiplica por 2 su parte del vector y suma los datos que tiene
	for (i=0; i<dataSize; i++) {
		VA[i] = VA[i]*2;
		sumaParcial += VA[i];
	}
	printf("Proceso %d: VA después de recibir los datos y multiplicarlos por 2: \n", rank);
	//Cada proceso imprime su parte del vector
	for (i = 0; i < dataSize; i++){
		printf("%4d", VA[i]);
	}
	printf("\n\n");
	printf("Proceso %d: Suma Parcial: %d\n", rank, sumaParcial);
	//Uso reduce para recolectar los valores de los procesos y operar
	// Realizamos la suma de los valores parciales en resultado y lo llevamos al maestro
	// 1 porque solo es un dato a enviar
	MPI_Reduce(&sumaParcial, &resultado, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	//El maestro imprime el resultado
	if (rank == 0){
		printf("Proceso %d: Imprime el resultado calculado:\n", rank);
		printf("%d\n", resultado);	
	}
	printf("\n\n");
			
	//Cierro MPI
	MPI_Finalize();
	return 0;
}