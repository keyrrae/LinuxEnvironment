#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <float.h>
#include <math.h>

#include <sys/time.h>
//#include "mkl.h"
#include <mkl.h>
#include <omp.h>
#include <pthread.h>
#include "dgemm-blas-pthread.h"
//#include <acml.h> //assumes AMD platform
/*
#ifdef __cplusplus
extern "C"
{
#endif
   #include <cblas.h>
#ifdef __cplusplus
}
#endif
*/
/* Your function must have the following signature: */

extern const char* dgemm_desc;
extern void *square_dgemm( void *arg );
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

void fill( double *p, int n )
{
    for( int i = 0; i < n; i++ )
        p[i] = i;//2 * drand48( ) - 1;
}

void absolute_value( double *p, int n )
{
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

void matrix_copy(double *src, double *dest, int col, int row, int start, int n){
	for (int i = 0; i < row; i++){
		for (int j = 0; j < col; j++){
			dest[i*col+j] = *(src + start + j + n * i);
		}
	}
}
 

/* The benchmarking program */

int main( int argc, char **argv )
{
    printf ("Description:\t%s\n\n", dgemm_desc);
    if (argc != 2)
	{
	printf("Usage: %s n\n  where n is no. of thread\n", argv[0]);
	exit(1);
	}
	int n_pthreads = atoi(argv[1]);/* These sizes should highlight performance dips at multiples of certain powers-of-two */
	int	n = 4;
	pthread_t *threads;
	struct parm *arg;
	
	threads = (pthread_t *) malloc(n_pthreads * sizeof(pthread_t));
	arg=(struct parm *)malloc(sizeof(struct parm)*n_pthreads);
	/*For each test size*/
		/*Craete and fill 3 random matrices A,B,C*/
        double *A = (double*) malloc( n * n * sizeof(double) );
        double *B = (double*) malloc( n * n * sizeof(double) );
        double *C = (double*) malloc( n * n * sizeof(double) );
        fill( A, n * n );
        fill( B, n * n );
        //fill( C, n * n );
        int blocksize = n/n_pthreads;
        memset( C, 0, sizeof( double ) * n * n );
		for (int i = 0; i < n_pthreads; i ++){
			arg[i].n = n;
			arg[i].blocksize = blocksize;
			arg[i].A = A + i * blocksize * n;
			arg[i].B = B;
			arg[i].C = C + i * blocksize *n;
			int success = pthread_create(&threads[i], NULL, square_dgemm, (void *)(arg+i));
			if (success != 0){
				printf("Creation of No. %d thread failed!", i);fflush(stdout);
			}
		}
		printf("cp\n");fflush(stdout);
		for (int i = 0; i < n_pthreads; i++) {
			int success = pthread_join(threads[i], NULL);
			if (success != 0){
				printf("Joining No. %d thread failed!", i);fflush(stdout);
			}
		 }
		 free(threads);
		 free(arg);
		 /*
		for (int i = 0; i < n_pthreads ;i ++){
			
        	cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, blocksize, n, n, 1, A + i*blocksize*n, n, B,n, 1, C+i*blocksize*n,n );

		}*/
		
		printf("cp\n");fflush(stdout);
		if (n % n_pthreads > 0) {
			square_dgemm_notemp(n, (n/n_pthreads)*n_pthreads, n, A, B, C);
		}	
		print_matrix(A,n);
		printf("\n");
		print_matrix(B,n);
		printf("\n");
		print_matrix(C,n);
		printf("\n");
        memset( C, 0, sizeof( double ) * n * n );
        //square_dgemm( n, A, B, C );
		print_matrix(C,n);
		printf("\n");
		/*Subtract A*B from C using standard dgemm (note that this should be 0 to within machine roundoff)*/
        //dgemm( 'N','N', n,n,n, -1, A,n, B,n, 1, C,n );
        cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, n,n,n, -1, A,n, B,n, 1, C,n );
		/*Subtract the maximum allowed roundoff from each element of C*/
        absolute_value( A, n * n );
        absolute_value( B, n * n );
        absolute_value( C, n * n );
        //dgemm( 'N','N', n,n,n, -3.0*DBL_EPSILON*n, A,n, B,n, 1, C,n );
        cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, n,n,n, -3.0*DBL_EPSILON*n, A,n, B,n, 1, C,n );
		/*After this test if any element in C is still positive something went wrong in square_dgemm*/
        for( int i = 0; i < n * n; i++ )
            if( C[i] > 0 )
            {
                printf( "FAILURE: error in matrix multiply exceeds an acceptable margin\n" );
                exit(-1);
            }

		/*Deallocate memory*/
        free( C );
        free( B );
        free( A );
    
    
    return 0;
}
