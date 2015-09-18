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

/* need this to get O_DIRECT definition */
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <omp.h>

#include "utils.h"

int mk_rm_dir(unsigned int N){

    char d[32]; 
    int i = 0, nanosleepInterval = 0; 
    
    struct timespec start, end;
    struct timespec timeToSleep, timeRemaining;
    timeToSleep.tv_sec  = 0;
    timeToSleep.tv_nsec = 250000000L;    // 0.25 seconds
    
    /* if N has been left at 2 million by mistake, reduce */ 
    /* to 100k to avoid creating too many directories.    */
    if(N == 2000000) N = 100000;
    
    /* warm-up */
    # pragma omp parallel private(i, d)
    for(i=0; i<100;i++){
	sprintf(d, "warmupdir_%d_%d", omp_get_thread_num(), i);
	mkdir(d,777);
    }

    clock_gettime(CLOCK, &start);

    /* create directories */
    # pragma omp parallel private(i, d)
    for(i=0; i<N;i++){
	sprintf(d, "testdir_%d_%d", omp_get_thread_num(), i);
	mkdir(d,777);
	if ( i % 10000 == 0 ) {
	    nanosleep(&timeToSleep, &timeRemaining);
	    nanosleepInterval++;
	}
    }

    clock_gettime(CLOCK, &end);
    elapsed_time_hr(start, end, "mkdir");
    printf("Slept for :%6.3f, ms.\n", 1000*nanosleepInterval*0.25);
    nanosleepInterval = 0;
    
    sleep(5);

    /* warm-up */
    # pragma omp parallel private(i, d)
    for(i=0; i<100;i++){
	sprintf(d, "warmupdir_%d_%d", omp_get_thread_num(), i);
	rmdir(d);
    }
    clock_gettime(CLOCK, &start);

    /* remove previously created directories */
    # pragma omp parallel private(i, d)
    for(i=0; i<N;i++){
	sprintf(d, "testdir_%d_%d", omp_get_thread_num(), i);
	rmdir(d);
	if ( i % 10000 == 0 ) {
	    nanosleep(&timeToSleep, &timeRemaining);
	    nanosleepInterval++;
	}
    }
    
    clock_gettime(CLOCK, &end);
    elapsed_time_hr(start, end, "rmdir");
    printf("Slept for :%6.3f, ms.\n", 1000*nanosleepInterval*0.25);
    
    fflush(stdout);
    return 0;
}

