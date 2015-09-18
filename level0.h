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

void bench_level0(char *, unsigned int, unsigned int, unsigned long, char *, char *);

/* Basic op */
int int_basic_op(char *, unsigned long);
int float_basic_op(char *, unsigned long);
int double_basic_op(char *, unsigned long);
int long_basic_op(char *, unsigned long);

/* Memory */
int mem_calloc(unsigned int);
int mem_read_ram(unsigned long);
int mem_write_contig(unsigned int);
int mem_write_strided(unsigned int, unsigned int);
int mem_write_random(unsigned int);
int mem_read_contig(unsigned int);
int mem_read_strided(unsigned int, unsigned int);
int mem_read_random(unsigned int);

/* Function calls */
int function_calls(unsigned int);
int function_calls_recursive(unsigned int);

/* IO operations */
int mk_rm_dir(unsigned int);
int file_write(unsigned int);
int file_write_random(unsigned int);
int file_read(unsigned int);
#ifndef __MACH__
int file_read_direct(unsigned int);
#endif
int file_read_random(unsigned int);
#ifndef __MACH__
int file_read_random_direct(unsigned int);
#endif

/* Branches/jumps */
int all_true(unsigned long);
int all_false(unsigned long);
int true_false(unsigned long);
int t2_f2(unsigned long);
int t2_f2(unsigned long);
int t2_f2(unsigned long);
int t_f_random(unsigned long);
int switch_jump(unsigned long);
