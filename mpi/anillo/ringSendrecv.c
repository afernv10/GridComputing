#include <stdio.h>
#include <mpi.h>

/* Escribir un programa MPI que comunique N procesos en forma de anillo.
 * El proceso i debe informarle al proceso i+1 el nombre de la máquina en la que corre.
 * Implementación 3: usando funcion Sendrecv()
 */
int main(int argc, char* argv[]){

	int size, rank = 0, i, len, origen, dest, tag = 0;
	char message_send[100], message_recv[100], name[MPI_MAX_PROCESSOR_NAME];
	MPI_Status info;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD,&size);	// Número de procesos
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);	// Identificador de proceso
	MPI_Get_processor_name(name,&len);		// Nombre de la máquina

	origen = rank-1;
	dest = rank+1;

	sprintf(message_send,"Hola, soy el proceso %d de %d en la máquina %s",rank,size-1,name);
	
	if(rank==0){// El proceso 0 recibe del último
		origen = size-1;
	
	} else if(rank==size-1){// si soy el último proceso envío a 0
		dest = 0;
	} 
	MPI_Sendrecv(&message_send, 100, MPI_CHAR, dest, tag, &message_recv, 100, MPI_CHAR, origen, tag, MPI_COMM_WORLD, &info);
	printf("Proceso %d: Mensaje recibido -> %s\n\n", rank, message_recv);

	MPI_Finalize();
	return 0;
}