int file_write(unsigned int N)
{
    struct timespec start, end;
    char name[100];
    int size = 1;
    int i, j;
    unsigned char **data;
    int warmupsize;
    char titlebuffer[500];
    int fd;
    int reps;

    int nthreads;
    # pragma omp parallel
    if (omp_get_thread_num() == 0) nthreads = omp_get_num_threads();

    /* allocate and initialise data */
    data = malloc(nthreads * sizeof(unsigned char*));
    if (!data) {
	fprintf(stderr, "ERROR: out of memory in file_write\n");
	return 1;
    }
    for (i = 0; i < nthreads; i++) {
	data[i] = malloc(N);
	if (!data[i]) {
	    fprintf(stderr, "ERROR: out of memory in file_write\n");
	    return 1;
	}
    }

    fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
	fprintf(stderr, "ERROR: unable to open /dev/urandom in file_write\n");
	return 1;
    }
    for (i = 0; i < nthreads; i++) {
	read(fd, data[i], N);
    }
    close(fd);

    warmupsize = 10000;
    if (warmupsize > N) warmupsize = N;

    /* do warm-up */
    # pragma omp parallel private(i, fd, name)
    for (i = 0; i < 100; i++) {
	sprintf(name, "warmup_%d_%d", omp_get_thread_num(), i);
	fd = open(name, O_CREAT|O_WRONLY|O_TRUNC, 0644);
	if (fd < 0) {
	    fprintf(stderr, "ERROR: unable to open warmup file for writing in file_write\n");
	}
	else {
	    write(fd, data[omp_get_thread_num()], warmupsize);
	    close(fd);
	}
    }

    /* don't overload the system by doing too many tiny files */
    while (N >= 10000) {
	N = N / 2;
	size = size * 2;
    }

    while (N >= 1) {
	sprintf(titlebuffer, "file_write: %d files of %d bytes", N, size);

	/* do actual write test */
	clock_gettime(CLOCK, &start);

	reps = 128 / N;
	if (reps == 0) reps = 1;

        # pragma omp parallel private(i, j, name, fd)
	for (j = 0; j < reps; j++) {
	    for (i = 0; i < N; i++) {
		sprintf(name, "testfile_%d_%d", omp_get_thread_num(), i);
		
		fd = open(name, O_CREAT|O_WRONLY|O_TRUNC, 0644);
		if (fd < 0) {
		    fprintf(stderr, "ERROR: unable to open test file for writing in file_write\n");
		}
		else {
		    write(fd, data[omp_get_thread_num()], size);
		    fsync(fd);
		    close(fd);
		}
	    }
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, titlebuffer);

	/* remove files just created */
        # pragma omp parallel private(i, name)
	for (i = 0; i < N; i++) {
	    sprintf(name, "testfile_%d_%d", omp_get_thread_num(), i);
	    unlink(name);
	}

	/* halve number of files but double their size */
	N = N / 2;
	size = size * 2;
    }

    /* remove warm-up files */
    # pragma omp parallel private(i, name)
    for (i = 0; i < 100; i++) {
	sprintf(name, "warmup_%d_%d", omp_get_thread_num(), i);
	unlink(name);
    }

    for (i = 0; i < nthreads; i++) {
	free(data[i]);
    }
    free(data);
    fflush(stdout);
    return 0;
}

int file_write_random(unsigned int N)
{
    struct timespec start, end;
    char name[100];
    int size = 1;
    int i, j;
    unsigned char **data;
    int warmupsize;
    char titlebuffer[500];
    int fd;
    int block;
    int reps;

    int nthreads;
    # pragma omp parallel
    if (omp_get_thread_num() == 0) nthreads = omp_get_num_threads();

    /* allocate and initialise data */
    data = malloc(nthreads * sizeof(unsigned char*));
    for (i = 0; i < nthreads; i++) {
	data[i] = malloc(N);
    }

    fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
	fprintf(stderr, "ERROR: unable to open /dev/urandom in file_write_random\n");
	return 1;
    }
    for (i = 0; i < nthreads; i++) {
	read(fd, data[i], N);
    }
    close(fd);

    /* now create the test files, one per thread */
    # pragma omp parallel private(name, fd)
    {
	sprintf(name, "testfile_%d_1", omp_get_thread_num());
	fd = open(name, O_CREAT|O_WRONLY|O_TRUNC, 0644);
	if (fd < 0) {
	    fprintf(stderr, "ERROR: unable to open test file for writing in file_write_random\n");
	}
	else {
	    write(fd, data[omp_get_thread_num()], N);
	    fsync(fd);
	    close(fd);
	}
    }

    /* don't overload the system by doing too many tiny blocks */
    while (N >= 10000) {
	N = N / 2;
	size = size * 2;
    }

    while (N >= 1) {

	sprintf(titlebuffer, "file_write_random: %d blocks of %d bytes", N, size);

	clock_gettime(CLOCK, &start);

	reps = 128 / N;
	if (reps == 0) reps = 1;

        # pragma omp parallel private(i, j, fd, name, block)
	for (j = 0; j < reps; j++) {
	    for (i = 0; i < N; i++) {
		/* open the big test file for writing */
		sprintf(name, "testfile_%d_1", omp_get_thread_num());
		fd = open(name, O_WRONLY);
		if (fd < 0) {
		    fprintf(stderr, "ERROR: unable to open test file for writing in file_write_random\n");
		}
		else {
		    /* choose a random block within the file */
		    block = rand() % N;
		
		    /* write to that block */
		    lseek(fd, block * size, SEEK_SET);
		    write(fd, data[omp_get_thread_num()], size);
		    fsync(fd);
		    close(fd);
		}
	    }
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, titlebuffer);

	/* halve number of blocks but double their size */
	N = N / 2;
	size = size * 2;
    }

    /* clean up the test file */
    for (i = 0; i < nthreads; i++) {
	sprintf(name, "testfile_%d_1", i);
	unlink(name);
	free(data[i]);
    }
    free(data);
    fflush(stdout);

    return 0;
}

