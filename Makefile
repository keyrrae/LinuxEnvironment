#Comet
#MKLPATH=/opt/intel/Compiler/11.1/072/mkl
MKLPATH=$MKL_ROOT
CC=icpc
MPI=mpicxx
CFLAGS = -I$(MKLPATH)/include  

MKLFLAGS=  -I$MKL_ROOT/include ${MKL_ROOT}/lib/intel64/libmkl_scalapack_lp64.a -Wl,--start-group ${MKL_ROOT}/lib/intel64/libmkl_intel_lp64.a ${MKL_ROOT}/lib/intel64/libmkl_core.a ${MKL_ROOT}/lib/intel64/libmkl_sequential.a -Wl,--end-group ${MKL_ROOT}/lib/intel64/libmkl_blacs_intelmpi_lp64.a 

LDFLAGS = $(MKLFLAGS) -lpthread -lm 

OMPFLAGS = -openmp

PTHFLAGS = -pthread

all:	benchmark-naive benchmark-blocked benchmark-blas benchmark-naive-omp benchmark-naive-mpi benchmark-blas-pthread

benchmark-naive: benchmark.o dgemm-naive.o
	$(CC) -o $@ $^ $(LDFLAGS)
benchmark-blocked: benchmark.o dgemm-blocked.o
	$(CC) -o $@ $^ $(LDFLAGS)
benchmark-blas: benchmark.o dgemm-blas.o
	$(CC) -o $@ $^ $(LDFLAGS)
benchmark-blas-pthread: benchmark-pthread.o dgemm-blas-pthread.o
	$(CC) -o $@ $^ $(LDFLAGS) $(PTHFLAGS)
benchmark-naive-omp: benchmark-naive-omp.o dgemm-naive-omp.o
	$(CC) -o $@ $^ $(LDFLAGS) $(OMPFLAGS)
benchmark-naive-mpi: benchmark-mpi.o dgemm-naive-mpi.o
	$(MPI) -o $@ $^ $(LDFLAGS)

dgemm-naive-omp.o: dgemm-naive-omp.cpp
	$(CC) $(CFLAGS) $(OMPFLAGS) -c  $<

dgemm-blas_pthread.o: dgemm-blas-pthread.cpp
	$(CC) $(CFLAGS) $(PTHFLAGS) -c $<

dgemm-naive-mpi.o: dgemm-naive-mpi.cpp
	$(MPI) $(CFLAGS) -c $<

benchmark-naive-omp.o: benchmark-naive-omp.cpp
	$(CC) $(CFLAGS) $(OMPFLAGS) -c $<

benchmark-mpi.o: benchmark-mpi.cpp
	$(MPI) $(CFLAGS) -c $<

benchmark-pthread.o: benchmark-pthread.cpp
	$(CC) $(CFLAGS) $(PTHFLAGS) -c $<

%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f benchmark-naive benchmark-blocked benchmark-blas benchmark-naive-omp benchmark-naive-mpi benchmark-blas-pthread *.o

