#include <omp.h>
#include <stdio.h>
#define CHUNKSIZE 2
#define N 10

int main (){
	int i, chunk,nthreads, tid;
	int a[N],b[N],c[N];
	
	for (i=0;i<N;i++)
		a[i]=b[i]=i*1.0;
	chunk=CHUNKSIZE;
	//Las variables a, b, c, chunk son de tipo compartido
	//Las variables i, tid son privadas a cada thread
	#pragma omp parallel shared(a, b, c, chunk) private(i, tid)
	{

		//Asignación estática de iteraciones en la que el
		//tamaño del bloque se fija a chunk
		#pragma omp for nowait
		for (i=0;i<N;i++){
			//Get ID of thread
			tid = omp_get_thread_num();
			//Get number of threads
			nthreads = omp_get_num_threads();
			c[i]=a[i]+b[i];
			printf("El thread %d, de %d threads, calcula la iteración i = %d\n", tid, nthreads, i);
		}

		printf("Fin del hilo %d\n", tid);	// para ver la diferencia entre nowait y sin
	} //Fin de la región paralela
	printf("out pragma\n");
}