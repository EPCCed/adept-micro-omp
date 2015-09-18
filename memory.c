/* Copyright (c) 2015 The University of Edinburgh. */

/* 
* This software was developed as part of the                       
* EC FP7 funded project Adept (Project ID: 610490)                 
* www.adept-project.eu                                            
*/

/* Licensed under the Apache License, Version 2.0 (the "License"); */
/* you may not use this file except in compliance with the License. */
/* You may obtain a copy of the License at */

/*     http://www.apache.org/licenses/LICENSE-2.0 */

/* Unless required by applicable law or agreed to in writing, software */
/* distributed under the License is distributed on an "AS IS" BASIS, */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. */
/* See the License for the specific language governing permissions and */
/* limitations under the License. */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <omp.h>

#include "level0.h"
#include "utils.h"


/* callocate 'size' amount of memory (in MB), then free it. */
int mem_calloc(unsigned int size){
    
  struct timespec start, end;

  int i;

  /* work out requested size in Bytes */
  int nbytes = size * 1048576;
  int nelements = nbytes / sizeof(int);

  int nthreads;
  # pragma omp parallel
  if (omp_get_thread_num() == 0) nthreads = omp_get_num_threads();

  int **arrays = (int **)malloc(nthreads * sizeof(int*));
  if (!arrays) {
      printf("Out of memory.\n");
      return 0;
  }

  clock_gettime(CLOCK, &start);

  # pragma omp parallel
  {
      /* allocate array of nbytes */
      int threadid = omp_get_thread_num();
      arrays[threadid] = (int *)calloc(nelements, sizeof(int));
  }
  clock_gettime(CLOCK, &end);
  
  elapsed_time_hr(start, end, "Allocate memory");
  
  printf("Finished callocating %d MB memory.\n\n", size);

  for (i = 0; i < nthreads; i++) {
      if(arrays[i] == NULL){
	  printf("Out Of Memory: could not allocate space for the array.\n");
	  return 0;
      }
      free(arrays[i]);
  }
 
  free(arrays); // Remember to free !
    
  return 0;
  
}

/* measure main memory (RAM) reads */
int mem_read_ram(unsigned long reps){

  struct timespec start, end;

  int i, a;
  volatile int b, c;

  srand((int)time(NULL));
  b = rand();
  c = rand();

  clock_gettime(CLOCK, &start);


  for(i=0; i<reps; i++){
    a = b + c;
  }

  clock_gettime(CLOCK, &end);
  elapsed_time_hr(start, end, "RAM reads");
  
  printf("Finished %lu RAM reads.\n\n", (2 * reps));
    
  return 0;
}

/* allocate 'size' amount of memory (in MB)        */
/* write a random value to all the array elements  */
/* in a contiguous manner and then free the memory. */
int mem_write_contig(unsigned int size){
  
  int i;
  /* work out number of bytes from size in MB */
  int nbytes = size * 1048576; 
  /* work out number of ints that fit into nbytes */
  int nelements = nbytes / sizeof(int); 

  double oh, rt;

  struct timespec start, end;
  
  srand((int)time(NULL));
  
  int data = rand();

  int nthreads;
  # pragma omp parallel
  if (omp_get_thread_num() == 0) nthreads = omp_get_num_threads();

  int **arrays = (int **)malloc(nthreads * sizeof(int*));
  if (!arrays) {
      printf("Out of memory.\n");
      return 0;
  }
  
  /* allocate array of nbytes for each thread */
  for (i = 0; i < nthreads; i++) {
      arrays[i] = (int *)malloc(nbytes);
      if (!arrays[i]) {
	  printf("Out of memory.\n");
	  return 0;
      }
  }
  
  /* loop overhead */
  clock_gettime(CLOCK, &start);

  # pragma omp parallel private(i)
  for(i = 0; i < nelements; i++){
    __asm__ ("nop");
  }
  
  clock_gettime(CLOCK, &end);
  oh = elapsed_time_hr(start, end, "Loop overhead for contiguous write");


  clock_gettime(CLOCK, &start);

  # pragma omp parallel private(i)
  {
      int *array = arrays[omp_get_thread_num()];

      /* write data to array in contiguous manner - loop over i */
      for(i = 0; i < nelements; i++){
	  array[i] = data;
      }
  }
  
  clock_gettime(CLOCK, &end);
  rt = elapsed_time_hr(start, end, "Write contiguously");
  
  printf("Make sure compiler keeps result: array[0] = %d\n", arrays[0][0]);

  printf("Runtime: %f\n", (rt-oh));  
  
  printf("Finished allocating %d MB memory for %d ints and "
	 "filling the array in a contiguous manner.\n\n", size, nelements);
  
  for (i = 0; i < nthreads; i++) {
      free(arrays[i]);
  }
  free(arrays); // Remember to free !
  /* free(indices); */

  return 0;
  
}

