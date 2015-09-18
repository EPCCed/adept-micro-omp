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
#include <time.h>
#include <sys/time.h>
#include <signal.h>

#include "utils.h"

#ifdef __MACH__

void clock_gettime (void* clk, struct timespec *ts){
	clock_serv_t cclock;
	mach_timespec_t mts;
	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	ts->tv_sec = mts.tv_sec;
	ts->tv_nsec = mts.tv_nsec;
}

#endif


double elapsed_time_hr(struct timespec t1, struct timespec t2, char * title){

  struct timespec elapsed;
  sub_time_hr(&elapsed, &t1, &t2);
  double elapsed_duration = 0;
  double elapsed_start = 0;
  double elapsed_end = 0;

  /* This could potentially lead to loss of precision dependant on the rounding in conversion to double */
  elapsed_duration = elapsed.tv_sec + ((double)elapsed.tv_nsec/1000000000);
  elapsed_start = t1.tv_sec + ((double)t1.tv_nsec/1000000000);
  elapsed_end = t2.tv_sec + ((double)t2.tv_nsec/1000000000);
  

  printf("\n--- %s\n", title);
  printf("--- Timings ------------------------------------------------------------------------\n");
  printf("|\n");
  printf("| Start: %.9lf   ", elapsed_start);
  printf("End: %.9lf   ", elapsed_end);
  printf("Duration: %.9lf s\n", elapsed_duration);
  printf("|\n");
  printf("------------------------------------------------------------------------------------\n");

  return 1.0; // Compatibility
}

void loop_timer(unsigned long limit){

  struct timespec t1, t2;
  int index;

  clock_gettime(CLOCK, &t1);
  for(index=0; index<limit; index++) {
    /* __asm__ ("nop"); */
  }
  clock_gettime(CLOCK, &t2);


  elapsed_time_hr(t1, t2, "Loop on its own");
}

void loop_timer_nop(unsigned long limit){

  struct timespec t1, t2;
  int index;

  clock_gettime(CLOCK, &t1);
  for(index=0; index<limit; index++) {
    __asm__ ("nop");
  }
  clock_gettime(CLOCK, &t2);


  elapsed_time_hr(t1, t2, "Loop on its own with nop");
}

void warmup_loop(unsigned long limit){

  int index;

  for(index=0; index<limit; index++) {
    __asm__ ("nop");
  }

}

void interrupt_handler(int signum){
  stop = 1;
}

void discrete_elapsed_hr(struct timespec* oh_array, struct timespec* res_array, unsigned int* iter, char* title){

  /* This computes the overhead from the array you give it AND then removes this from the results array */


  double tminus = 0;
  double tcum = 0;
  int count = 0;
  int retval = 0;
  int i = 0;

  struct timespec overhead;
  struct timespec result;
  double cum_overhead = 0;
  int overhead_status = 0;


  /* The overhead array is always 1000 values long */
  for(i=0;i<1000;i++){
    overhead_status = sub_time_hr(&overhead, &oh_array[2*i], &oh_array[2*i+1]);
    if (overhead_status == 1){
      printf("Error computing overhead for %s\n", title);
    }
    cum_overhead += (overhead.tv_sec+((double)overhead.tv_nsec/1000000000));
  }

  double mean_oh;
  if (cum_overhead == 0){
    mean_oh = 0;
  }
  else{
    mean_oh = cum_overhead/1000;
  }

  /* Accumulate the difference between the results values and remove mean overhead */
  for (i=0;i<*iter;i++){
    retval = sub_time_hr(&result, &res_array[2*i], &res_array[2*i+1]);
    if (retval!=1){
      tminus = (result.tv_sec+((double)result.tv_nsec/1000000000)) - mean_oh;
      tcum += tminus;
      count++;
    }
  }
  double cumulative = tcum/count;

  printf("\n--- %s\n", title);
  printf("--- Timings ------------------------------------------------------------------------\n");
  printf("|\n");
  printf("| Iterations %d   ", *iter);
  printf("Mean time(adjusted) %.9lf s   ", cumulative);
  printf("Mean overhead %.9lf s\n", mean_oh);
  printf("|\n");
  printf("------------------------------------------------------------------------------------\n");

}

int sub_time_hr(struct timespec* result, struct timespec* start, struct timespec* end)
{

  if ((end->tv_nsec-start->tv_nsec)<0) {
    result->tv_sec = end->tv_sec-start->tv_sec-1;
    result->tv_nsec = 1000000000+end->tv_nsec-start->tv_nsec;
  } else {
    result->tv_sec = end->tv_sec-start->tv_sec;
    result->tv_nsec = end->tv_nsec-start->tv_nsec;
  }

  return end->tv_sec < start->tv_sec;
}
