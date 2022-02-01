slowestSubstring:
	gcc -pthread -Wall slowestSubstring.c -o slowestSubstring

fasterSubstringVectorized:
	gcc -O3 -pthread -Wall -mavx2 -mavx2 -mavx512vl -mavx512bw fastSubstring.c -o fastSubstring

all: clean fastSubstring slowestSubstring

clean:
	rm -rf  slowestSubstring fastSubstring
