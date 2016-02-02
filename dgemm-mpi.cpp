const char *dgemm_desc = "Naive, three-loop dgemm with MPI.";
void square_dgemm(int n, int start, int end, double *A, double *B, double *C , double *T)
{
  for( int i = start; i < end; i++ )
       for( int j = 0; j < n; j++ ) 
       {
            double cij = C[j*n+i];
            for( int k = 0; k < n; k++ )
                 cij += A[i*n+k] * B[k*n+j];
            T[j*n+i] = cij;
       }
  }
void square_dgemm_notemp(int n, int start, int end, double *A, double *B, double *C )
{
  for( int i = start; i < end; i++ )
       for( int j = 0; j < n; j++ ) 
       {
            double cij = C[j*n+i];
            for( int k = 0; k < n; k++ )
                 cij += A[i*n+k] * B[k*n+j];
            C[j*n+i] = cij;
       }
  }
