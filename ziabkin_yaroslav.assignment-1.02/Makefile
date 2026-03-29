map_generator: main.o terrain_generator.o queue.o
	gcc -Wall -Werror -g main.o terrain_generator.o queue.o -o map_generator

main.o: main.c terrain_generator.h queue.h
	gcc -Wall -Werror -g -c main.c

terrain_generator.o: terrain_generator.c terrain_generator.h queue.h
	gcc -Wall -Werror -g -c terrain_generator.c

queue.o: queue.c queue.h
	gcc -Wall -Werror -g -c queue.c

clean:
	rm -f *.o map_generator *~
