#include <stdio.h>
#include <mpi.h>

/* Escribir un programa MPI que comunique N procesos en forma de anillo.
 * El proceso i debe informarle al proceso i+1 el nombre de la máquina en la que corre.
 * Implementación 2: usando funciones Isend() y Irecv()
 */
int main(int argc, char* argv[]){

	int size, rank = 0, i, len, origen, dest, tag = 0;
	char message[100], name[MPI_MAX_PROCESSOR_NAME];
	MPI_Status info;
	MPI_Request send_request, recv_request;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD,&size);	// Número de procesos
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);	// Identificador de proceso
	MPI_Get_processor_name(name,&len);		// Nombre de la máquina

	origen = rank-1;
	dest = rank+1;

	/**
	* NOTA: El orden de Irecv e Isend es indiferente, lo importante es el wait de recv antes de imprimir
	*/

	// Envio
	sprintf(message,"Hola, soy el proceso %d de %d en la máquina %s",rank,size-1,name);
	// si soy el último proceso envío a 0
	if(rank==size-1) dest = 0;
	MPI_Isend(&message,100,MPI_CHAR,dest,tag,MPI_COMM_WORLD,&send_request);

	// Recibo
	// EL proceso 0 recibe del último
	if(rank==0) origen = size-1;	
	MPI_Irecv(&message, 100, MPI_CHAR, origen, tag, MPI_COMM_WORLD, &recv_request);
	
	// Wait de recv primero para aprovechar la ventaja de no bloqueantes
	MPI_Wait(&recv_request, &info);	// Esperamos a que se haya recibido para imprimir
	printf("Proceso %d: Mensaje recibido -> %s\n\n", rank, message);

	MPI_Wait(&send_request, &info);	// Me aseguro que haya acabado el envío, aprovechando sin bloquear

	MPI_Finalize();
	return 0;
}