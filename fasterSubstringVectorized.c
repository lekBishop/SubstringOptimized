#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <immintrin.h>
#include <string.h>

#define toInt(a) (a - 0)
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

static int commonlen(int length_s1, int length_s2, char *s1, char *s2)
{
    int res = 0, pos = 0;
    char vector1[16];

    while (pos + 16 < length_s1 && pos + 16 < length_s2) {
        __m256i s1_vector = _mm256_lddqu_si256((__m256i const *)s1);
        __m256i s2_vector = _mm256_lddqu_si256((__m256i const *)s2);
        __m256i compare = _mm256_cmpeq_epi32(s1_vector, s2_vector);
        _mm256_storeu_epi32((__m256i*) vector1, compare);
        
        for (int i = 0; i < 16; i++)
        {
            
            pos++;
            s1++;
            s2++;
            if (!vector1[i])
                return res;
            res++;
        }
    }

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
           args->result = MAX(args->result, commonlen(args->m - i, args->n - j, args->str1 + i, args->str2 + j));
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
    double elapsed;
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
        
        num_threads = 1;

    // Get bytes in second file
    fseek(infile2, 0L, SEEK_END);
    numbytes2 = ftell(infile2);

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

    tinfo = calloc(num_threads, sizeof(*tinfo));

    if (tinfo == NULL)
    {
        die("thread args fail");
    }
    
    length = numbytes1 / num_threads;

    {
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < num_threads; i++)
    {
        tinfo[i].length = length;
        tinfo[i].m = numbytes1;
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
    }

    printf("The longest substring is %d characters long\n", length);
    printf("Calculation took %f seconds\n", elapsed);
}
