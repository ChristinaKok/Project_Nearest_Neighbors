CC = g++
CFLAGS = -g -Wall
all: lsh cube graph_search graph_search2 cluster cluster2

lsh: main_lsh.cpp functions.cpp lsh.cpp
	$(CC) $(CFLAGS) -o lsh main_lsh.cpp functions.cpp lsh.cpp

cube: main_hypercube.cpp hypercube.cpp functions.cpp lsh.cpp
	$(CC) $(CFLAGS) -o cube main_hypercube.cpp hypercube.cpp functions.cpp lsh.cpp

graph_search: main_graph.cpp graph.cpp functions.cpp lsh.cpp
	$(CC) $(CFLAGS) -o graph_search main_graph.cpp graph.cpp functions.cpp lsh.cpp

graph_search2: main_graph_red.cpp graph.cpp functions.cpp lsh.cpp
	$(CC) $(CFLAGS) -o graph_search2 main_graph_red.cpp graph.cpp functions.cpp lsh.cpp

cluster: main_cluster.cpp clusters.cpp lsh.cpp hypercube.cpp functions.cpp
	$(CC) $(CFLAGS) -O2 -o cluster main_cluster.cpp clusters.cpp hypercube.cpp functions.cpp lsh.cpp

cluster2: main_cluster_red.cpp clusters.cpp lsh.cpp hypercube.cpp functions.cpp
	$(CC) $(CFLAGS) -O2 -o cluster2 main_cluster_red.cpp clusters.cpp hypercube.cpp functions.cpp lsh.cpp

clean:
	rm -f *.o lsh cube graph_search graph_search2 cluster cluster2
