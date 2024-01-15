#!/bin/bash
g++ -DEPSILON=16 -O3 -std=c++17 -I. -fopenmp -o serial_16 Serial.cpp
g++ -DEPSILON=32 -O3 -std=c++17 -I. -fopenmp -o serial_32 Serial.cpp
g++ -DEPSILON=64 -O3 -std=c++17 -I. -fopenmp -o serial_64 Serial.cpp

mpicxx -DT=8 -DEPSILON=16 -O3 -std=c++17 -I. -o mpi_8_16 mpi_impl.cpp -pthread -fopenmp
mpicxx -DT=16 -DEPSILON=16 -O3 -std=c++17 -I. -o mpi_16_16 mpi_impl.cpp -pthread -fopenmp
mpicxx -DT=16 -DEPSILON=32 -O3 -std=c++17 -I. -o mpi_16_32 mpi_impl.cpp -pthread -fopenmp
mpicxx -DT=32 -DEPSILON=32 -O3 -std=c++17 -I. -o mpi_32_32 mpi_impl.cpp -pthread -fopenmp
mpicxx -DT=32 -DEPSILON=32 -O3 -std=c++17 -I. -o mpi_32_64 mpi_impl.cpp -pthread -fopenmp
mpicxx -DT=64 -DEPSILON=64 -O3 -std=c++17 -I. -o mpi_64_64 mpi_impl.cpp -pthread -fopenmp

g++ -DT=8 -DEPSILON=16 -DPOOLSIZE=2 -O3 -std=c++17 -I. -o pthread_8_16_2 pthread.cpp -pthread -fopenmp
g++ -DT=8 -DEPSILON=16 -DPOOLSIZE=3 -O3 -std=c++17 -I. -o pthread_8_16_3 pthread.cpp -pthread -fopenmp
g++ -DT=8 -DEPSILON=16 -DPOOLSIZE=4 -O3 -std=c++17 -I. -o pthread_8_16_4 pthread.cpp -pthread -fopenmp
g++ -DT=16 -DEPSILON=16 -DPOOLSIZE=2 -O3 -std=c++17 -I. -o pthread_16_16_2 pthread.cpp -pthread -fopenmp
g++ -DT=16 -DEPSILON=16 -DPOOLSIZE=3 -O3 -std=c++17 -I. -o pthread_16_16_3 pthread.cpp -pthread -fopenmp
g++ -DT=16 -DEPSILON=16 -DPOOLSIZE=4 -O3 -std=c++17 -I. -o pthread_16_16_4 pthread.cpp -pthread -fopenmp
g++ -DT=16 -DEPSILON=32 -DPOOLSIZE=2 -O3 -std=c++17 -I. -o pthread_16_32_2 pthread.cpp -pthread -fopenmp
g++ -DT=16 -DEPSILON=32 -DPOOLSIZE=3 -O3 -std=c++17 -I. -o pthread_16_32_3 pthread.cpp -pthread -fopenmp
g++ -DT=16 -DEPSILON=32 -DPOOLSIZE=4 -O3 -std=c++17 -I. -o pthread_16_32_4 pthread.cpp -pthread -fopenmp
g++ -DT=32 -DEPSILON=32 -DPOOLSIZE=2 -O3 -std=c++17 -I. -o pthread_32_32_2 pthread.cpp -pthread -fopenmp
g++ -DT=32 -DEPSILON=32 -DPOOLSIZE=3 -O3 -std=c++17 -I. -o pthread_32_32_3 pthread.cpp -pthread -fopenmp
g++ -DT=32 -DEPSILON=32 -DPOOLSIZE=4 -O3 -std=c++17 -I. -o pthread_32_32_4 pthread.cpp -pthread -fopenmp
g++ -DT=32 -DEPSILON=64 -DPOOLSIZE=2 -O3 -std=c++17 -I. -o pthread_32_64_2 pthread.cpp -pthread -fopenmp
g++ -DT=32 -DEPSILON=64 -DPOOLSIZE=3 -O3 -std=c++17 -I. -o pthread_32_64_3 pthread.cpp -pthread -fopenmp
g++ -DT=32 -DEPSILON=64 -DPOOLSIZE=4 -O3 -std=c++17 -I. -o pthread_32_64_4 pthread.cpp -pthread -fopenmp
g++ -DT=64 -DEPSILON=64 -DPOOLSIZE=2 -O3 -std=c++17 -I. -o pthread_64_64_2 pthread.cpp -pthread -fopenmp
g++ -DT=64 -DEPSILON=64 -DPOOLSIZE=3 -O3 -std=c++17 -I. -o pthread_64_64_3 pthread.cpp -pthread -fopenmp
g++ -DT=64 -DEPSILON=64 -DPOOLSIZE=4 -O3 -std=c++17 -I. -o pthread_64_64_4 pthread.cpp -pthread -fopenmp
