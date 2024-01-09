.PHONY: clean all



all: pthread mpi
	
mpi: mpi_impl.cpp
	mpicxx -O3 -std=c++17 -I. -o mpi mpi_impl.cpp -pthread

pthread: pthread.cpp
	g++ -O3 -std=c++17 -o pthread pthread.cpp -pthread

clean:
	rm pthread mpi