int file_read(unsigned int N)
{
    struct timespec start, end;
    char name[100];
    int size = 1;
    int i, j;
    unsigned char **data;
    int warmupsize;
    char titlebuffer[500];
    int fd;
    int reps;
    
    int nthreads;
    # pragma omp parallel
    if (omp_get_thread_num() == 0) nthreads = omp_get_num_threads();

    /* allocate and initialise test data */
    data = malloc(nthreads * sizeof(unsigned char*));
    for (i = 0; i < nthreads; i++) {
        data[i] = malloc(N);
    }

    fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
	fprintf(stderr, "ERROR: unable to open /dev/urandom in file_read\n");
	return 1;
    }
    for (i = 0; i < nthreads; i++) {
        read(fd, data[i], N);
    }
    close(fd);
    
    /* don't overload the system by doing too many tiny files */
    while (N >= 10000) {
	N = N / 2;
	size = size * 2;
    }

    while (N >= 1) {
	sprintf(titlebuffer, "file_read: %d files of %d bytes", N, size);

	/* first create files to read later */
        # pragma omp parallel private(i, fd, name)
	for (i = 0; i < N; i++) {
            sprintf(name, "testfile_%d_%d", omp_get_thread_num(), i);

	    fd = open(name, O_CREAT|O_WRONLY|O_TRUNC, 0644);
	    if (fd < 0) {
		fprintf(stderr, "ERROR: unable to open test file for writing in file_read\n");
	    }
	    else {
	        write(fd, data[omp_get_thread_num()], size);
	        fsync(fd);
  	        close(fd);
            }
	}

	reps = 32768 / N;
	if (reps == 0) reps = 1;

	/* now do read test */
	clock_gettime(CLOCK, &start);

        # pragma omp parallel private(i, j, fd, name)
	for (j = 0; j < reps; j++) {
	    for (i = 0; i < N; i++) {
	        sprintf(name, "testfile_%d_%d", omp_get_thread_num(), i);
		
		fd = open(name, O_RDONLY);
		if (fd < 0) {
		    fprintf(stderr, "ERROR: unable to open test file for reading in file_read\n");
		}
		else {
	            read(fd, data[omp_get_thread_num()], size);
	            close(fd);
                }
	    }
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, titlebuffer);

	/* remove files just created */
        # pragma omp parallel private(i, name)
	for (i = 0; i < N; i++) {
	    sprintf(name, "testfile_%d_%d", omp_get_thread_num(), i);
	    unlink(name);
	}

	/* halve number of files but double their size */
	N = N / 2;
	size = size * 2;
    }

    for (i = 0; i < nthreads; i++) {
	free(data[i]);
    }
    free(data);
    fflush(stdout);

    return 0;
}

