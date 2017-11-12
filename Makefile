all : multi_block

multi_block: multi_block.o
	gcc -o multi_block multi_block.o -lssl -lcrypto -lm -lnetfilter_queue

multi_block.o: multi_block.c multi_block.h
	gcc -c -o multi_block.o multi_block.c

clean:
	rm multi_block *.o
