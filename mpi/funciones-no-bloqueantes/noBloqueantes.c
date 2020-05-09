#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define SIZE_MASTER 100000	// Vector de 100000 en maestro
#define SIZE_SLAVES 100		// Vector de 100 en esclavos
#define N_MAX 10000			// Valores entre 0-10000

int calcSuma(int *v, int countElems);

int main(int argc, char* argv[]){

	//Variables de mi programa
	int size, rank, i, sumaParcial, sumaTotal, elemsLeft, source, fin, contEnviados;
	int *vector;
	MPI_Status status, status_msg_s;
	MPI_Request send_reqmaster, recv_reqslave, send_reqslave;

	sumaParcial = 0, sumaTotal = 0, source = 0, fin = 0, contEnviados = 0;
	elemsLeft = SIZE_MASTER;

	srand(time(NULL));
	
	// Inicializo MPI
	MPI_Init(&argc, &argv);
	// Calculo cuántos procesos hay
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	// Calculo qué proceso soy
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	// Inicialización
	if(rank==0){// si soy el proceso maestro
		vector =(int*)malloc(SIZE_MASTER*sizeof(int));
		if(vector==NULL){
			perror("Malloc error.");
			exit(1);
		}
		for (i = 0; i < SIZE_MASTER; i++){
			vector[i] = rand() % (N_MAX+1);	// Relleno
		}
	
	} else {
		vector =(int*)malloc(SIZE_SLAVES*sizeof(int));
		if(vector==NULL){
			perror("Malloc error.");
			exit(1);
		}
		for (i = 0; i < SIZE_SLAVES; i++){
			vector[i] = 0;
		}
	}
	
	// Sincronizo procesos
	MPI_Barrier(MPI_COMM_WORLD);

	if(rank==0){// Master
		
		printf("Número de procesos: %d\nSIZE_MASTER: %d\nSIZE_SLAVES: %d\n\n", size, SIZE_MASTER, SIZE_SLAVES);
		
		// de primeras mando a todos los procesos
		// TODO comprobación de que haya procesos<=posibles a repartir
		for (i = 1; i < size; i++){
			MPI_Isend(&vector[(i-1)*SIZE_SLAVES], SIZE_SLAVES, MPI_INT, i, 0, MPI_COMM_WORLD, &send_reqmaster);
			sumaParcial = calcSuma(&vector[(i-1)*SIZE_SLAVES], SIZE_SLAVES);
			printf("MAESTRO - Calcula la suma del v enviado a ESCLAVO %d [v en pos %d]: %d\n", i, (i-1)*SIZE_SLAVES, sumaParcial);
			elemsLeft -= SIZE_SLAVES;
			contEnviados++;
			MPI_Wait(&send_reqmaster, &status);	// por si acaso falla
		}
		
		// mientras haya mensajes por recibir o haya elementos
		while(!fin){
			// Bloqueamos hasta recibir mensajes de las características: tag=0
			MPI_Probe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			source = status.MPI_SOURCE;

			MPI_Recv(&sumaParcial, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
			sumaTotal = sumaTotal+sumaParcial;
			printf("MAESTRO - Recibe de ESCLAVO %d su suma parcial: %d\n", source, sumaParcial);
			
			if(elemsLeft>0){
				
				MPI_Isend(&vector[SIZE_MASTER - elemsLeft], SIZE_SLAVES, MPI_INT, source, 0, MPI_COMM_WORLD, &send_reqmaster);
				sumaParcial = calcSuma(&vector[SIZE_MASTER - elemsLeft], SIZE_SLAVES);
				printf("MAESTRO - Calcula la suma del v enviado a ESCLAVO %d [v en pos %d]: %d\n", source, SIZE_MASTER - elemsLeft, sumaParcial);
				elemsLeft -= SIZE_SLAVES;
				contEnviados++;

			} else {
				// Se han enviado todos los elementos
				MPI_Isend(&vector[0], SIZE_SLAVES, MPI_INT, source, 1, MPI_COMM_WORLD, &send_reqmaster);
				contEnviados++;
				if(contEnviados==((SIZE_MASTER/SIZE_SLAVES)+size-1)) fin = 1;
			}
			// Wait por si acaso fallara
			MPI_Wait(&send_reqmaster, &status); 
		}

		printf("FIN - MAESTRO ha enviado y recibido todo.\n");

	} else {// Slaves
		
		while(!fin){
			// Comprobamos mensajes pendientes
			MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status_msg_s);
			// Probe bloquea, solo pasará cuando se espera el msg con esas caract = aqui cualquier msg

			if(status_msg_s.MPI_TAG == 1){
				fin = 1;	// Se acabó, ya no recibo más

			} else {
				MPI_Irecv(&vector[0], SIZE_SLAVES, MPI_INT, 0, status_msg_s.MPI_TAG, MPI_COMM_WORLD, &recv_reqslave);
				MPI_Wait(&recv_reqslave, &status);
				sumaParcial = calcSuma(&vector[0], SIZE_SLAVES);
				MPI_Isend(&sumaParcial, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &send_reqslave);
				MPI_Wait(&send_reqslave, &status);	// por si acaso falla
			} 
		}
		printf("FIN - ESCLAVO %d.\n", rank);
	}
	
	// Sincronizo procesos
	MPI_Barrier(MPI_COMM_WORLD);
	
	if (rank==0){
		printf("MAESTRO - Suma total: %d\n", sumaTotal);
	}
	
	MPI_Finalize();
	return 0;
}

int calcSuma(int *v, int countElems){
	int i;
	int suma = 0;
	for (i = 0; i < countElems; i++){
		suma += v[i];
	}
	return suma;
}