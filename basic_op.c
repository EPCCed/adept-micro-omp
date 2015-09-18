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
#include <sys/time.h>
#include <time.h>
#include <omp.h>

#include "level0.h"
#include "utils.h"


int int_basic_op(char *opType, unsigned long reps){
  
  int i;
  
#ifdef ONEVOL
  unsigned int a, b, c;
  volatile unsigned int d;
#undef ONEVOL
#else
#ifdef ALLVOL
  volatile unsigned int a, b, c, d;
#undef ALLVOL
#else
  unsigned int a, b, c, d;
#endif
#endif
  
  char choice = (char)opType[0];
  
  srand((unsigned int)time(NULL));
  
  a = rand();
  b = rand();
  c = rand();
  d = rand();
  
  struct timespec start, end;
  
  /* warm-up loop with nop */
  
  warmup_loop(reps);
  
  /* measure loop on its own with nop */
  loop_timer_nop(reps);
  
  /* measure loop on its own */
  loop_timer(reps);
  
  switch(choice){
    case '+': {
      
      /* warm-up loop */
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<100; i++) c = a+b;
      
      /* main loops */
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++) c = a+d;
      clock_gettime(CLOCK, &end);
      elapsed_time_hr(start, end, "Integer Addition - 1 op HR");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a+d;
        b = d+c;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Integer Addition - 2 ops");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a+d;
        b = d+c;
        a = b+d;
        c = d+a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Integer Addition - 4 ops");
      
      reps = reps * 4 / 5;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a+d;
        b = d+c;
        a = b+d;
        c = d+a;
        b = c+d;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Integer Addition - 5 ops");
      
      reps = reps * 5 / 8;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a+d;
        b = d+c;
        a = b+d;
        c = d+a;
        b = c+d;
        c = d+b;
        a = d+c;
        b = a+d;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Integer Addition - 8 ops");
      
      reps = reps * 4 / 5;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a+d;
        b = d+c;
        a = b+d;
        c = d+a;
        b = c+d;
        c = d+b;
        a = d+c;
        b = a+d;
        c = b+d;
        b = c+d;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Integer Addition - 10 ops");
      
      break;
    }
    case '-': {
      /* warm-up loop */
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<100; i++) c = a-b;
      
      /* main loop */
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++) c = a-b;
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Integer Subtraction - 1 op");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a-b;
        a = b-c;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Integer Subtraction - 2 ops");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a-b;
        a = b-c;
        b = a-c;
        c = c-a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Integer Subtraction - 4 ops");
      
      reps = reps * 4 / 5;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a-b;
        a = b-c;
        b = a-c;
        c = c-a;
        b = b-a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Integer Subtraction - 5 ops");
      
      reps = reps * 5 / 8;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a-b;
        a = b-c;
        b = a-c;
        c = c-a;
        b = b-a;
        c = c-b;
        a = a-c;
        b = b-c;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Integer Subtraction - 8 ops");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a-b;
        a = b-c;
        b = a-c;
        c = c-a;
        b = b-a;
        c = c-b;
        a = a-c;
        b = b-c;
        c = b-a;
        b = c-a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Integer Subtraction - 10 ops");
      
      break;
    }
    case '*': {
      /* warm-up loop */
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<100; i++) c = a*b;
      
      /* main loop */
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++) c = a*b;
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Integer Multiplication - 1 op");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a*b;
        a = b*c;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Integer Multiplication - 2 ops");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a*b;
        a = b*c;
        b = a*c;
        c = c*a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Integer Multiplication - 4 ops");
      
      reps = reps * 4 / 5;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a*b;
        a = b*c;
        b = a*c;
        c = c*a;
        b = b*a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Integer Multiplication - 5 ops");
      
      reps = reps * 5 / 8;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a*b;
        a = b*c;
        b = a*c;
        c = c*a;
        b = b*a;
        c = c*b;
        a = a*c;
        b = b*c;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Integer Multiplication - 8 ops");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a*b;
        a = b*c;
        b = a*c;
        c = c*a;
        b = b*a;
        c = c*b;
        a = a*c;
        b = b*c;
        c = b*a;
        b = c*a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Integer Multiplication - 10 ops");
      
      break;
    }
    case '/': {
      
      /* need to avoid re-use of variables to make sure */
      /* we do not get divison by 0.                    */
      volatile int r1,r2,r3,r4,r5,r6,r7,r8,r9,r10;
      
      /* warm-up loop */
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<100; i++) r1 = a/b;
      
      /* main loop */
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++) r1 = a/b;
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Integer Division - 1 op");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        r1 = a/b;
        r2 = b/c;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Integer Division - 2 ops");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        r1 = a/b;
        r2 = b/c;
        r3 = a/c;
        r4 = b/a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Integer Division - 4 ops");
      
      reps = reps * 4 / 5;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        r1 = a/b;
        r2 = b/c;
        r3 = a/c;
        r4 = b/a;
        r5 = c/a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Integer Division - 5 ops");
      
      reps = reps * 5 / 8;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        r1 = a/b;
        r2 = b/c;
        r3 = a/c;
        r4 = b/a;
        r5 = c/a;
        r6 = c/b;
        r7 = a/a;
        r8 = b/b;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Integer Division - 8 ops");
      
      reps = reps * 4 / 5;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        r1 = a/b;
        r2 = b/c;
        r3 = a/c;
        r4 = b/a;
        r5 = c/a;
        r6 = c/b;
        r7 = a/a;
        r8 = b/b;
        r9 = c/c;
        r10 = r10/a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Integer Division - 10 ops");
      
      break;
    }
    default: printf("Only possible operation choices are '+', '-', '*' and '/'.\n");
  }
  
  return 0;
  
}


