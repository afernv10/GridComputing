#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define TAM 10000	// Matriz de 10000x10000
#define N_MAX 100	// Valores entre 0-100

float calcMax(float *vMatrix, int countElems);
void normalizar(float *v, int countElems, float max);
void printAsMatrix(float *vToPrint, int nFilas, int rank);

int main(int argc, char* argv[]){

	//Variables de mi programa
	int size, rank, i, j, nFilasData, nElemsData;
	MPI_Status info;
	float *matrix;
	float maxParcial, maxTotal;
	double t1, t2;
	srand(time(NULL));
	
	// Inicializo MPI
	MPI_Init(&argc, &argv);
	// Calculo cuántos procesos hay
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	// Calculo qué proceso soy
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	// Calculo cuántas filas le tocan de matriz a cada proceso (TAM/size)
	nFilasData = TAM/size;
	// Calculo número de elementos a cada proceso
	nElemsData = nFilasData*TAM;
	if (rank==0) printf("Ejecución del programa con %d procesos.\nMatriz %dx%d.\tValores del 0 al %d.\n\n", size, TAM, TAM, N_MAX);

	int h = 0;
	// Inicialización
	// La matriz la pongo en forma de vector para que los punteros tengan posiciones contiguas
	if(rank==0){// si soy el proceso maestro
		matrix =(float*)malloc(TAM*TAM*sizeof(float));
		if(matrix==NULL){
			perror("Malloc error");
			exit(1);
		}
		for (i = 0; i < TAM*TAM; i++){
			matrix[i] = rand() % (N_MAX+1);	// Relleno
		}
	
	} else {
		matrix =(float*)malloc(nElemsData*sizeof(float));
		for (i = 0; i < nElemsData; i++){
			matrix[i] = 0;
		}
	}
	
	//printAsMatrix(&matrix[0], nFilasData, rank);

	// Sincronizo procesos
	MPI_Barrier(MPI_COMM_WORLD);
	t1 = MPI_Wtime();

	// Reparto la matriz del maestro entre todos los procesos
	MPI_Scatter(&matrix[0], nElemsData, MPI_FLOAT, &matrix[0], nElemsData, MPI_FLOAT, 0, MPI_COMM_WORLD);

	// Máximos parciales en cada proceso
	maxParcial = calcMax(&matrix[0], nElemsData);
	printf("Proceso %d: mi máximo parcial es %f.\n", rank, maxParcial);
	
	// Calculamos el máximo global y lo saben todos, en maxTotal
	MPI_Allreduce(&maxParcial, &maxTotal, 1, MPI_FLOAT, MPI_MAX, MPI_COMM_WORLD);
	printf("Proceso %d: el máximo global es %f.\n", rank, maxTotal);

	// Normalizo los valores entre 0 y 1
	normalizar(&matrix[0], nElemsData, maxTotal);
	
	// Todos los procesos envían sus valores actualizados al maestro  
	MPI_Gather(&matrix[0], nElemsData, MPI_FLOAT, &matrix[0], nElemsData, MPI_FLOAT, 0, MPI_COMM_WORLD);
	
	// Sincronizo procesos
	MPI_Barrier(MPI_COMM_WORLD);
	t2 = MPI_Wtime();

	//printAsMatrix(&matrix[0], nFilasData, rank);

	if(rank==0){
		printf("\nTiempo empleado [%d procesos]: %f segundos\n", size,(t2-t1));
	}
	free(matrix);
	MPI_Finalize();
	return 0;
}

float calcMax(float *vMatrix, int countElems){
	int i;
	float max = 0;
	for (i = 0; i < countElems; i++){
		if (vMatrix[i] > max){
			max = vMatrix[i];
		}
	}
	return max;
}

void normalizar(float *v, int countElems, float max){
	int i;

	for (i = 0; i < countElems; i++){
		v[i] = v[i]/max;
	}
}

void printAsMatrix(float *vToPrint, int nFilas, int rank){
	int i, j, h = 0;
	if(rank==0){
		for (i = 0; i < TAM; i++){
		
			for (j = 0; j < TAM; j++){
				printf("%f\t", vToPrint[h]);	// Relleno
				h++;
			}
			printf("\n");
		}
		printf("\n");
	} else {
		for (i = 0; i < nFilas; i++){
		
			for (j = 0; j < TAM; j++){
				printf("%f\t", vToPrint[h]);	// Relleno
				h++;
			}
			printf("\n");
		}
		printf("\n");
	}
}