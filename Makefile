.PHONY: clean all

all: pthread mpi serial serial_parallel_hk
	
mpi: mpi_impl.cpp hopcroftKarp.h
	mpicxx -O3 -std=c++17 -I. -o mpi mpi_impl.cpp -pthread -fopenmp

pthread: pthread.cpp hopcroftKarp.h
	g++ -O3 -std=c++17 -I. -o pthread pthread.cpp -pthread -fopenmp
	
serial: Serial.cpp hopcroftKarp.h
	g++ -O3 -std=c++17 -I. -o serial Serial.cpp -fopenmp

serial_parallel_hk: Serial_parallel_HK.cpp hopcroftKarp.h
	g++ -O3 -std=c++17 -I. -o serial_parallel_hk Serial_parallel_HK.cpp -fopenmp

clean:
	rm pthread mpi