int float_basic_op(char *opType, unsigned long reps){
  
  int i;
  
#ifdef ONEVOL
  float a, b, c;
  volatile float d;
#undef ONEVOL
#else
#ifdef ALLVOL
  volatile float a, b, c, d;
#undef ALLVOL
#else
  float a, b, c, d;
#endif
#endif
  
  char choice = (char)opType[0];
  
  srand((unsigned int)time(NULL));
  
  a = rand();
  b = rand();
  c = rand();
  d = rand();
  
  struct timespec start, end;
  
  /* warm-up loop with nop */
  warmup_loop(reps);
  
  /* measure loop on its own with nop */
  loop_timer_nop(reps);
  
  /* measure loop on its own */
  loop_timer(reps);
  
  switch(choice){
    case '+': {
      
      /* warm-up loop */
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<100; i++) c = a+b;
      
      /* main loops */
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++) c = a+d;
      clock_gettime(CLOCK, &end);
      elapsed_time_hr(start, end, "Float Addition - 1 op HR");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a+d;
        b = d+c;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Float Addition - 2 ops");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a+d;
        b = d+c;
        a = b+d;
        c = d+a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Float Addition - 4 ops");
      
      reps = reps * 4 / 5;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a+d;
        b = d+c;
        a = b+d;
        c = d+a;
        b = c+d;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Float Addition - 5 ops");
      
      reps = reps * 5 / 8;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a+d;
        b = d+c;
        a = b+d;
        c = d+a;
        b = c+d;
        c = d+b;
        a = d+c;
        b = a+d;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Float Addition - 8 ops");
      
      reps = reps * 4 / 5;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a+d;
        b = d+c;
        a = b+d;
        c = d+a;
        b = c+d;
        c = d+b;
        a = d+c;
        b = a+d;
        c = b+d;
        b = c+d;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Float Addition - 10 ops");
      
      break;
    }
    case '-': {
      /* warm-up loop */
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<100; i++) c = a-b;
      
      /* main loop */
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++) c = a-b;
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Float Subtraction - 1 op");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a-b;
        a = b-c;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Float Subtraction - 2 ops");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a-b;
        a = b-c;
        b = a-c;
        c = c-a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Float Subtraction - 4 ops");
      
      reps = reps * 4 / 5;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a-b;
        a = b-c;
        b = a-c;
        c = c-a;
        b = b-a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Float Subtraction - 5 ops");
      
      reps = reps * 5 / 8;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a-b;
        a = b-c;
        b = a-c;
        c = c-a;
        b = b-a;
        c = c-b;
        a = a-c;
        b = b-c;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Integer Subtraction - 8 ops");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a-b;
        a = b-c;
        b = a-c;
        c = c-a;
        b = b-a;
        c = c-b;
        a = a-c;
        b = b-c;
        c = b-a;
        b = c-a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Float Subtraction - 10 ops");
      
      break;
    }
    case '*': {
      /* warm-up loop */
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<100; i++) c = a*b;
      
      /* main loop */
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++) c = a*b;
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Float Multiplication - 1 op");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a*b;
        a = b*c;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Float Multiplication - 2 ops");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a*b;
        a = b*c;
        b = a*c;
        c = c*a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Float Multiplication - 4 ops");
      
      reps = reps * 4 / 5;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a*b;
        a = b*c;
        b = a*c;
        c = c*a;
        b = b*a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Float Multiplication - 5 ops");
      
      reps = reps * 5 / 8;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a*b;
        a = b*c;
        b = a*c;
        c = c*a;
        b = b*a;
        c = c*b;
        a = a*c;
        b = b*c;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Float Multiplication - 8 ops");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a*b;
        a = b*c;
        b = a*c;
        c = c*a;
        b = b*a;
        c = c*b;
        a = a*c;
        b = b*c;
        c = b*a;
        b = c*a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Float Multiplication - 10 ops");
      
      break;
    }
    case '/': {
      
      /* need to avoid re-use of variables to make sure */
      /* we do not get divison by 0.                    */
      volatile int r1,r2,r3,r4,r5,r6,r7,r8,r9,r10;
      
      /* warm-up loop */
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<100; i++) r1 = a/b;
      
      /* main loop */
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++) r1 = a/b;
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Float Division - 1 op");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        r1 = a/b;
        r2 = b/c;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Float Division - 2 ops");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        r1 = a/b;
        r2 = b/c;
        r3 = a/c;
        r4 = b/a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Float Division - 4 ops");
      
      reps = reps * 4 / 5;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        r1 = a/b;
        r2 = b/c;
        r3 = a/c;
        r4 = b/a;
        r5 = c/a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Float Division - 5 ops");
      
      reps = reps * 5 / 8;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        r1 = a/b;
        r2 = b/c;
        r3 = a/c;
        r4 = b/a;
        r5 = c/a;
        r6 = c/b;
        r7 = a/a;
        r8 = b/b;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Float Division - 8 ops");
      
      reps = reps * 4 / 5;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        r1 = a/b;
        r2 = b/c;
        r3 = a/c;
        r4 = b/a;
        r5 = c/a;
        r6 = c/b;
        r7 = a/a;
        r8 = b/b;
        r9 = c/c;
        r10 = r10/a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Float Division - 10 ops");
      
      break;
    }
    default: printf("Only possible operation choices are '+', '-', '*' and '/'.\n");
  }
  
  return 0;
  
}


