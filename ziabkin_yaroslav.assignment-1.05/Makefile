game: main.o terrain_generator.o queue.o characters_players.o min_heap.o moves_queue.o
	gcc -Wall -Werror -g main.o terrain_generator.o queue.o characters_players.o min_heap.o moves_queue.o -o game -lncurses

main.o: main.c terrain_generator.h queue.h characters_players.h moves_queue.h
	gcc -Wall -Werror -g -c main.c

terrain_generator.o: terrain_generator.c terrain_generator.h queue.h characters_players.h
	gcc -Wall -Werror -g -c terrain_generator.c

queue.o: queue.c queue.h
	gcc -Wall -Werror -g -c queue.c

characters_players.o: characters_players.c terrain_generator.h characters_players.h min_heap.h
	gcc -Wall -Werror -g -c characters_players.c 

min_heap.o: min_heap.c min_heap.h terrain_generator.h
	gcc -Wall -Werror -g -c min_heap.c

moves_queue.o: moves_queue.c moves_queue.h terrain_generator.h characters_players.h
	gcc -Wall -Werror -g -c moves_queue.c

clean:
	rm -f *.o game *~