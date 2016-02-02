
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <float.h>
#include <math.h>

#include <sys/time.h>
//#include "mkl.h"
#include <mkl.h>
#include <omp.h>
#include <mpi.h>
//#include <acml.h> //assumes AMD platform


/* Your function must have the following signature: */

extern const char* dgemm_desc;
extern void square_dgemm(int n, int start, int end, double *A, double *B, double *C , double *T);
extern void square_dgemm_notemp(int n, int start, int end, double *A, double *B, double *C );

/* Helper functions */

double read_timer( )
{
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday( &start, NULL );
        initialized = true;
    }

    gettimeofday( &end, NULL );

    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

void fill( double *p, int n ) {
    for( int i = 0; i < n; i++ )
        p[i] = i;//2 * drand48( ) - 1;
}

void absolute_value( double *p, int n ) {
    for( int i = 0; i < n; i++ )
        p[i] = fabs( p[i] );
}

void print_matrix(double *p ,int n) {
  	int i, j = 0;
    for (i=0; i<n; i++) {
	    printf("\n\t| ");
		for (j=0; j<n; j++){
		   printf("%.2g ", p[i*n+j]);
		}
		printf("|");
	}
}


/* The benchmarking program */

int main( int argc, char **argv )
{
    int done = 0, myid, numprocs, i;
	int from, to;
    int  namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
	double seconds, Mflop_s;;
	
	int root_process = 0;
	int n_iterations = 1, iter = 0;
	int n = 1600; 

   	double *A = (double*) malloc( n * n * sizeof(double) );
   	double *B = (double*) malloc( n * n * sizeof(double) );
   	double *C = (double*) malloc( n * n * sizeof(double) );
    
	MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    MPI_Get_processor_name(processor_name,&namelen);
	
	/* These sizes should highlight performance dips at multiples of certain powers-of-two */
	/*Craete and fill 3 random matrices A,B,C*/
	from = myid * n/numprocs;
	to = (myid+1) * n/numprocs;
	if(myid == root_process){
    	printf ("Description:\t%s\n\n", dgemm_desc);
		n_iterations = 1;
	}

START:	
	if (myid == root_process){
		

    	fill( A, n * n );
    	fill( B, n * n );
    	//fill( C, n * n );
        memset( C, 0, sizeof( double ) * n * n );

	}
	MPI_Bcast(A, n * n, MPI_DOUBLE, 0,MPI_COMM_WORLD);
	MPI_Bcast(B, n * n, MPI_DOUBLE, 0,MPI_COMM_WORLD);
	MPI_Bcast(C, n * n, MPI_DOUBLE, 0,MPI_COMM_WORLD);


	if(myid == root_process){
		iter = 0;
	}
	
	double *T = (double*) malloc( n * n * sizeof(double) );
	
ITERATION:
   	if(myid == root_process){
   		seconds = MPI_Wtime();
   	}
   	

	square_dgemm(n, from, to, A, B, C, T);
//	MPI_Barrier(MPI_COMM_WORLD);

	MPI_Gather(T + from * n,
               		n * (n / numprocs),
			        MPI_DOUBLE,
					C + from * n,
				    n * (n / numprocs),
				    MPI_DOUBLE,
				    0,
				    MPI_COMM_WORLD);




/*		
		if (iter < n_iterations){
			iter++;
			goto ITERATION;
		}

        seconds = MPI_Wtime() - seconds;
        if (seconds < 0.1){
			n_iterations *= 2;
			goto START;
		}
*/
        seconds = MPI_Wtime() - seconds;
		Mflop_s = 1e-6 * n_iterations * n * n * n / seconds;
		printf("Mflops: %g time: %g \n", Mflop_s, seconds);
        
		cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, n,n,n, -1, A,n, B,n, 1, C,n );
		/*Subtract the maximum allowed roundoff from each element of C*/
        absolute_value( A, n * n );
        absolute_value( B, n * n );
        absolute_value( C, n * n );
        //dgemm( 'N','N', n,n,n, -3.0*DBL_EPSILON*n, A,n, B,n, 1, C,n );
        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, n,n,n, -3.0*DBL_EPSILON*n, A,n, B,n, 1, C,n );
		/*After this test if any element in C is still positive something went wrong in square_dgemm*/
        for( int i = 0; i < n * n; i++ )
            if( C[i] > 0 ) {
                printf( "FAILURE: error in matrix multiply exceeds an acceptable margin\n" );
                exit(-1);
            }

/*		
		if (iter < n_iterations){
			iter++;
			goto ITERATION;
		}

        seconds = MPI_Wtime() - seconds;
        if (seconds < 0.1){
			n_iterations *= 2;
			goto START;
		}
*/

    
	}
		//                                     }
		/*Deallocate memory*/
        free( C );
        free( B );
        free( A );
		free(T);
		//                                         }

	/*		
		for(an_id = 1; an_id < num_procs; an_id++) { 
			start_row = an_id * avg_rows_per_process + 1; 
			end_row = (an_id + 1) * avg_rows_per_process; 
			if((num_rows - end_row) < avg_rows_per_process) {
				end_row = num_rows - 1;
			}
			num_rows_to_send = end_row - start_row + 1; 
			
			MPI_Send( &num_rows_to_send, 1 , MPI_INT, an_id, send_data_tag, MPI_COMM_WORLD); 
			MPI_Send( &A[start_row], num_rows_to_send, MPI_INT, an_id, send_data_tag, MPI_COMM_WORLD); }
			MPI_Send( &B[start_row], num_rows_to_send, MPI_INT, an_id, send_data_tag, MPI_COMM_WORLD); }
			MPI_Send( &A[start_row], num_rows_to_send, MPI_INT, an_id, send_data_tag, MPI_COMM_WORLD); }



	} else {


	}
		
		for( int i = 0; i < n; i++ )
       for( int j = 0; j < n; j++ ) 
       {
            double cij = C[i+j*n];
            for( int k = 0; k < n; k++ )
                 cij += A[i+k*n] * B[k+j*n];
            C[i+j*n] = cij;
       }
  }*/
    /*  measure Mflop/s rate; time a sufficiently long sequence of calls to eliminate noise*/
    /*double Mflop_s, seconds = -1.0;
    for( int n_iterations = 1; seconds < 0.1; n_iterations *= 2 ) {
        square_dgemm( n, A, B, C );
            
        //seconds = read_timer( );
        seconds = MPI_Wtime();
        for( int i = 0; i < n_iterations; i++ )
           square_dgemm( n, A, B, C );
        seconds = MPI_Wtime() - seconds;
           
        Mflop_s = 2e-6 * n_iterations * n * n * n / seconds;
    }
    printf ("Size: %d\tMflop/s: %g\tParallel Time(s): %g\n", n, Mflop_s, seconds);
      */  
        /*  Ensure that error does not exceed the theoretical error bound */
		
		/* Set initial C to 0 and do matrix multiply of A*B */
        //memset( C, 0, sizeof( double ) * n * n );
        //square_dgemm( n, A, B, C );
		/*Subtract A*B from C using standard dgemm (note that this should be 0 to within machine roundoff)*/
        //dgemm( 'N','N', n,n,n, -1, A,n, B,n, 1, C,n );
    
    MPI_Finalize();
    return 0;
}
