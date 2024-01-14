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
