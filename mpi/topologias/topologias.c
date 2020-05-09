#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <math.h>	// NOTA: compilar con -lm: mpicc topologias.c -lm -o exec


#define MAX_VALUE 9			// Valores entre 0-9
#define SIZE_MATRIX 12	

/*
*	Calcula la suma de los elementos de matriz A de tamaño nxn, enteros rand 0-9.
*		- Creamos una topología de procesadores de tamaño pxp, siendo n divisible por p.
*		- El proceso P0 inicializa y reparte la matriz A por bloques bidimensionales 
*		entre los procesos. Estos efectúan la suma de los elementos del correspondiente
*		bloque y devuelven el resultado a P0.
*		- Para enviar submatrices usar tipos derivados MPI_Type_vector
*		- Ejecuta el programa con 4, 9 y 16 procesos, valores mult de 12 (120, 1200...)
*/

int calcSumaParcial(int *cuadrante, int elems);
void printA(int A[SIZE_MATRIX][SIZE_MATRIX], int filsCols);
void printVCuadrante(int *cuadrante, int elems);

int main(int argc, char* argv[]){

	int nprocesos, rank, blockSize, intSqrt, A[SIZE_MATRIX][SIZE_MATRIX], *Avector, sumaParcial = 0, sumaGlobal = 0, i, j, proceso;
	int dims[2], periods[2], reorder = 0, coords[2];
	float floatSqrt;
	MPI_Status status;
	MPI_Datatype bloque;
	MPI_Comm cart;
	srand(time(NULL));

	// Inicializo MPI
	MPI_Init(&argc, &argv);
	// Calculo cuántos procesos hay
	MPI_Comm_size(MPI_COMM_WORLD,&nprocesos);
	// Calculo qué proceso soy
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	// Miramos si el número de procesos es válido, raiz perfecta
	floatSqrt = sqrt(nprocesos);
	intSqrt = floatSqrt;	// a float, cast

	if(floatSqrt!=intSqrt){
		if(rank==0){
			printf("ERROR. Número de procesos incorrecto. Introduzca 4, 9 o 16.\n");
		}
		MPI_Finalize();
		return 0;
	}

	blockSize = SIZE_MATRIX/intSqrt;	// Tamaño de fila/col del bloque

	// Creamos topología
	dims[0] = dims[1] = intSqrt;	// número de procesos en cada dim
	periods[0] = periods[1] = 0; 	// ciclica NO = 0, aquí nos da igual
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &cart);


	if(rank==0){
		printf("Número de procesos: %d\nMatriz: %dx%d.\nTamaño del bloque: %dx%d.\n\n", nprocesos, SIZE_MATRIX, SIZE_MATRIX, blockSize, blockSize);
	}

	// Inicialización
	if(rank==0){
		// Master inicializa matriz A, valores rand 0-9
		for (i = 0; i < SIZE_MATRIX; i++){
			for (j = 0; j < SIZE_MATRIX; j++){
				A[i][j] = rand() % (MAX_VALUE+1);
			}
		}

		//printA(A, SIZE_MATRIX);
	}

	// Todos los procesos inicializan su vector bloque
	// Reservo memoria
	Avector = (int*)malloc(blockSize*blockSize*sizeof(int));

	// 1 - count: blockSize --> n bloques = filas que tiene el bloque
	// 2 - blocklen: blockSize --> unidades en cada bloque = n cols 
	// 3 - stride: SIZE_MATRIX --> desplazamiento hasta siguiente fila/bloque desde pos 1 del bloque
	MPI_Type_vector(blockSize, blockSize, SIZE_MATRIX, MPI_INT, &bloque);
	MPI_Type_commit(&bloque);

	// Sincronizo procesos
	MPI_Barrier(MPI_COMM_WORLD);

	if(rank==0){
		
		// Bucle sobre cada uno de los bloques de la fila
		// intSqrt: n cuadrantes en fila o col = n procesos
		for (i = 0; i < intSqrt; i++){
			for(j = 0; j < intSqrt; j++){
				coords[0] = i;
				coords[1] = j;

				MPI_Cart_rank(cart, coords, &proceso);
				printf("Enviando bloque a proceso %d...\n", proceso);
				MPI_Send(&(A[i*blockSize][j*blockSize]), 1, bloque, proceso, 0, MPI_COMM_WORLD);
				printf("Bloque enviado a proceso %d...\n", proceso);
			}
		}
		
		MPI_Type_free(&bloque);
	}

	// Todos los procesos reciben un bloque y hacen su suma
	printf("Esperando a bloque en proceso %d...\n", rank);
	MPI_Recv(&Avector[0], blockSize*blockSize, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
	printf("Bloque recibido en proceso %d.\n", rank);
	// imprimir cuadrantes, solo comprobar individualmente, se mezcla
	/*
	if(rank==0){
		printVCuadrante(Avector, blockSize*blockSize);
	}*/

	// Suma
	sumaParcial = calcSumaParcial(Avector, blockSize*blockSize);
	printf("Proceso %d: suma = %d\n", rank, sumaParcial);

	// Reduce de todas las sumas parciales de cada uno a a sumaGlobal en master
	MPI_Reduce(&sumaParcial, &sumaGlobal, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if(rank==0){
		printf("\nEl resultado de la suma de la matriz es: %d.\n", sumaGlobal);
	}

	free(Avector);
	MPI_Finalize();
	return 0;
}

int calcSumaParcial(int *cuadrante, int elems){
	int i, suma = 0;

	for(i = 0; i < elems; i++){
		suma+= cuadrante[i];
	}
	return suma;
}

void printA(int A[SIZE_MATRIX][SIZE_MATRIX], int filsCols){
	int i, j;

	for(i = 0; i < filsCols; i++){
		for(j = 0; j < filsCols; j++){
			printf("%d\t", A[i][j]);
		}
		printf("\n");
	}
	
}

void printVCuadrante(int *cuadrante, int elems){
	int i;

	for(i = 0; i < elems; i++){
		printf("%d\t", cuadrante[i]);
	}
	printf("\n");
}