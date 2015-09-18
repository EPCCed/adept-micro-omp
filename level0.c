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
#include <string.h>
#include <limits.h>

#include "level0.h"

/*
 *
 * Level 0 benchmark driver - calls appropriate function
 * based on command line arguments.
 *
 */
void bench_level0(char *b, unsigned int s, unsigned int t, unsigned long r, char *o, char *dt){

  /* basic operations */
  if(strcmp(b, "basic_op") == 0){

    if(strcmp(dt, "int") == 0)
      int_basic_op(o,r);

    else if(strcmp(dt, "float") == 0)
      float_basic_op(o,r);

    else if(strcmp(dt, "long") == 0)
      long_basic_op(o,r);

    else if(strcmp(dt, "double") == 0)
      double_basic_op(o,r);

    else fprintf(stderr, "ERROR: check you are using a valid data type...\n");
  }

  /* memory reads and writes */
  else if(strcmp(b, "memory") == 0){

    if(strcmp(o, "calloc") == 0)
      mem_calloc(s);

    else if(strcmp(o, "read_ram") == 0)
      mem_read_ram(r);

    else if(strcmp(o, "write_contig") == 0)
      mem_write_contig(s);

    else if(strcmp(o, "write_strided") == 0)
      mem_write_strided(s, t);

    else if(strcmp(o, "write_random") == 0)
      mem_write_random(s);

    else if(strcmp(o, "read_contig") == 0)
      mem_read_contig(s);

    else if(strcmp(o, "read_strided") == 0)
      mem_read_strided(s, t);

    else if(strcmp(o, "read_random") == 0)
      mem_read_random(s);

    else fprintf(stderr, "ERROR: check you are using a valid operation type...\n");
  }

  /* function calls*/
  else if(strcmp(b, "function") == 0){

    if(strcmp(o, "normal") == 0)
      function_calls(s);

    else if(strcmp(o, "recursive") == 0)
      function_calls_recursive(s);

    else fprintf(stderr, "ERROR: check you are using a valid operation type...\n");

  }

  /* IO operations */
  else if(strcmp(b, "io") == 0){

    if(strcmp(o, "mk_rm_dir") == 0)
      mk_rm_dir(s);

    else if(strcmp(o, "file_write") == 0)
      file_write(s);

    else if(strcmp(o, "file_read") == 0)
      file_read(s);

    else if(strcmp(o, "file_write_random") == 0)
      file_write_random(s);

    else if(strcmp(o, "file_read_random") == 0)
      file_read_random(s);

#ifndef __MACH__
    else if(strcmp(o, "file_read_direct") == 0)
      file_read_direct(s);

    else if(strcmp(o, "file_read_random_direct") == 0)
      file_read_random_direct(s);
#endif

    else fprintf(stderr, "ERROR: check you are using a valid operation type...\n");
  }

  /* Branches/jumps */
  else if (strcmp(b, "branch") == 0){

    if(strcmp(o, "all_true") == 0)
      all_true(r);

    else if(strcmp(o, "all_false") == 0)
      all_false(r);

    else if(strcmp(o, "true_false") == 0)
      true_false(r);

    else if(strcmp(o, "t2_f2") == 0)
      t2_f2(r);

    else if(strcmp(o, "t4_f4") == 0)
      t2_f2(r);

    else if(strcmp(o, "t8_f8") == 0)
      t2_f2(r);

    else if(strcmp(o, "t_f_random") == 0)
      t_f_random(r);

   else if(strcmp(o, "switch") == 0)
      switch_jump(r);

    else fprintf(stderr, "ERROR: check you are using a valid operation type...\n");

  }

  /* everything that has not been implemented */
  else
    printf("Not implemented yet.\n");

}