/* allocate 'size' MB of memory, write a random    */
/* value to all the  array elements in a strided   */
/* manner and then free the memory.                */
int mem_write_strided(unsigned int size, unsigned int stride){
  
  int i;
  int j, s, n = 0;
  unsigned int nbytes = size * 1048576;
  unsigned int strbytes = stride * 1024;

  /* calculate total number of elements to be allocated/written */
  unsigned int nelements = nbytes / sizeof(int);
  /* convert stride size into ints */
  unsigned int str = strbytes / sizeof(int);

  double oh, rt;

  struct timespec start, end;

  int nthreads;
  # pragma omp parallel
  if (omp_get_thread_num() == 0) nthreads = omp_get_num_threads();

  srand((int)time(NULL));
  
  int data = rand();
  
  /* perform the strided write for all stride values */
  /* from 2 up to str                                */
  for(s=2; s<=str; s=s*2){

    /* allocate 'size' MB of memory              */
    /* get a fresh allocation for each iteration */
    int **arrays = (int **)malloc(nthreads * sizeof(int*));
    if(arrays == NULL){
      printf("Out Of Memory: could not allocate space for the array.\n");
      return 0;
    }
    for (i = 0; i < nthreads; i++) {
	arrays[i] = (int *)malloc(nbytes);
	if(arrays[i] == NULL){
	    printf("Out Of Memory: could not allocate space for the array.\n");
	    return 0;
	}
    }
    
    printf("Memory size in ints: %d, stride in ints: %d\n", nelements, s);
    
    /* measure the overhead */
    clock_gettime(CLOCK, &start);

    # pragma omp parallel private(i,j)
    {
	int nl = 0;
	for(i = 0; i < s; i++){
	    for(j = 0; j < nelements; j=j+s){
		if(i+j < nelements) {
		    nl = i+j;
		}
		if(nl == nelements-1) break;
		nl++;
	    }
	    if(nl == nelements-1) break;
	}
	if (omp_get_thread_num() == 0) n = nl;
    }

    clock_gettime(CLOCK, &end);
    oh = elapsed_time_hr(start, end, "Overhead for strided write");

    /* make sure the compiler executes the loop that calculates n */
    printf("n = %d\n", n);

    n = 0;
    
    clock_gettime(CLOCK, &start);

    
    /* write data to array in strided manner */
    # pragma omp parallel private(i,j)
    {
	int *array = arrays[omp_get_thread_num()];
	int nl = 0;
	for(i = 0; i < s; i++){
	    for(j = 0; j < nelements; j=j+s){
		if(i+j < nelements) {
		    nl = i+j;
		    array[nl]=data;
		}
		if(nl == nelements-1) break;
		nl++;
	    }
	    if(nl == nelements-1) break;
	}
	if (omp_get_thread_num() == 0) n = nl;
    }
    
    clock_gettime(CLOCK, &end);
    rt = elapsed_time_hr(start, end, "Strided write");
    
    /* make sure the compiler executes the loop that calculates n */
    printf("n = %d, array[0] = %d\n", n, arrays[0][0]);

    printf("Runtime: %f\n", (rt-oh));  
  
    for (i = 0; i < nthreads; i++) {
	free(arrays[i]);
    }
    free(arrays); // Remember to free !
  
  }

  printf("Finished allocating %d MB memory and "
	 "filling the array in a strided manner "
	 "up to a stride size of %d KB.\n\n", size, stride);
      
  return 0;
  
}

