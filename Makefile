fasterSubstring:
	gcc -O3 -pthread -Wall fasterSubstring.c -o fasterSubstring

slowestSubstring:
	gcc -pthread -Wall slowestSubstring.c -o slowestSubstring

fasterSubstringVectorized:
	gcc -O3 -pthread -Wall -mavx2 -mavx2 -mavx512vl -mavx512bw fasterSubstringVectorized.c -o fasterSubstringVectorized

all: clean fasterSubstring fasterSubstringVectorized slowestSubstring

clean:
	rm -rf fasterSubstring slowestSubstring fasterSubstringVectorized
