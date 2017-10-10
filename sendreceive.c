#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>

int main( argc, argv )
int  argc;
char **argv;
{
	int id, size;
	double* numbers;
	double* received;
	double t1,t2; 
	double n=atof(argv[1]);
	MPI_Init( &argc, &argv );
	MPI_Comm_size( MPI_COMM_WORLD, &size );
	MPI_Comm_rank( MPI_COMM_WORLD, &id );
	int chunk_size =  n/size;
	int level= 2;
	double localMin=99999.0;
	//printf( "Hello world from process %d of %d\n", id, size );
	if(id == 0){
		//printf("chunk size:%d ",chunk_size);
		numbers= (double*) malloc(n*sizeof(double));
		int i=0;
		for(;i<n;i++){
			numbers[i] = (double)rand() / (double)rand(); 
		}
		numbers[(int)n/2] = -5;
		i=1;
		for(;i<size;i++){
			MPI_Send(&numbers[i*chunk_size],chunk_size,MPI_DOUBLE,i,5,MPI_COMM_WORLD);
		
		}
	}
	else{
				received = (double*) malloc(chunk_size*sizeof(double));
				MPI_Recv(received,chunk_size,MPI_DOUBLE,0,5,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	}
	
	if(id == 0){
		
		t1 = MPI_Wtime();
		int i=0;
		for(;i<chunk_size;i++)
			if(localMin > numbers[i])
				localMin = numbers[i];	
	}else {
		
		int j=0;
				for(;j<chunk_size;j++)
					if(localMin > received[j])
						localMin = received[j];
		
		
	}
	while(level <= size){

		if(id%level == 0){

			double receivedLocalMin;
			MPI_Recv(&receivedLocalMin,1,MPI_DOUBLE,id + level/2,3,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			//printf("compared %lf %lf\n",localMin,receivedLocalMin);
			localMin = (localMin < receivedLocalMin) ? localMin : receivedLocalMin ;
		}

		if((id % level) == level/2){

			//printf("%lf send from %d\n",localMin,id);
			MPI_Send(&localMin,1,MPI_DOUBLE,id -level/2,3,MPI_COMM_WORLD);
		}

		level *= 2;
	}
	
	if(id==0){
		t2 = MPI_Wtime();
		 printf("%lf\n%lf\n",localMin,(t2-t1));
	}
    MPI_Finalize();
    return 0;
}

