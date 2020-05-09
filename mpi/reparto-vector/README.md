#mpi-reparto-vector-2020-afernv10
La tarea consiste en la distribución de N datos de un vector entre los esclavos por medio del uso de MPI.
Se repartirá de manera igualitaria si es posible; en el caso de no poder ser,
el último esclavo tendrá los datos restantes.

Compilación del archivo: mpicc mpiVector.c -o ejecutable

Ejecución: mpirun -np [numero de procesos] ./ejecutable
