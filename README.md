Copyright (c) 2015 The University of Edinburgh.
 
This software was developed as part of the                       
EC FP7 funded project Adept (Project ID: 610490)                 
    www.adept-project.eu                                            

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

# Adept Micro Benchmarks

This README file introduces the different benchmarks in this suite, including, where necessary details on implementation and usage.

## Citation & Further Information
If you would like to cite this work, please cite:
Nick Johnson et al., "Adept Deliverable D2.3 - Updated Report on Adept Benchmarks", September 2015.
available at http://www.adept-project.eu/images/Deliverables/Adept%20D2.3.pdf


## Basic Operations

A benchmark to measure the basic operations (add, sub, mult, div) on scalar data types (int, long, float, double).

Each run of the benchmark consists of:

- a warm-up loop
- a timed loop with a `nop` instruction
- an empty timed loop
- loops with the basic operation of choice, with increasing work per iteration and correspondingly reduced number of iterations (i.e. the number of iteration remains consistent).
  
The `nop` and empty loops are used to get an understanding of the overheads incurred by the loop. Similary, increasing the number of operations per loop iterations while reducing the number of iterations gives an idea of how work inside a loop impacts performance and power.

The user can choose if the data used as part of the operations is volatile (i.e. read from memory at each access). The options are no volatile variables (default), ONEVOL where 1 of the variable is declared volatile, or ALLVOL where all the variables are volatile.

## Branch & Jump

The branch and jump benchmarks exercise instructions that determine the path of execution and measure the cost of those instructions. They are repeated in a loop with a user-defined number of iternations. They are:

- switch: the `switch` benchmark jumps to a `case` based on the value of one element of an input array. The benchmark is executed in a loop with a user-defined number of iterations. The array element is chosen based on the current loop iterator.  
- if/else: these benchmarks aim to measure the impact of taking or not taking certain code branches. Seven different patterns are being exercised: 
  1.always take if branch (all TRUE)
  2. always take else branch (all FALSE)
  3. repeat pattern of if, else, if, else (TRUE-FALSE-TRUE-FALSE)
  4. repeat pattern of 2 if, 2 else (2xTRUE 2xFALSE)
  5. repeat pattern of 4 if, 4 else (4xTRUE 4xFALSE)
  6. repeat pattern of 8 if, 8 else (8xTRUE 8xFALSE)
  7. random pattern of if and else (Random TRUE FALSE)
In order to make both pre-fetching and branch prediction difficult, the loops are stuffed with computation. Both the loop and computation are measure separately in a loop withouth branches in order to be able to discount their overheads.

## Function Calls

A benchmark to measure the overhead of calling a function.

The benchmark uses an approximation of pi as the computational workload. The workload computation is performed directly, as well as using a function or a recursive function. The difference between the different implementation represents the function overhead.

This benchmark must be compiled without optimisation (-O0) to avoid the compiler inlining the function calls.

## I/O
The disk benchmark exercises the I/O subsystem by performing common file based operations. There are seven options to the benchmarks:

1. `mk_rm_dir`: a number of directories are created within a single directory (creating a flat non-recursive structure) and then deleted. The number may be specified by the user. 
2. `file_write`: a single file is written contiguously with randomly generated data that is generated outside of the write loop. The size of the file may be specified by the user. 
3. `file_read`: a single file filled with randomly generated data is read contiguously. The file is generated outside of the measurement loop and its size may be specified by the user. 
4. `file_write_random`: a single file is filled with randomly data, again generated outside of the write loop, with writes occurring at randomised locations in the file. The size of the file may be specified by the user. 
5. `file_read_random`: a single file filled with randomly generated data is read where the location (block) to be read is randomly selected. The file is generated outside of the measurement loop and its size may be specified by the user. 
6. `file_read_direct`: as per `file_read`, but where the file is opened with the `O_DIRECT` flag which requests the OS perform as little caching of the file as possible. The size of the file may be specified by the user. 
7. `file_read_random_direct`: as per `file_read_random` where the file is opened with the `O_DIRECT` flag which requests the OS perform as little caching of the file as possible. The size of the file may be specified by the user. 

## Memory
The memory benchmark is designed to exercise all levels of the memory hierarchy of the system under test and observe conditions when hierarchical boundaries are crossed, for example, from L1 cache to L2 cache.The size of the memory block to use for the benchmark is user defined and as such can be adjusted to explore said boundaries.

Accesses to the block of memory can be made in one of three ways: contiguous, strided or random. For write benchmarks, a single data value is pre-computed and assigned to each desired element of the array. For read benhmarks, the array is pre-filled with random data. 

* For the contiguous access case, elements of the array are accessed in order of monotonically increasing index. Each element of the array is accessed once, and once only.
* For the strided access case, the array is treated as a circular buffer. The indices of the elements to be accessed increase by a constant, the stride length, which begins at two elements, and doubles on each pass to a maximum the value requested by the user. For example, if the user requests a stride length of 4, the benchmark will be run twice, first using a stride length of 2 and then again using a stride length of 4. Because the array is considered quasiTcircular, all elements of the array are accessed for each stride length. QuasiTcircular, in this case, means that for each pass through the array, the offset increases by 1. For example, with an array of length 10, and a stride length of 2, the elements would be accessed in the following order: 0, 2, 4, 8, 1, 3, 5, 7, 9. A true circular buffer would see only elements 0, 2, 4 and 8 accessed. Here, when the end of the array is reached, the offset, initially 0, is increased by 1, allowing access to elements 1 (0+1), 3 (2+1), 5 (4+1), 7 (6+1) and 9 (8+1). Each element of the array is accessed once, and once only, for each stride length. 
* For the random access case, the element of the array to be accessed is determined randomly, once per iterationY the number of iterations is equal to the number of elements in the array. The randomTaccess case does not store a list of previously accessed elements so it is likely that some elements may be accessed more than once and some never accessed.

The memory benchmark also has an option to measure a calloc operation, i.e. assigning and zeroTing a block of memory, for a user-specified amount of memory.