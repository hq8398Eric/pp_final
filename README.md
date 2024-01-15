# Parallelizing the “Lazy Approach” Framework in Approximating Dynamic Bipartite Matching

### How to compiler the problem for test:
To generate the program for test, we can use `make test` to generate programs with different epsilon and y.
After `make test`, we can run the program with generated graph.
For example:
```
make test
# for pthread and serial
./pthread_8_16_2 graph_10000_10000_100000.txt
# this will show the running time of pthread with epsilon 16, y 8 and 2 worker with input file graph_10000_10000_100000.txt

# for openMPI
mpirun -n 4 --hostfile <hostfile> ./mpi_8_16 graph_10000_10000_100000txt
```

### How to generate random bipartile graph:
To generate random bipartile graph, you can use generate_sparse to do it.
First use `make generate_sparse` to compile the program and than use it.
For example:
```
make generate_sparse
./generate_sparse 10000 10000 1000000
# this will give you a graph with filename graph_10000_10000_10000.txt
``