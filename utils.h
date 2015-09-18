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

#include <signal.h>
volatile sig_atomic_t stop;

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>

#define CLOCK 0

void clock_gettime (void*, struct timespec*);

#else

#ifdef CLOCK_MONOTONIC_RAW
#define CLOCK CLOCK_MONOTONIC_RAW
#else
#define CLOCK CLOCK_MONOTONIC
#endif

#endif

double elapsed_time_hr(struct timespec, struct timespec, char *);
void loop_timer(unsigned long);
void loop_timer_nop(unsigned long);
void warmup_loop(unsigned long);
void interrupt_handler(int);
void discrete_elapsed_hr(struct timespec*, struct timespec*, unsigned int*, char*);
int sub_time_hr(struct timespec*, struct timespec*, struct timespec*);
