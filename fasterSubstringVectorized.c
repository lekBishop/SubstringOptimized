#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <immintrin.h>

#define toInt(a) (a - 0)
#define NUM_THREADS 100
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)

struct thread_data {
    char *str1, *str2;
    long n;
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
    int i, j, x, ls2 = 16;
    int s1_len = strlen(s1);
    int s2_len = strlen(s2);
    short s1_store[16];
    short s2_store[16];
    while (*s1 && *s2 && unlikely(*s1 == *s2)) {
		ls1 = 16;
        for (i = 0; i < 16; i++){
            if (s1 && s2){
                ;
            } else{
                ls1 = i + 1;
                break;
            }
            s1_store[i] = (short) toInt(*(s1++));
            s2_store[i] = (short) toInt(*(s2++));
        }
        __m128i s1_vector = _mm128_set_epi8(s1_store[0], s1_store[1],s1_store[2], s1_store[3],s1_store[4], s1_store[5], s1_store[6], s1_store[7], 
        s1_store[8], s1_store[9],s1_store[10], s1_store[11],s1_store[12], s1_store[13],s1_store[14], s1_store[15]);
        __m128i s2_vector = _mm128_set_epi8(s2_store[0], s2_store[1],s2_store[2], s2_store[3],s2_store[4], s2_store[5], s2_store[6], s2_store[7], 
        s2_store[8], s2_store[9],s2_store[10], s2_store[11],s2_store[12], s2_store[13],s2_store[14], s2_store[15]);
        x = __mm_cmpestrc(s1_vector, ls1, s2_vector, ls2, _SIDD_CMP_RANGES);
        print("%d\n", x);
    }
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
    pthread_t threads[NUM_THREADS];
    struct thread_data *tinfo;
    int length;


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

    tinfo = calloc(NUM_THREADS, sizeof(*tinfo));

    if (tinfo == NULL)
    {
        die("thread args fail");
    }
    
    length = numbytes1 / NUM_THREADS;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < NUM_THREADS; i++)
    {
        tinfo[i].length = length;
        tinfo[i].n = numbytes2;
        tinfo[i].start = length * i;
        tinfo[i].str1 = str1;
        tinfo[i].str2 = str2;
        tinfo[i].result = 0;

        pthread_create(&threads[i], NULL, LCS, &tinfo[i]);
    }

    length = 0;

    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);

        length = MAX(length, tinfo[i].result);
    }
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

    printf("The longest substring is %d characters long\n", length);
    printf("Calculation took %f seconds\n", elapsed);
}