/* allocate memory for 'size' number of elements   */
/* of type 'dt', write a random value to all the   */
/* array elements using random access and then     */
/* free the memory.                                */
int mem_write_random(unsigned int size){
  
  int i, n;
  unsigned int t;
  /* work out number of bytes from size in MB */
  int nbytes = size * 1048576;
  /* work out number of ints that fit into nbytes */
  int nelements = nbytes / sizeof(int);

  double oh, rt = 0.0;

  struct timespec start, end;
  
  t = (int)time(NULL);

  int nthreads;
  # pragma omp parallel
  if (omp_get_thread_num() == 0) nthreads = omp_get_num_threads();

  /* allocate 'size' MB of memory per thread */
  int **arrays = (int **)malloc(nthreads * sizeof(int*));
  if(arrays == NULL){
    printf("Out Of Memory: could not allocate space for the array.\n");
    return 0;
  }
  for (i = 0; i < nthreads; i++) {
      arrays[i] = (int *)malloc(nbytes);
      if(arrays[i] == NULL){
	  printf("Out Of Memory: could not allocate space for the array.\n");
	  return 0;
      }
  }

  srand(t);

  clock_gettime(CLOCK, &start);


  /* measure overheads */
  # pragma omp parallel private(i)
  {
      int ln;
      for(i=0; i<nelements; i++){
	  ln = rand() % (nelements);
      }
      n = ln;
  }

  clock_gettime(CLOCK, &end);
  oh = elapsed_time_hr(start, end, "Overhead for random write");

  printf("Keep result: n = %d\n", n);  
  
  srand(t);
  int data = rand();

  clock_gettime(CLOCK, &start);
  
  
  /* write data to array in random manner */
  # pragma omp parallel private(i, n)
  {
      int *array = arrays[omp_get_thread_num()];
      for(i = 0; i < nelements; i++){
	  n = rand() % (nelements);
	  array[n] = data;
      }
  }
    
  clock_gettime(CLOCK, &end);
  rt = elapsed_time_hr(start, end, "Write random");

  printf("Runtime: %f\n", (rt-oh));
  
  printf("Finished allocating %d MB memory (%d ints) and "
	 "filling the array in a random manner.\n\n", size, nelements);

  for (i = 0; i < nthreads; i++) {
      free(arrays[i]);
  }
  free(arrays); // Remember to free !

  return 0;
  
}


