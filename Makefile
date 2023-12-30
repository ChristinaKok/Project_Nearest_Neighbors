CC = g++
CFLAGS = -g -Wall
all: lsh cube graph_search graph_search2

lsh: main_lsh.cpp functions.cpp lsh.cpp
	$(CC) $(CFLAGS) -o lsh main_lsh.cpp functions.cpp lsh.cpp

cube: main_hypercube.cpp hypercube.cpp functions.cpp lsh.cpp
	$(CC) $(CFLAGS) -o cube main_hypercube.cpp hypercube.cpp functions.cpp lsh.cpp

graph_search: main_graph.cpp graph.cpp functions.cpp lsh.cpp
	$(CC) $(CFLAGS) -o graph_search main_graph.cpp graph.cpp functions.cpp lsh.cpp

graph_search2: main_graph_red.cpp graph.cpp functions.cpp lsh.cpp
	$(CC) $(CFLAGS) -o graph_search2 main_graph_red.cpp graph.cpp functions.cpp lsh.cpp

clean:
	rm -f *.o lsh cube graph_search graph_search2
