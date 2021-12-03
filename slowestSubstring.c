#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

static void die(const char * err)
{
    fprintf(stderr, "Error: %s", err);
    exit(1);
}

static int LCS(char *str1, char *str2, long m, long n)
{
    int lengths[m + 1][n + 1];
    int res = 0;

    for(int i = 0; i <= m; i++)
    {
        for(int j = 0; j <= n; j++)
        {
            if (i == 0 || j == 0)
                lengths[i][j] = 0;
            else if (str1[i - 1] == str2[j - 1]) {
                lengths[i][j] = lengths[i - 1][j - 1] + 1;
                res = MAX(res, lengths[i][j]);
            }
            else lengths[i][j] = 0;
        }
    }

    return res;
}

/*
This program takes two files as command line inputs and returns the longest commond substring
It is assumed that the txt files are one line.
*/
int main(int argc, char *argv[])
{
    FILE *infile1, *infile2;
    char *buf1, *buf2;
    long numbytes1, numbytes2, length;
    clock_t start, end;

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

    printf("File1: %ld\nFile2: %ld", numbytes1, numbytes2);

    // Reset File pointers to start
    rewind(infile1);
    rewind(infile2);

    // Allocate strings and null terminate
    buf1 = malloc(numbytes1 + 1);
    buf2 = malloc(numbytes2 + 1);

    if (!buf1 || !buf2)
        die("Malloc Failed");

    buf1[numbytes1] = '\0';
    buf2[numbytes2] = '\0';
    
    if (fread(buf1, 1, numbytes1, infile1) != numbytes1 || fread(buf2, 1, numbytes2, infile2) != numbytes2)
        die("fread error");

    start = clock();
    length = LCS(buf1, buf2, numbytes1, numbytes2);
    end = clock();

    printf("The longest substring is: %ld characters long\n", length);
    printf("Calculation took %d seconds\n", (double)(start-end) / CLOCKS_PER_SEC);
}
