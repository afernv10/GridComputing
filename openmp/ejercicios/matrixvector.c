#include <omp.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define N 4
#define M 4

int main(){
	int i,j, nthreads, tid, n, m, sum, a[M],c[N],b[M][N];
	
	srand(time(NULL));
	m=M;
	n=N;
	//Inicialización de b, c, m, n
	for (i=0; i<M; i++) {
		for (j=0; j<N; j++) {
			b[i][j]=rand()%100;
		}
		
	}
	for (i=0; i<N; i++) 
		c[i]=rand()%100;
	//Las variable a,b,c,m,n,nthreads son de tipo compartido
	//Las variables i,j,sum,tid son de tipo privado
	
	#pragma omp parallel for shared(a,b,c,m,n,nthreads) private(i,j,sum,tid)
	for (i=0;i<m;i++){
		//........Get ID of thread
		tid = omp_get_thread_num();
		//........Get number of threads
		nthreads = omp_get_num_threads();
		sum=0;
		for(j=0;j<n;j++)
			sum+=b[i][j]*c[j];
			a[i]=sum;
			printf("El thread %d, de %d threads, calcula la iteración i=%d\n", tid, nthreads,i);
	}
	

	// Para mostrar no tenemos que paralelizar
	for (i=0; i<M; i++) {
		printf("a[%d]=%d\n",i,a[i]);
	}
}