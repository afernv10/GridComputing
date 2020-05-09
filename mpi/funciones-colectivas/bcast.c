#include <stdio.h>
#include <mpi.h>

#define N 10

int main(int argc, char* argv[]){

	int size, rank, i, from, to, ndat, part, tag, VA[N];
	MPI_Status info;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	
	//Inicializo vector
	for (i=0; i<N; i++) {
		VA[i] = 0;
	}
	
	//Añadir el código para:
	//  - Si soy maestro, inicializo el vector almacenando en cada elemento del vector, su índice i
	//	- Tanto si soy maestro como si soy esclavo, imprimir el vector VA
	//  - Enviar el VA a TODOS los procesos usando Broadcast (los esclavos lo recibirán)
	//  - Tanto si soy maestro como si soy esclavo, imprimir el vector VA
	if (rank == 0){	// soy el maestro
		for (i=0; i<N; i++) {
			VA[i] = i;
		}
		printf("Proceso %d (MAESTRO): VA con datos antes de enviar:\n", rank);
		for (i = 0; i < N; i++){
			printf("%4d", VA[i]);
		}
		printf("\n\n");

		// Hacemos broadcast, enviamos a esclavos
		MPI_Bcast(&VA[0], N, MPI_INT, rank, MPI_COMM_WORLD);

		printf("Proceso %d (MAESTRO): VA con datos después de enviar:\n", rank);
		for (i = 0; i < N; i++){
			printf("%4d", VA[i]);
		}
		printf("\n\n");
	}

	// si soy esclavo...
	else {
		printf("Proceso %d: VA antes de recibir:\n", rank);
		for (i = 0; i < N; i++){
			printf("%4d", VA[i]);
		}
		printf("\n\n");

		// Ponemos 0 porque recibira del maestro, el unico que envia
		MPI_Bcast(&VA[0], N, MPI_INT, 0, MPI_COMM_WORLD);

		printf("Proceso %d recibe VA con datos:\n", rank);
		for (i = 0; i < N; i++){
			printf("%4d", VA[i]);
		}
		printf("\n\n");
	}
	
	MPI_Finalize();
	return 0;
}