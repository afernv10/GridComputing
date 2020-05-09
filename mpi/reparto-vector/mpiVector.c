#include <stdio.h>
#include <stdbool.h>
#include <mpi.h>

#define N 10

int main(int argc, char* argv[]){

	int size, rank, i, origen, destino, ndat, part, tag, VA[N], ndat_reparto = 0, ndat_restantes = 0, nslaves, aux = 0;
	bool hay_restantes;
	MPI_Status info;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	// Comprobamos que exista por lo menos un maestro y un esclavo y todos tengan algun dato
	if(size < 2){
		MPI_Finalize();
		printf("ATENCIÓN. Introduce un número de procesos mayor que 1.\n");
		return 0;
	} else if(size>N){
		MPI_Finalize();
		printf("Introduce un número menor de procesos que de datos a enviar para que todos tengan datos.\n");
		return 0;
	}

	//Inicializo el vector
	for (i=0; i<N; i++) {
		VA[i] = 0;
	}
	
	//Si soy maestro tengo unos valores distintos
	if (rank == 0){
		for (i=0; i<N; i++) {
			VA[i] = i;
		}
		
		//Añadir el código necesario para:
		
		//1. Calcular cuántos datos se envían a cada esclavo (por ejemplo, repartir 100 datos entre 4 esclavos-> 25 a cada uno)
		// Comprobar si N es divisible entre número de procesos
		nslaves = size-1;
		//printf("Número de procesos: %d (esclavos: %d)\n", size, nslaves);
		//printf("Número de datos: %d\n", N);

		if((N % nslaves == 0 ) && (nslaves!=0)){
			// Es divisible, reparto igual
			ndat_reparto = N / nslaves;
			hay_restantes = false;
		} else {
			hay_restantes = true;
			ndat_reparto = N / nslaves;	// Datos para los esclavos que no son el último
			ndat_restantes = (N % nslaves) + ndat_reparto;	// Datos para el último esclavo
		}
		printf("Datos a esclavos %d, ndat_restantes: %d\n\n", ndat_reparto, ndat_restantes);

		//2. Para todos los esclavos...
		//		2.1 Identificar destino y tag
		//      2.2. Distinguir entre los esclavos y el último esclavo
		//		2.3. Si no es el último esclavo, se le envía el número de datos que se calculó antes
		//		2.3. Si es el último esclavo, se le envían los datos restantes 
		if(!hay_restantes){
			for (i = 1; i <= nslaves; i++){
				destino = i; tag = 0;
				//printf("A esclavo - destino: %d, datos reparto: %d \n", destino, ndat_reparto);
				if(i!=1) aux+=ndat_reparto;	// Me ubico en la posicion adecuada de VA
				MPI_Send(&VA[aux], ndat_reparto, MPI_INT, destino, tag, MPI_COMM_WORLD);
			}
		} else {
			// Para los esclavos que no son el último
			for (i = 1; i < nslaves; i++){
				destino = i; tag = 0;
				//printf("A esclavo - destino: %d, datos reparto: %d \n", destino, ndat_reparto);
				if(i!=1) aux+=ndat_reparto;
				MPI_Send(&VA[aux], ndat_reparto, MPI_INT, destino, tag, MPI_COMM_WORLD);
			}

			// Para el último esclavo
			destino = nslaves; tag = 0;	// Tag de último esclavo
			//printf("A último esclavo - destino: %d, datos reparto: %d \n", destino, ndat_restantes);
			if(i!=1) aux+=ndat_reparto;
			MPI_Send(&VA[aux], ndat_restantes, MPI_INT, destino, tag, MPI_COMM_WORLD);
		}
	}
	
	//si soy esclavo...
	else {
		printf("Proceso %d: VA antes de recibir datos: \n",rank);
		
		// Añadir el código necesario para: 
		
		// 1. Imprimir el vector antes de recibir nada
		// 2. Recibir los datos del maestro
		// 3. Calcular cuántos datos se han recibido del maestro
		for (i = 0; i < N; i++){
			printf("%4d", VA[i]);
		}
		printf("\n\n");

		origen = 0; tag = 0;
		MPI_Recv(&VA[0], N, MPI_INT, origen, tag, MPI_COMM_WORLD, &info);
		MPI_Get_count(&info, MPI_INT, &ndat);

		printf("Proceso %d recibe VA de %d: tag %d, ndat %d; \nVA = ", rank, info.MPI_SOURCE, info.MPI_TAG, ndat);
		
		for (i=0; i<ndat; i++) {
			printf("%d  ",VA[i]);
		}
		printf("\n\n");
	}

	MPI_Finalize();
	return 0;
}