#ifndef __MACH__
int file_read_direct(unsigned int N)
{
    struct timespec start, end;
    char name[100];
    int size = 1;
    int i, j;
    unsigned char **data_orig, **data;
    int warmupsize;
    char titlebuffer[500];
    int fd;
    int reps;

    int nthreads;
    # pragma omp parallel
    if (omp_get_thread_num() == 0) nthreads = omp_get_num_threads();

    /* allocate buffer and align to 512 byte boundary */
    data_orig = malloc(nthreads * sizeof(unsigned char *));
    data = malloc(nthreads * sizeof(unsigned char *));
    for (i = 0; i < nthreads; i++) {
        data_orig[i] = malloc(N + 512);
	data[i] = (unsigned char *)(((long)data_orig[i] + 511) & (~511));
    }
    
    /* initialise test data */
    fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
	fprintf(stderr, "ERROR: unable to open /dev/urandom in file_read_direct\n");
	return 1;
    }
    for (i = 0; i < nthreads; i++) {
        read(fd, data[i], N);
    }
    close(fd);

    /* N needs to be a multiple of 512 */
    while (size < 512) {
	N = N / 2;
	size = size * 2;
    }


    while (N >= 1) {
	sprintf(titlebuffer, "file_read_direct: %d files of %d bytes", N, size);

	/* first create files to read later */
        # pragma omp parallel private(i, name, fd)
	for (i = 0; i < N; i++) {
            sprintf(name, "testfile_%d_%d", omp_get_thread_num(), i);

	    fd = open(name, O_CREAT|O_WRONLY|O_TRUNC, 0644);
	    if (fd < 0) {
		fprintf(stderr, "ERROR: unable to open test file for writing in file_read_direct\n");
	    }
	    else {
                write(fd, data[omp_get_thread_num()], size);
	        fsync(fd);
	        close(fd);
            }
	}

	reps = 2048 / N;
	if (reps == 0) reps = 1;

	/* now do read test */
	clock_gettime(CLOCK, &start);

        # pragma omp parallel private(i, j, name, fd)
	for (j = 0; j < reps; j++) {
	    for (i = 0; i < N; i++) {
                sprintf(name, "testfile_%d_%d", omp_get_thread_num(), i);
		
		fd = open(name, O_RDONLY|O_DIRECT);
		if (fd < 0) {
		    fprintf(stderr, "ERROR: unable to open test file for reading in file_read_direct\n");
		}
		else {
                    read(fd, data[omp_get_thread_num()], size);
		    close(fd);
                }
	    }
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, titlebuffer);

	/* remove files just created */
        # pragma omp parallel private(i, name)
	for (i = 0; i < N; i++) {
            sprintf(name, "testfile_%d_%d", omp_get_thread_num(), i);
	    unlink(name);
	}

	/* halve number of files but double their size */
	N = N / 2;
	size = size * 2;
    }
    for (i = 0; i < nthreads; i++) {
        free(data_orig[i]);
    }
    free(data_orig);
    free(data);
    fflush(stdout);

    return 0;
}
#endif

int file_read_random(unsigned int N)
{
    struct timespec start, end;
    char name[100];
    int size = 1;
    int i, j;
    unsigned char **data;
    int warmupsize;
    char titlebuffer[500];
    int fd;
    int block;
    int reps;

    int nthreads;
    # pragma omp parallel
    if (omp_get_thread_num() == 0) nthreads = omp_get_num_threads();

    /* allocate and initialise data */
    data = malloc(nthreads * sizeof(unsigned char *));
    for (i = 0; i < nthreads; i++) {
        data[i] = malloc(N);
    } 

    fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
	fprintf(stderr, "ERROR: unable to open /dev/urandom in file_read_random\n");
	return 1;
    }
    for (i = 0; i < nthreads; i++) {
        read(fd, data[i], N);
    }
    close(fd);

    /* now create the test files, one per thread */
    # pragma omp parallel private(fd, name)
    {
        sprintf(name, "testfile_%d_1", omp_get_thread_num());
	fd = open(name, O_CREAT|O_WRONLY|O_TRUNC, 0644);
	if (fd < 0) {
            fprintf(stderr, "ERROR: unable to open test file for writing in file_read_random\n");
        }
	else {
            write(fd, data[omp_get_thread_num()], N);
	    fsync(fd);
	    close(fd);
        }
    }

    /* don't overload the system by doing too many tiny blocks */
    while (N >= 10000) {
	N = N / 2;
	size = size * 2;
    }

    while (N >= 1) {

	sprintf(titlebuffer, "file_read_random: %d blocks of %d bytes", N, size);

	reps = 65536 / N;
	if (reps == 0) reps = 1;

	clock_gettime(CLOCK, &start);

        # pragma omp parallel private(j, i, fd, name, block)
	for (j = 0; j < reps; j++) {
	    for (i = 0; i < N; i++) {
		/* open the big test file for reading */
                sprintf(name, "testfile_%d_1", omp_get_thread_num());
		fd = open(name, O_RDONLY);
		if (fd < 0) {
		    fprintf(stderr, "ERROR: unable to open test file for reading in file_read_random\n");
		}
		else {
                    /* choose a random block within the file */
                    block = rand() % N;
		
		    /* write to that block */
		    lseek(fd, block * size, SEEK_SET);
		    read(fd, data[omp_get_thread_num()], size);
		    close(fd);
                }
	    }
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, titlebuffer);

	/* halve number of blocks but double their size */
	N = N / 2;
	size = size * 2;
    }

    /* clean up the test files */
    for (i = 0; i < nthreads; i++) {
        sprintf(name, "testfile_%d_1", i);
        unlink(name);
	free(data[i]);
    }
    free(data);
    fflush(stdout);

    return 0;
}

