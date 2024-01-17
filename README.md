# What is this about?
This repo consists of various parallel algorithms that I implemented in C using
popular libraries like pthreads, openmp and mpi(in single machine simulations),
which I implemented during early 2020 while pursuing Msc., Mathematics and Computing at IIT Guwahati.

# Contents
## parallel_add
naive_sum.c which is single threaded code and recursive_add.c which is
parallel mpi simulation in a single machine.

## parallel_sort
msort_omp.c has merge sort implementation using openmp pragmas.
odd_even.c has merge sort implementation using mpi framework, it works based on the specification here https://en.wikipedia.org/wiki/Odd%E2%80%93even_sort

## Matrix Multiplications
Basic implementation can be found at naive.c.
opt1.c shows how to make use of cache hits to achieve better performance.
recursive_multiply.c is the have the best algorithmic complexity.
mpi implementation of scatter and gather for matrix multiplication.

## kmp (Knuth Morris Patt) pattern matching
Single threaded and multi threaded implementation using pthreads can be found

## dfa pattern matching
Single threaded and multi threaded implementation using pthreads can be found

## fft
Not complete and incorrect