/* allocate 'size' amount of memory (in MB)        */
/* write a random value to all the array elements  */
/* in a contiguous manner, read back from memory   */
/* and, finally, free the memory.                  */
int mem_read_contig(unsigned int size){
  
    int i, j, n;
  /* work out number of bytes from size in MB */
  int nbytes = size * 1048576; 
  /* work out number of ints that fit into nbytes */
  int nelements = nbytes / sizeof(int); 
  
  double oh, rt;

  struct timespec start, end;
  
  srand((int)time(NULL));
  
  int data = rand();
  int receive = 0;

  int nthreads;
  # pragma omp parallel
  if (omp_get_thread_num() == 0) nthreads = omp_get_num_threads();

  int **arrays = (int **)malloc(nthreads * sizeof(int*));
  if (!arrays) {
      printf("Out of memory.\n");
      return 0;
  }
  
  /* allocate array of nbytes for each thread */
  for (i = 0; i < nthreads; i++) {
      arrays[i] = (int *)malloc(nbytes);
      if (!arrays[i]) {
	  printf("Out of memory.\n");
	  return 0;
      }
  }
  
  /* populate array with random values */
  for (j = 0; j < nthreads; j++) {
      for(i = 0; i < nelements; i++){
	  arrays[j][i] = data;
      }
  }

  clock_gettime(CLOCK, &start);


  /* loop overhead */
  # pragma omp parallel private(i)
  for(i=0; i<nelements; i++){
    __asm__ ("nop");
  }

  clock_gettime(CLOCK, &end);
  oh = elapsed_time_hr(start, end, "Loop overhead for contiguous read");
  
  clock_gettime(CLOCK, &start);

    
  /* read data back from array */
  # pragma omp parallel private(i)
  {
      int *array = arrays[omp_get_thread_num()];
      int lreceive;
      for(i = 0; i < nelements; i++){
	  lreceive = array[i];
      }
      receive = lreceive;
  }
  
  clock_gettime(CLOCK, &end);
  rt = elapsed_time_hr(start, end, "Read contiguously");
  
  printf("Keep result: receive = %d\n", receive);

  printf("Runtime: %f\n", (rt-oh));  

  printf("Finished reading %d MB memory (%d ints) in a contiguous manner.\n\n", size, nelements);
  
  for (i = 0; i < nthreads; i++) {
      free(arrays[i]);
  }
  free(arrays); // Remember to free !

  return 0;
  
}


/* allocate memory for 'size' number of elements   */
/* of type 'dt', write a random value to all the   */
/* array elements, read those values back in a     */
/* strided manner and then free the memory.        */
int mem_read_strided(unsigned int size, unsigned int stride){
  
  int i, j, s, n = 0;
  /* work out number of bytes from size in MB */
  unsigned int nbytes = size * 1048576;
  /* work out number of bytes from stride in KB */
  unsigned int strbytes = stride * 1024;

  /* calculate total number of elements to be allocated/read */
  unsigned int nelements = nbytes / sizeof(int);
  /* convert stride size into ints */
  unsigned int str = strbytes / sizeof(int);

  double oh, rt = 0.0;

  struct timespec start, end;

  int nthreads;
  # pragma omp parallel
  if (omp_get_thread_num() == 0) nthreads = omp_get_num_threads();

  srand((int)time(NULL));
  
  int data = rand();
  int receive = 0;

  /* perform the strided write for all stride values */
  /* from 2 up to str                                */
  for(s=2; s<=str; s=s*2){

    printf("Memory size in ints: %d, stride in ints: %d\n", nelements, s);
    
    /* allocate array of 'size' ints and write data in contiguous manner */
    /* get a fresh allocation for each iteration */
    int **arrays = (int **)malloc(nthreads * sizeof(int*));
    if(arrays == NULL){
      printf("Out Of Memory: could not allocate space for the array.\n");
      return 0;
    }
    for (i = 0; i < nthreads; i++) {
	arrays[i] = (int *)malloc(nbytes);
	if(arrays[i] == NULL){
	    printf("Out Of Memory: could not allocate space for the array.\n");
	    return 0;
	}
    }
    
    /* fill array with data - no need for striding here */  
    for (j = 0; j < nthreads; j++) {
	for(i = 0; i < nelements; i++){
	    arrays[j][i] = data;
	}
    }

    clock_gettime(CLOCK, &start);

    
    /* measure the overhead */
    # pragma omp parallel private(i, j)
    {
	int nl = 0;
	for(i = 0; i < s; i++){
	    for(j = 0; j < nelements; j=j+s){
		if(i+j < nelements){
		    nl = i+j;
		}
		if(nl == nelements-1) break;
		nl++;
	    }
	    if(nl == nelements-1) break;
	}
	n = nl;
    }
    
    clock_gettime(CLOCK, &end);
    oh = elapsed_time_hr(start, end, "Overhead for strided read");
    
    /* make sure the compiler executes the loop that calculates n */
    printf("n = %d\n", n);
    
    clock_gettime(CLOCK, &start);

    
    /* read data from array following strided pattern */  
    # pragma omp parallel private(i, j)
    {
	int nl = 0;
	int *array = arrays[omp_get_thread_num()];
	for(i = 0; i < s; i++){
	    for(j = 0; j < nelements; j=j+s){
		if(i+j < nelements){
		    nl = i+j;
		    receive = array[nl];
		}
		if(nl == nelements-1) break;
		nl++;
	    }
	    if(nl == nelements-1) break;
	}
	n = nl;
    }
    
    clock_gettime(CLOCK, &end);
    rt = elapsed_time_hr(start, end, "Read strided");
    
    /* make sure the compiler executes the loop that calculates n */
    printf("n = %d, receive = %d\n", n, receive);
    
    printf("Runtime: %f\n", (rt-oh));
    
    for (i = 0; i < nthreads; i++) {
	free(arrays[i]);
    }
    free(arrays); // Remember to free !
  }    
  
  printf("Finished allocating %d MB memory and "
	 "reading the array in a strided manner "
	 "with a stride size of %d KB.\n\n", size, stride);

  return 0;
  
}

