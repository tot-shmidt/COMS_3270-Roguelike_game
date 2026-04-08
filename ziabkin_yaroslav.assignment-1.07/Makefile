game: main.o terrain_generator.o queue.o characters_players.o min_heap.o moves_queue.o database.o
	g++ -Wall -Werror -g main.o terrain_generator.o queue.o characters_players.o min_heap.o moves_queue.o database.o -o game -lncurses

main.o: main.cpp terrain_generator.h queue.h characters_players.h moves_queue.h database.h
	g++ -Wall -Werror -g -c main.cpp

terrain_generator.o: terrain_generator.cpp terrain_generator.h queue.h characters_players.h
	g++ -Wall -Werror -g -c terrain_generator.cpp

queue.o: queue.cpp queue.h
	g++ -Wall -Werror -g -c queue.cpp

characters_players.o: characters_players.cpp terrain_generator.h characters_players.h min_heap.h
	g++ -Wall -Werror -g -c characters_players.cpp 

min_heap.o: min_heap.cpp min_heap.h terrain_generator.h
	g++ -Wall -Werror -g -c min_heap.cpp

moves_queue.o: moves_queue.cpp moves_queue.h terrain_generator.h characters_players.h
	g++ -Wall -Werror -g -c moves_queue.cpp

database.o: database.h database.cpp
	g++ -Wall -Werror -g -c database.cpp

clean:
	rm -f *.o game *~