#ifndef __MACH__
int file_read_random_direct(unsigned int N)
{
    struct timespec start, end;
    char name[100];
    int size = 1;
    int i, j;
    unsigned char **data, **data_orig;
    int warmupsize;
    char titlebuffer[500];
    int fd;
    int block;
    int reps;

    int nthreads;
    # pragma omp parallel
    if (omp_get_thread_num() == 0) nthreads = omp_get_num_threads();

    /* allocate and align buffers */
    data = malloc(nthreads * sizeof(unsigned char *));
    data_orig = malloc(nthreads * sizeof(unsigned char *));
    for (i = 0; i < nthreads; i++) {
        data_orig[i] = malloc(N + 512);
	data[i] = (unsigned char *)(((long)data_orig[i] + 511) & (~511));
    }

    /* initialise data */
    fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
	fprintf(stderr, "ERROR: unable to open /dev/urandom in file_read_random_direct\n");
	return 1;
    }
    for (i = 0; i < nthreads; i++) {
        read(fd, data[i], N);
    }
    close(fd);

    /* now create the test files, one for each thread */
    # pragma omp parallel private(fd, name)
    {
	sprintf(name, "testfile_%d_1", omp_get_thread_num());
	fd = open(name, O_CREAT|O_WRONLY|O_TRUNC, 0644);
	if (fd < 0) {
	    fprintf(stderr, "ERROR: unable to open test file for writing in file_read_random_direct\n");
	}
	else {
	    write(fd, data[omp_get_thread_num()], N);
	    fsync(fd);
	    close(fd);
	}
    }

    /* size needs to be 512 byte aligned */
    while (size < 512) {
	N = N / 2;
	size = size * 2;
    }

    while (N >= 1) {

	sprintf(titlebuffer, "file_read_random_direct: %d blocks of %d bytes", N, size);

	reps = 512 / N;
	if (reps == 0) reps = 1;

	clock_gettime(CLOCK, &start);

        # pragma omp parallel private(i, j, fd, name, block)
	for (j = 0; j < reps; j++) {
	    for (i = 0; i < N; i++) {
		/* open the big test file for reading */
		sprintf(name, "testfile_%d_1", omp_get_thread_num());
		fd = open(name, O_RDONLY|O_DIRECT);
		if (fd < 0) {
		    fprintf(stderr, "ERROR: unable to open test file for reading in file_read_random_direct\n");
		}
		else {
		    /* choose a random block within the file */
		    block = rand() % N;
		
		    /* write to that block */
		    lseek(fd, block * size, SEEK_SET);
		    read(fd, data[omp_get_thread_num()], size);
		    close(fd);
		}
	    }
	}

	clock_gettime(CLOCK, &end);
	elapsed_time_hr(start, end, titlebuffer);

	/* halve number of blocks but double their size */
	N = N / 2;
	size = size * 2;
    }

    /* clean up the test files */
    for (i = 0; i < nthreads; i++) {
	sprintf(name, "testfile_%d_1", i);
	unlink(name);
	free(data_orig[i]);
    }
    free(data_orig);
    free(data);
    fflush(stdout);

    return 0;
}
#endif
