fasterSubstring:
	gcc -O3 -pthread -Wall fasterSubstring.c -o fasterSubstring

slowestSubstring:
	gcc -pthread -Wall slowestSubstring.c -o slowestSubstring

fasterSubstringVectorized:
	gcc -pthread -Wall -mavx2 fasterSubstringVectorized.c -o fasterSubstringVectorized

all: clean fasterSubstring slowestSubstring

clean:
	rm -rf fasterSubstring slowestSubstring