int long_basic_op(char *opType, unsigned long reps){
  
  int i;
  
#ifdef ONEVOL
  long a, b, c;
  volatile long d;
#undef ONEVOL
#else
#ifdef ALLVOL
  volatile long a, b, c, d;
#undef ALLVOL
#else
  long a, b, c, d;
#endif
#endif
  
  char choice = (char)opType[0];
  
  srand((unsigned int)time(NULL));
  
  a = rand();
  b = rand();
  c = rand();
  d = rand();
  
  struct timespec start, end;
  
  /* warm-up loop with nop */
  warmup_loop(reps);
  
  /* measure loop on its own with nop */
  loop_timer_nop(reps);
  
  /* measure loop on its own */
  loop_timer(reps);
  
  switch(choice){
    case '+': {
      
      /* warm-up loop */
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<100; i++) c = a+b;
      
      /* main loops */
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++) c = a+d;
      clock_gettime(CLOCK, &end);
      elapsed_time_hr(start, end, "Long Addition - 1 op HR");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a+d;
        b = d+c;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Long Addition - 2 ops");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a+d;
        b = d+c;
        a = b+d;
        c = d+a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Long Addition - 4 ops");
      
      reps = reps * 4 / 5;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a+d;
        b = d+c;
        a = b+d;
        c = d+a;
        b = c+d;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Long Addition - 5 ops");
      
      reps = reps * 5 / 8;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a+d;
        b = d+c;
        a = b+d;
        c = d+a;
        b = c+d;
        c = d+b;
        a = d+c;
        b = a+d;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Long Addition - 8 ops");
      
      reps = reps * 4 / 5;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a+d;
        b = d+c;
        a = b+d;
        c = d+a;
        b = c+d;
        c = d+b;
        a = d+c;
        b = a+d;
        c = b+d;
        b = c+d;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Long Addition - 10 ops");
      
      break;
    }
    case '-': {
      /* warm-up loop */
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<100; i++) c = a-b;
      
      /* main loop */
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++) c = a-b;
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Long Subtraction - 1 op");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a-b;
        a = b-c;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Long Subtraction - 2 ops");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a-b;
        a = b-c;
        b = a-c;
        c = c-a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Long Subtraction - 4 ops");
      
      reps = reps * 4 / 5;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a-b;
        a = b-c;
        b = a-c;
        c = c-a;
        b = b-a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Long Subtraction - 5 ops");
      
      reps = reps * 5 / 8;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a-b;
        a = b-c;
        b = a-c;
        c = c-a;
        b = b-a;
        c = c-b;
        a = a-c;
        b = b-c;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Integer Subtraction - 8 ops");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a-b;
        a = b-c;
        b = a-c;
        c = c-a;
        b = b-a;
        c = c-b;
        a = a-c;
        b = b-c;
        c = b-a;
        b = c-a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Long Subtraction - 10 ops");
      
      break;
    }
    case '*': {
      /* warm-up loop */
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<100; i++) c = a*b;
      
      /* main loop */
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++) c = a*b;
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Long Multiplication - 1 op");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a*b;
        a = b*c;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Long Multiplication - 2 ops");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a*b;
        a = b*c;
        b = a*c;
        c = c*a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Long Multiplication - 4 ops");
      
      reps = reps * 4 / 5;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a*b;
        a = b*c;
        b = a*c;
        c = c*a;
        b = b*a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Long Multiplication - 5 ops");
      
      reps = reps * 5 / 8;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a*b;
        a = b*c;
        b = a*c;
        c = c*a;
        b = b*a;
        c = c*b;
        a = a*c;
        b = b*c;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Long Multiplication - 8 ops");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a*b;
        a = b*c;
        b = a*c;
        c = c*a;
        b = b*a;
        c = c*b;
        a = a*c;
        b = b*c;
        c = b*a;
        b = c*a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Long Multiplication - 10 ops");
      
      break;
    }
    case '/': {
      
      /* need to avoid re-use of variables to make sure */
      /* we do not get divison by 0.                    */
      volatile int r1,r2,r3,r4,r5,r6,r7,r8,r9,r10;
      
      /* warm-up loop */
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<100; i++) r1 = a/b;
      
      /* main loop */
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++) r1 = a/b;
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Long Division - 1 op");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        r1 = a/b;
        r2 = b/c;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Long Division - 2 ops");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        r1 = a/b;
        r2 = b/c;
        r3 = a/c;
        r4 = b/a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Long Division - 4 ops");
      
      reps = reps * 4 / 5;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        r1 = a/b;
        r2 = b/c;
        r3 = a/c;
        r4 = b/a;
        r5 = c/a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Long Division - 5 ops");
      
      reps = reps * 5 / 8;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        r1 = a/b;
        r2 = b/c;
        r3 = a/c;
        r4 = b/a;
        r5 = c/a;
        r6 = c/b;
        r7 = a/a;
        r8 = b/b;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Long Division - 8 ops");
      
      reps = reps * 4 / 5;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        r1 = a/b;
        r2 = b/c;
        r3 = a/c;
        r4 = b/a;
        r5 = c/a;
        r6 = c/b;
        r7 = a/a;
        r8 = b/b;
        r9 = c/c;
        r10 = r10/a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Long Division - 10 ops");
      
      break;
    }
    default: printf("Only possible operation choices are '+', '-', '*' and '/'.\n");
  }
  
  return 0;
  
}

