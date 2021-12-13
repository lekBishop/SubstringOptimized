#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define MAX_THREADS 100
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)

struct thread_data {
    char *str1, *str2;
    long n, m;
    int start, length, result;
};

static void die(const char * err)
{
    fprintf(stderr, "Error: %s", err);
    exit(1);
}

static int commonlen(char *s1, char *s2)
{
    int res = 0;

    while(*s1 && *s2 && unlikely(*s1++ == *s2++))
        ++res;

    return res;
}

static void *LCS(void *arg)
{
    int end;
    struct thread_data *args = arg;

    end = args->start + args->length;

    for(int i = args->start; i <= end; i++)
    {
        for(int j = 0; j <= args->n; j++)
        {
           args->result = MAX(args->result, commonlen(args->str1 + i, args->str2 + j));
        }
        if (args->result >= args->m - i)
            break;
    }

    pthread_exit(NULL);
    return 0;
}

/*
This program takes two files as command line inputs and returns the longest commond substring
It is assumed that the txt files are one line.
*/
int main(int argc, char *argv[])
{
    struct timespec start,finish;
    double elapsed, avg_elapsed;
    FILE *infile1, *infile2;
    char *str1, *str2;
    long numbytes1, numbytes2;
    pthread_t threads[MAX_THREADS];
    struct thread_data *tinfo;
    int length, num_threads;


    if(argc !=3) {
        fprintf(stderr, "Usage: %s <file 1> <file 2>", argv[0]);
        exit(1);
    }

    infile1 = fopen(argv[1], "r");
    infile2 = fopen(argv[2], "r");

    if(!infile1 || !infile2)
        die("File open failure");
    
    // Get bytes in first file
    fseek(infile1, 0L, SEEK_END);
    numbytes1 = ftell(infile1);

    // Get bytes in second file
    fseek(infile2, 0L, SEEK_END);
    numbytes2 = ftell(infile2);

    if (numbytes1 >= 10000)
        num_threads = 100;
    if (numbytes1 >= 1000)
        num_threads = 50;
    if (numbytes1 >= 100)
        num_threads = 4;
    else
        num_threads = 1;

    // Reset File pointers to start
    rewind(infile1);
    rewind(infile2);

    // Allocate strings and null terminate
    str1 = malloc(numbytes1 + 1);
    str2 = malloc(numbytes2 + 1);

    if (!str1 || !str2)
        die("Malloc Failed");

    str1[numbytes1] = '\0';
    str2[numbytes2] = '\0';
    
    if (fread(str1, 1, numbytes1, infile1) != numbytes1 || fread(str2, 1, numbytes2, infile2) != numbytes2)
        die("fread error");

    avg_elapsed = 0;
    for (size_t i = 0; i < 50; i++)
    {
        tinfo = calloc(num_threads, sizeof(*tinfo));

        if (tinfo == NULL)
        {
            die("thread args fail");
        }
        
        length = numbytes1 / num_threads;
        
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int i = 0; i < num_threads; i++)
        {
            tinfo[i].length = length;
            tinfo[i].m = numbytes2;
            tinfo[i].n = numbytes2;
            tinfo[i].start = length * i;
            tinfo[i].str1 = str1;
            tinfo[i].str2 = str2;
            tinfo[i].result = 0;

            pthread_create(&threads[i], NULL, LCS, &tinfo[i]);
        }

        length = 0;

        for (int i = 0; i < num_threads; i++)
        {
            pthread_join(threads[i], NULL);

            length = MAX(length, tinfo[i].result);
        }
        clock_gettime(CLOCK_MONOTONIC, &finish);
        elapsed = (finish.tv_sec - start.tv_sec);
        elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
        avg_elapsed += elapsed;
        free(tinfo);
        printf("Calculation took %f seconds\n", elapsed);
    }

    avg_elapsed = avg_elapsed / 50;

    printf("The longest substring is %d characters long\n", length);
    printf("Average calculation took %f seconds\n", avg_elapsed);
}
