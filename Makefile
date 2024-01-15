.PHONY: clean all test

all: pthread mpi serial generate_sparse.cpp
	
mpi: mpi_impl.cpp hopcroftKarp.h
	mpicxx -O3 -std=c++17 -I. -o mpi mpi_impl.cpp -pthread -fopenmp

pthread: pthread.cpp hopcroftKarp.h
	g++ -O3 -std=c++17 -I. -o pthread pthread.cpp -pthread -fopenmp
	
serial: Serial.cpp hopcroftKarp.h
	g++ -O3 -std=c++17 -I. -o serial Serial.cpp -fopenmp
	
generate_sparse: generate_sparse.cpp
	g++ -O3 -std=c++17 -I. -o generate_sparse generate_sparse.cpp -fopenmp

test:
	./gen_different_version.sh


clean:
	rm pthread_* mpi_[0-9_]* serial_* generate_sparse