int double_basic_op(char *opType, unsigned long reps){
  
  int i;
  
#ifdef ONEVOL
  double a, b, c;
  volatile double d;
#undef ONEVOL
#else
#ifdef ALLVOL
  volatile double a, b, c, d;
#undef ALLVOL
#else
  double a, b, c, d;
#endif
#endif
  
  char choice = (char)opType[0];
  
  srand((unsigned int)time(NULL));
  
  a = rand();
  b = rand();
  c = rand();
  d = rand();
  
  struct timespec start, end;
  
  /* warm-up loop with nop */
  warmup_loop(reps);
  
  /* measure loop on its own with nop */
  loop_timer_nop(reps);
  
  /* measure loop on its own */
  loop_timer(reps);
  
  switch(choice){
    case '+': {
      
      /* warm-up loop */
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<100; i++) c = a+b;
      
      /* main loops */
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++) c = a+d;
      clock_gettime(CLOCK, &end);
      elapsed_time_hr(start, end, "Double Addition - 1 op HR");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a+d;
        b = d+c;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Double Addition - 2 ops");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a+d;
        b = d+c;
        a = b+d;
        c = d+a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Double Addition - 4 ops");
      
      reps = reps * 4 / 5;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a+d;
        b = d+c;
        a = b+d;
        c = d+a;
        b = c+d;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Double Addition - 5 ops");
      
      reps = reps * 5 / 8;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a+d;
        b = d+c;
        a = b+d;
        c = d+a;
        b = c+d;
        c = d+b;
        a = d+c;
        b = a+d;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Double Addition - 8 ops");
      
      reps = reps * 4 / 5;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a+d;
        b = d+c;
        a = b+d;
        c = d+a;
        b = c+d;
        c = d+b;
        a = d+c;
        b = a+d;
        c = b+d;
        b = c+d;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Double Addition - 10 ops");
      
      break;
    }
    case '-': {
      /* warm-up loop */
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<100; i++) c = a-b;
      
      /* main loop */
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++) c = a-b;
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Double Subtraction - 1 op");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a-b;
        a = b-c;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Double Subtraction - 2 ops");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a-b;
        a = b-c;
        b = a-c;
        c = c-a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Double Subtraction - 4 ops");
      
      reps = reps * 4 / 5;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a-b;
        a = b-c;
        b = a-c;
        c = c-a;
        b = b-a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Double Subtraction - 5 ops");
      
      reps = reps * 5 / 8;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a-b;
        a = b-c;
        b = a-c;
        c = c-a;
        b = b-a;
        c = c-b;
        a = a-c;
        b = b-c;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Integer Subtraction - 8 ops");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a-b;
        a = b-c;
        b = a-c;
        c = c-a;
        b = b-a;
        c = c-b;
        a = a-c;
        b = b-c;
        c = b-a;
        b = c-a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Double Subtraction - 10 ops");
      
      break;
    }
    case '*': {
      /* warm-up loop */
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<100; i++) c = a*b;
      
      /* main loop */
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++) c = a*b;
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Double Multiplication - 1 op");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a*b;
        a = b*c;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Double Multiplication - 2 ops");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a*b;
        a = b*c;
        b = a*c;
        c = c*a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Double Multiplication - 4 ops");
      
      reps = reps * 4 / 5;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a*b;
        a = b*c;
        b = a*c;
        c = c*a;
        b = b*a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Double Multiplication - 5 ops");
      
      reps = reps * 5 / 8;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a*b;
        a = b*c;
        b = a*c;
        c = c*a;
        b = b*a;
        c = c*b;
        a = a*c;
        b = b*c;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Double Multiplication - 8 ops");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        c = a*b;
        a = b*c;
        b = a*c;
        c = c*a;
        b = b*a;
        c = c*b;
        a = a*c;
        b = b*c;
        c = b*a;
        b = c*a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Double Multiplication - 10 ops");
      
      break;
    }
    case '/': {
      
      /* need to avoid re-use of variables to make sure */
      /* we do not get divison by 0.                    */
      volatile int r1,r2,r3,r4,r5,r6,r7,r8,r9,r10;
      
      /* warm-up loop */
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<100; i++) r1 = a/b;
      
      /* main loop */
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++) r1 = a/b;
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Double Division - 1 op");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        r1 = a/b;
        r2 = b/c;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Double Division - 2 ops");
      
      reps = reps / 2;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        r1 = a/b;
        r2 = b/c;
        r3 = a/c;
        r4 = b/a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Double Division - 4 ops");
      
      reps = reps * 4 / 5;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        r1 = a/b;
        r2 = b/c;
        r3 = a/c;
        r4 = b/a;
        r5 = c/a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Double Division - 5 ops");
      
      reps = reps * 5 / 8;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        r1 = a/b;
        r2 = b/c;
        r3 = a/c;
        r4 = b/a;
        r5 = c/a;
        r6 = c/b;
        r7 = a/a;
        r8 = b/b;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Double Division - 8 ops");
      
      reps = reps * 4 / 5;
      
      clock_gettime(CLOCK, &start);
# pragma omp parallel private(i) firstprivate(a,b,c,d)
      for(i=0; i<reps; i++){
        r1 = a/b;
        r2 = b/c;
        r3 = a/c;
        r4 = b/a;
        r5 = c/a;
        r6 = c/b;
        r7 = a/a;
        r8 = b/b;
        r9 = c/c;
        r10 = r10/a;
      }
      clock_gettime(CLOCK, &end);
      
      elapsed_time_hr(start, end, "Double Division - 10 ops");
      
      break;
    }
    default: printf("Only possible operation choices are '+', '-', '*' and '/'.\n");
  }
  
  return 0;
  
}


