fasterSubstring:
	gcc -O3 -pthread -Wall fasterSubstring.c -o fasterSubstring

slowestSubstring:
	gcc -pthread -Wall slowestSubstring.c -o slowestSubstring

all: fasterSubstring slowestSubstring

clean:
	rm -rf fasterSubstring slowestSubstring
