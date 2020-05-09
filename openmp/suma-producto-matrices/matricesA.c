#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#ifdef _OPENMP
#  include <omp.h>
#endif
#include <stdlib.h>
#include <stdbool.h>

#define TAM 500           // Tamaño de matriz cuadrada
#define NUM_THREADS 16     // Número de hilos
#define N_MAX 99          // Valores de matrices de 0 - N_MAX
#define CHUNKSIZE 8       // Tamaño de bloque para clausula schedule
#define ANIDAMIENTO true  

int main(){

  // Declaración de variables
  int i, j, k, aux = 0, chunk;
  int A[TAM][TAM], B[TAM][TAM], mat_mult[TAM][TAM], mat_suma[TAM][TAM];
  clock_t t1,t2;
  srand(time(NULL));  // Semilla de aleatoriedad en matrices
   
  omp_set_num_threads(NUM_THREADS); // Especificamos nHilos
  omp_set_nested(ANIDAMIENTO);      // Activamos anidamiento o no

  # ifdef _OPENMP
  #pragma omp parallel
    { 
  #   pragma omp single 
      printf("OpenMP-parallel with %1d threads\n", omp_get_num_threads());
    } /* end omp parallel */
  #endif
  printf("Anidamiento: %d\nChunksize: %d\n", ANIDAMIENTO, CHUNKSIZE);
  // ***** Inicialización de matrices ***** //
  for(i=0; i<TAM; i++){
    for(j=0; j<TAM; j++){
      A[i][j] = (int) rand() % (N_MAX+1);   // Valores de 0 a 99
    }
  }
  for(i=0;i<TAM;i++){
    for(j=0;j<TAM;j++){
      B[i][j] = (int) rand() % (N_MAX+1); // Valores de 0 a 99
    }
  }
  // Matrices resultado
  for(i=0; i<TAM; i++){
    for(j=0; j<TAM; j++){
      mat_suma[i][j] = 0;
      mat_mult[i][j] = 0;
    }
  }
  chunk=CHUNKSIZE;

  t1 = clock(); // Iniciamos tiempo
  
  // ***** INICIO OMP PARALLEL ***** //
  #pragma omp parallel private(i,j,k) 
  {
    #pragma omp sections  
    {
      /* SECCIÓN PARA SUMA */
      #pragma omp section
      { 
        /* OPCIÓN A: Dos bucles for para recorrer i y j*/
        #pragma omp parallel for private(i,j) schedule(static, chunk)
        for(i=0;i<TAM;i++){
          for(j=0;j<TAM;j++){
            mat_suma[i][j] = (A[i][j]+B[i][j]); 
            //printf("Hilo suma %d / %d, it %d\n", omp_get_thread_num(), omp_get_num_threads(), i); 
          }
        } // end omp parallel for
        /* FIN OPCIÓN A */

      } // end section SUMA

      /**********************************************/
      /**********************************************/

      /* SECCIÓN PARA MULTIPLICACION */
      #pragma omp section
      {
        /* OPCIÓN A: Dos bucles for para recorrer i y j*/ 
        // Descomentar/comentar los // antes del /+*
        ///*
        #pragma omp parallel for private(i,j,k) schedule(static, chunk)
        for(i=0;i<TAM;i++){
          for(j=0;j<TAM;j++){
            aux = 0;
            for(k=0;k<TAM;k++){
              aux+=(A[i][k]*B[k][j]);
            }
            mat_mult[i][j] = aux;
            //printf("Hilo mult %d / %d, it %d\n", omp_get_thread_num(), omp_get_num_threads(), i); 
          }
        } // end omp parallel for
        //*/
        /* FIN OPCIÓN A */

      } // end section MULTIPLICACION
    } // end sections
  } // end omp parallel
  t2 = clock();
  
  //Imprimo las matrices resultado
  /*
  printf("Suma\n");
  for (i=0;i<TAM;i++){
    for (j=0;j<TAM;j++){
      printf("%d\t", mat_suma[i][j]);
    }
    printf("\n");
  }

  
  printf("Multiplicación\n");
  for (i=0;i<TAM;i++){
    for (j=0;j<TAM;j++){
      printf("%d\t", mat_mult[i][j]);
    }
    printf("\n");
  }
  */
  
  printf( "CPU time (clock)                = %12.4g ms\n", (t2-t1)/1000.0 );
  // Fin del programa
  
  return 0;
}
