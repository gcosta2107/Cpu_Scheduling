CC = gcc

objects = rate.o functions.o

output: $(objects)
	$(CC) -o rate rate.o functions.o
rate.o: functions.h

.PHONY: clean
clean:
	rm $(objects) rate
	rm rate_gcs.out
