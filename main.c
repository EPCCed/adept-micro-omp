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

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <limits.h>
#include <sys/utsname.h>

#include "level0.h"

void usage();
void info();

int main(int argc, char **argv){
  
  int c;
  
  char *bench = "basic_op";
  unsigned long rep = ULONG_MAX;
  unsigned int size = 200;
  unsigned int stride = 64;
  char *op  = "+";
  char *dt = "int";
  
  static struct option option_list[] =
    { {"bench", required_argument, NULL, 'b'},
      {"size", required_argument, NULL, 's'},
      {"stride", required_argument, NULL, 't'},
      {"reps", required_argument, NULL, 'r'},
      {"op", required_argument, NULL, 'o'},
      {"dtype", required_argument, NULL, 'd'},
      {"info", no_argument, NULL, 'i'},
      {"help", no_argument, NULL, 'h'},
      {0, 0, 0, 0}
    };

  while((c = getopt_long(argc, argv, "b:s:t:r:o:d:ih", option_list, NULL)) != -1){
    switch(c){
    case 'b':
      bench = optarg;
      printf("Benchmark is %s.\n", bench);
      break;
    case 's':
      size = atoi(optarg);
      printf("Size is %d.\n", size);
      break;
    case 't':
      stride = atoi(optarg);
      printf("Stride is %d KB.\n", stride);
      break;
    case 'r':
      rep = atol(optarg);
      printf("Number of repetitions %lu.\n", rep);
      break;
    case 'o':
      op = optarg;
      printf("Operation %s\n", op);
      break;
    case 'd':
      dt = optarg;
      printf("Data type is %s\n", dt);
      break;
    case 'i':
      info();
      return 0;
    case 'h':
      usage();
      return 0;
    default:
      printf("Undefined.\n");
      return 0;
    }
  }
    
  bench_level0(bench, size, stride, rep, op, dt);
  
  return 0;
  
}


void usage(){
  printf("Usage for OpenMP MICRO benchmarks:\n\n");
  printf("\t -b, --bench NAME \t name of the benchmark - possible values are basic_op, memory, function, io and branch.\n");
  printf("\t -s, --size N \t\t number of elements/files/directories. Default is 200.\n");
  printf("\t\t\t\t  --> for the function benchmark, this value should be set to at least 100 million.\n");
  printf("\t\t\t\t  --> for the memory benchmark, this value should be the amount of memory to allocate/use in MBytes.\n");
  printf("\t -t, --stride N \t optional stride value (in KB) for memory benchmarks write_strided and read_strided. Default is 64KB.\n");
  printf("\t -r, --reps N \t\t number of repetitions. Default value is ULONG_MAX.\n");
  printf("\t -o, --op TYPE \t\t TYPE of operation.\n");
  printf("\t\t\t\t --> for basic_op benchmark: \"+\", \"-\", \"*\" and \"/\". Default is \"+\".\n");
  printf("\t\t\t\t --> for memory   benchmark: \"calloc\", \"read_ram\", \"write_contig\", \"write_strided\", \"write_random\",\n");
  printf("\t\t\t\t \"read_contig\", \"read_strided\", \"read_random\".\n");
  printf("\t\t\t\t --> for IO benchmark: \"mk_rm_dir\", \"file_write\", \"file_read\", \"file_write_random\", \"file_read_random\", \"file_read_direct\", \"file_read_random_direct\".\n");
  printf("\t\t\t\t --> for function benchmark: \"normal\", \"recursive\".\n");  
  printf("\t\t\t\t --> for branch benchmark: \"switch\", \"all_true\", \"all_false\", \"true_false\", \"t2_f2\", \"t4_f4\", \"t8_f8\", \"t_f_random\".\n");
  printf("\t -d, --dtype DATATYPE \t DATATYPE to be used - possible values are int, long, float, double. Default is int.\n");
  printf("\t -i, --info \t\t Print out system information such as current CPU frequency, core counts, cache size, plus datatype sizes.\n");
  printf("\t -h, --help \t\t Displays this help.\n");
  printf("\n\n");
}


void info(){  
#ifdef __linux__
  printf("\n***************************************\n");
  system("lscpu");
  printf("***************************************\n");
#endif
  printf("\n***************************************\n");
  printf("Datatype sizes on this platform are\n");
  printf("\n");
  printf("Size of int: \t\t%lu bytes\n", sizeof(int));
  printf("Size of long: \t\t%lu bytes\n", sizeof(long));
  printf("Size of float: \t\t%lu bytes\n", sizeof(float));
  printf("Size of double: \t%lu bytes\n", sizeof(double));
  printf("***************************************\n");
  printf("\n\n");
}

