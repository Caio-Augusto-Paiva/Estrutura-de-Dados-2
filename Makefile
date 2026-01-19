CC = g++
CFLAGS = -Wall -std=c++11 -g

all: programa

programa: main.o GameReview.o HashTable.o ArvoreB.o Compressor.o
	$(CC) $(CFLAGS) -o programa main.o GameReview.o HashTable.o ArvoreB.o Compressor.o

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

GameReview.o: GameReview.cpp GameReview.h
	$(CC) $(CFLAGS) -c GameReview.cpp

HashTable.o: HashTable.cpp HashTable.h
	$(CC) $(CFLAGS) -c HashTable.cpp

ArvoreB.o: ArvoreB.cpp ArvoreB.h
	$(CC) $(CFLAGS) -c ArvoreB.cpp

Compressor.o: Compressor.cpp Compressor.h
	$(CC) $(CFLAGS) -c Compressor.cpp

clean:
	rm -f *.o programa reviews.bin reviewsComp.bin reviewsDesc.txt