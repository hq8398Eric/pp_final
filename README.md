
# Parallelizing the “Lazy Approach” Framework in Approximating Dynamic Bipartite Matching
A final project for NYCU class "Parallel Programming"
Aims to parallelize the "lazy approach" in many theoratical work in dynamic matching.

Read [this](https://docs.google.com/presentation/d/1FF85iwKjKEkPBgv1qmPeoQkJosQoKABCJ6n0V56dnUQ/edit?usp=sharing) to know more about the details

```pthread.cpp``` and ```mpi_impl.cpp``` are implementation in different frameworks

```generate.cpp``` and ```generate_sparse.cpp``` are of test purpose, generates the graph in the format
```
n m
va_1 vb_1
va_2 vb_2
.
.
.
va_m vb_m
q
d x y
```
vertices for two sides are independent, both 0 indexed.
each query is a modification to the edge set.
Starting with a d, d = 0 stands for deletion while d = 1 stands for addition.

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
```
