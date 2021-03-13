major2: main.o processor.o
	gcc -o major2 main.c processor.c

main.o: processor.h processor.c main.c
	gcc -c processor.c main.c

processor.o: processor.h processor.c
	gcc -c processor.c
clean:
	rm *.o
	rm major2

run: 
	./major2