/* allocate memory for 'size' number of elements   */
/* of type 'dt', write a random value to all the   */
/* array elements, read those values back in a     */
/* random manner and then free the memory.        */
int mem_read_random(unsigned int size){
  
    int i, j, n;
  unsigned int t;

  double oh, rt;

  /* work out number of bytes from size in MB */
  unsigned int nbytes = size * 1048576;
  /* calculate total number of elements to be allocated/read */
  unsigned int nelements = nbytes / sizeof(int);
  
  struct timespec start, end;
  
  t =(int)time(NULL);

  int nthreads;
  # pragma omp parallel
  if (omp_get_thread_num() == 0) nthreads = omp_get_num_threads();

  int data = rand();
  int receive = 0;
  
  /* allocate array of 'size' ints and write data in contiguous manner */
  int **arrays = (int **)malloc(nthreads * sizeof(int*));
  if(arrays == NULL){
    printf("Out Of Memory: could not allocate space for the array.\n");
    return 0;
  }
  for (i = 0; i < nthreads; i++) {
      arrays[i] = (int *)malloc(nbytes);
      if(arrays[i] == NULL){
	  printf("Out Of Memory: could not allocate space for the array.\n");
	  return 0;
      }
  }
  
  /* fill the array with random values */
  for (j = 0; j < nthreads; j++) {
      for(i = 0; i < nelements; i++){
	  arrays[j][i] = data;
      }
  }

  srand(t);

  /* measure the overhead */
  clock_gettime(CLOCK, &start);

  # pragma omp parallel private(i)
  {
      int nl;
      for(i=0; i<nelements; i++){
	  nl = rand() % (nelements);
      }
      n = nl;
  }

  clock_gettime(CLOCK, &end);
  oh = elapsed_time_hr(start, end, "Read random");
  
  printf("Make sure the result is kept: %d\n", n);

  srand(t);

  /* measure the random reads */
  clock_gettime(CLOCK, &start);

  # pragma omp parallel private(i, receive)
  {
      int receivel;
      int nl;
      int *array = arrays[omp_get_thread_num()];
      for(i = 0; i < nelements; i++){
	  nl = rand() % (nelements);
	  receivel = array[nl];
      }
      receive = receivel;
  }
  
  clock_gettime(CLOCK, &end);
  rt = elapsed_time_hr(start, end, "Read random");
  
  printf("Make sure the result is kept: %d\n", receive);

  printf("Runtime: %f\n", (rt-oh));

  printf("Finished reading %d MB of memory (%d ints) in a random manner.\n\n", size, nelements);
  
  for (i = 0; i < nthreads; i++) {
      free(arrays[i]);
  }
  free(arrays); // Remember to free !

  return 0;
  
}
