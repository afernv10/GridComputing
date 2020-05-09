#include <omp.h>
#include <stdio.h>
int main(){
int nthreads, tid;
#pragma omp parallel private(tid) //Expande un grupo de threads
{
	//Cada thread contiene sus propias copias de variables
	tid=omp_get_thread_num(); //Obtiene el id del thread
	nthreads=omp_get_num_threads(); //Núm de threads
	printf("Hola desde el thread %d de %d threads\n", tid,
	nthreads);
}
//Todos los threads se unen al thread maestro y finalizan
return 0;
}