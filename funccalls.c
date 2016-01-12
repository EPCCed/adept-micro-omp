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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <omp.h>

#include "utils.h"

/* Part of the Pi approximation calculation */
double piapprox(int index, unsigned int n){
  double approx;
  approx = 1.0/( 1.0 + (((double) index)-0.5)/((double) n) *  (((double) index)-0.5)/((double) n) );
  return approx;
}

/* Recursive part of the Pi approximation calculation */
double recur_piapprox(int index, unsigned int n){
  double result;
  double approx;

  if (index == 1) result = 1.0 / ( 1.0 + (((double) index)-0.5)/((double) n)*(((double) index)-0.5)/((double) n) );
  else{
    approx = 1.0 / ( 1.0 + (((double) index)-0.5)/((double) n)* (((double) index)-0.5)/((double) n) );
    result = approx + recur_piapprox(index-1, n);
  }
  return result;
}


/*******************************************************/
/* Calculate the overhead of function calls.           */
/*                                                     */
/* Compute an approximate value of Pi - first by doing */
/* the calculation directly inside a for loop, and     */
/* then by using a function to do the calculation. The */
/* difference in time and power usage between the two  */
/* methods is the overhead added by the function calls.*/
/*******************************************************/

int function_calls(unsigned int N){

  /*
   * If size (N) is < 10^6 the error on pi is too high in normal mode and causes misleading output.
   * The computational pattern is correct, but a result with high error may cause users to believe something
   * is wrong. So, set the minimum to 10^6.
   */
  if (N < 1000000){
    N = 1000000;
  }

  double pi, exactpi;
  int i;

  struct timespec start, end;

  pi = 0.0;

  /* warm-up */
  # pragma omp parallel private(i) firstprivate(pi)
  for (i=1; i<=1000; i++){
    pi = pi + 1.0/( 1.0 + (((double) i)-0.5)/((double) 1000) * (((double) i)-0.5)/((double) 1000) );
  }

  clock_gettime(CLOCK, &start);

  /* loop to calculate approximate vaule of pi directly */
  # pragma omp parallel private(i) firstprivate(pi)
  for (i=N; i>=1; i--){
    pi = pi + 1.0/( 1.0 + (((double) i)-0.5)/((double) N)*(((double) i)-0.5)/((double) N) );
  }

  clock_gettime(CLOCK, &end);
  elapsed_time_hr(start, end, "No Function Calls");

  pi = pi * 4.0/((double) N);
  exactpi = 4.0*atan(1.0);

  /* verify result */
  /* printf("pi = %f, %% error = %f\n", pi, fabs(100.0*(pi-exactpi)/exactpi)); */

  /* allow things to settle down */
  sleep(5);
 
  pi=0.0;

  /* warm-up */
  # pragma omp parallel private(i) firstprivate(pi)
  for (i=1; i<=1000; i++){
    pi = pi + piapprox(i, 1000);
  }

  //printf("\n*** Computing approximation to pi with function calls.\n");

  clock_gettime(CLOCK, &start);

  /* loop to calculate approximate vaule of pi using a function */
  # pragma omp parallel private(i) firstprivate(pi)
  for (i=N; i>=1; i--){
    pi = pi + piapprox(i, N);
  }

  clock_gettime(CLOCK, &end);
  elapsed_time_hr(start, end, "Function Calls");

  pi = pi * 4.0/((double) N);
  exactpi = 4.0*atan(1.0);

  /* verify result */
  printf("pi = %f, %% error = %f\n", pi, fabs(100.0*(pi-exactpi)/exactpi));

  return 0;

}

int function_calls_recursive(unsigned int N){

  double pi, exactpi;
  int i = 0, j = 0, rep = N;

  struct timespec start, end;

  if(N > 10000) {
    rep = N/10000;
    N = 10000;
  }

  pi = 0.0;

  /* warm-up */
  # pragma omp parallel private(i) firstprivate(pi)
  for (i=1; i<=1000; i++){
    pi = pi + 1.0/( 1.0 + (((double) i)-0.5)/((double) 1000)*(((double) i)-0.5)/((double) 1000) );
  }

  //printf("\n*** Computing approximation to pi without function calls.\n");

  clock_gettime(CLOCK, &start);
  
  # pragma omp parallel private(i,j,pi)
  for(j=0;j<rep;j++){
    pi = 0.0;
    /* loop to calculate approximate vaule of pi directly */
    for (i=N; i>=1; i--){
      pi = pi + 1.0/( 1.0 + (((double) i)-0.5)/((double) N)*(((double) i)-0.5)/((double) N) );
    }
  }

  clock_gettime(CLOCK, &end);
  elapsed_time_hr(start, end, "No Function Calls");

  pi = pi * 4.0/((double) N);
  exactpi = 4.0*atan(1.0);

  /* verify result */
  /* printf("pi = %f, %% error = %f\n", pi, fabs(100.0*(pi-exactpi)/exactpi)); */

  /* allow things to settle down */
  sleep(5);
 
  pi=0.0;

  /* warm-up */
  # pragma omp parallel private(i) firstprivate(pi)
  for (i=1; i<=1000; i++){
    pi = pi + piapprox(i, 1000);
  }

  //  printf("\n*** Computing approximation to pi with function calls.\n");

  clock_gettime(CLOCK, &start);

  # pragma omp parallel private(i,j,pi)
  for(j=0;j<rep;j++){
    pi = 0.0;
    /* loop to calculate approximate vaule of pi using a function */
    for (i=N; i>=1; i--){
      pi = pi + piapprox(i, N);
    }
  }
  clock_gettime(CLOCK, &end);
  elapsed_time_hr(start, end, "Function Calls");

  pi = pi * 4.0/((double) N);
  exactpi = 4.0*atan(1.0);

  /* verify result */
  /* printf("pi = %f, %% error = %f\n", pi, fabs(100.0*(pi-exactpi)/exactpi)); */

  /* allow things to settle down */
  sleep(5);
 
  /* warm-up */
  i = 1000;

  pi = recur_piapprox(i, N);

  //  printf("\n*** Computing approximation to pi with recursive function.\n");

  clock_gettime(CLOCK, &start);

  /* loop to calculate approximate vaule of pi using a function */

  i = N;
  # pragma omp parallel private(j,pi)
  for(j=0;j<rep;j++){
    pi = 0.0;
    pi = recur_piapprox(i, N);
  }

  clock_gettime(CLOCK, &end);
  elapsed_time_hr(start, end, "Recursive Function Calls");

  pi = pi * 4.0/((double) N);
  exactpi = 4.0*atan(1.0);

  /* verify result */
  /* printf("pi = %f, %% error = %f\n", pi, fabs(100.0*(pi-exactpi)/exactpi)); */

  return 0;
}
