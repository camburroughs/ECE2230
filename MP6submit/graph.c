/* graph.c
 * Cameron Burroughs
 * burrou5
 * C17134631
 * ECE 2230 Fall 2020
 * MP6
 *
 * Purpose: Contains modular graph functions used to create graphs,
 * 	use Dijkstras shortest path algorithm, find paths, remove paths
 *	
 * Assumptions: User has basic knowledge of how to use program.
 *
 *	For all testing purposes, vertices and weights will be 
 *	non negative 
 * 
 * Bugs:
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h> // for FLT_MAX
#include <float.h>

#include "graph.h"

#define TRUE 1
#define FALSE 0

extern int Verbose;

//private functions
void graph_rec_link_disjoint(graph_t* G, paths_t P, int dest);
void graph_rec_path_print(paths_t P, int v); 

/* This void function takes in a graph type G, an integer source vertice, 
   and an integer destination vertice. Using a private recursive helper
   function, graph_rec_link_destruct, it will find the shortest link 
   between src and dest and delete the path. It will continue until 
   path links src and dest
 */
void graph_link_disjoint(graph_t* G, int src, int dest)
{
    int flag = 1;	// only changes when no path exists
    int paths = 0;
    paths_t P;

    if(src == dest) {
	printf("The path from %d to %d is 0\n", src, dest);
	return;
    }

    printf("Finding paths from %d to %d...\n", src, dest);
    while(flag) {
	P = graph_shortest_path(G, src);
	// no paths exist
	if(P.shortestDist[dest] >= FLT_MAX) {
	    graph_path_destruct(P);
	    printf("No more paths\n");
	    flag = 0;
	    return;
	} else {   // path found
	    paths++;
	    printf("Path %d: ", paths);
	    graph_path_print(P, dest);
	    graph_rec_link_disjoint(G, P, dest);
	    graph_path_destruct(P);
	}
    }
}

/* This is a void helper function for graph_link_disjoint to 
   recursively destroy edges in a path
*/
void graph_rec_link_disjoint(graph_t* G, paths_t P, int dest)
{
    // base condition
    if(P.source == dest) 
	return;

    // destroy edge
    graph_rec_link_disjoint(G, P, P.predecessor[dest]);
    G->array[P.predecessor[dest]][dest] = FLT_MAX;
}

/* graph_contruct takes in an integer number of vertices and
   returns a graph_t type pointer that points to an allocated
   2-D array of doubles
*/
graph_t* graph_construct(int NumVertices)
{
    graph_t* G = (graph_t*)malloc(sizeof(graph_t));
    G->vertices = NumVertices;
    G->array = (double**)malloc(NumVertices*sizeof(double*));

    // allocate adjacency array
    for(int i = 0; i < NumVertices; i++) {
	G->array[i] = (double*)malloc(NumVertices*sizeof(double));
	for(int j = 0;  j < NumVertices; j++) {
	    if(i == j) {
		G->array[i][j] = 0;
	    } else {
		G->array[i][j] = FLT_MAX;
	    }
	}
    }
    graph_debug_validate(G);    
    return G;
}

/* This void function takes in a graph_t type pointer, G, and
   deallocates all space assigned to it and its members
*/
void graph_destruct(graph_t* G)
{
    if(G == NULL) return;

    for(int i = 0; i < G->vertices; i++) {
	free(G->array[i]);
    }
    free(G->array);
    free(G);

    G = NULL;
}

/* this void type function takes in a graph_t type G, an int src, an 
   int destination, and an int weight. weight is inserted into G's
   adjacency array that corresponds to the src and dest vertices
*/
void graph_add_edge(graph_t* G, int src, int dest, int weight)
{
    assert(src < G->vertices && src >= 0);
    assert(dest < G->vertices && dest >= 0);

    if(G == NULL) return;

    G->array[src][dest] = weight;
}

/* this function takes in a graph_t type pointer, G and a src vertice,
   path_src. Using Dijkstras algorithm, it computes the costs and pre-
   decessors of each vertice from the source assigning it to path_t 
   member arrays shortestDist and predecessor
*/
paths_t graph_shortest_path(graph_t* G, int path_src)
{
    int count_added = 1;
    int confirmed = 0;
    paths_t P;
    
    graph_debug_validate(G);    

    // allocate space
    P.source = path_src;
    P.predecessor = (int*)calloc(sizeof(int),G->vertices);
    P.shortestDist = (double*)calloc(sizeof(double),G->vertices);

    int W[G->vertices];
    double cost_u2w = 0;
    double MinDistance;
    int w, i, u, j;

    if(Verbose) {
	printf("Starting at node %d\n",path_src);
    }

    // initialize arrays
    for(i = 0; i < G->vertices; i++) {
	W[i] = FALSE;
	P.shortestDist[i] = G->array[path_src][i];
	P.predecessor[i] = path_src;
    }
    W[path_src] = TRUE;
    confirmed++;
    P.predecessor[path_src] = -1;
    P.shortestDist[path_src] = 0;
    
    // continuously enlarge W until is contains all vertices in G
    while(confirmed != G->vertices) {
	MinDistance = FLT_MAX;
        w = -1; 
 	//find vertex that is minimum distance from path_src
	for(j = 0; j < G->vertices; j++) {
	    if(!W[j] && P.shortestDist[j] < MinDistance) {
		MinDistance = P.shortestDist[j];
		w = j;
	    }
	}
	// if found, update other costs
	if(w != -1) {
	    W[w] = TRUE;
            confirmed++;
	    if(Verbose) {
		count_added++;
		if(MinDistance != FLT_MAX) {
		    printf("%d: %d confirmed with cost %g and predecessor %d\n",
		    count_added, w, MinDistance, P.predecessor[w]);
		}
	    }
	    for(u = 0; u < G->vertices; u++) {
		if(!W[u] && G->array[w][u] < FLT_MAX) {
		    cost_u2w = P.shortestDist[w] + G->array[w][u];
		    if(cost_u2w < P.shortestDist[u]) {
			P.shortestDist[u] = cost_u2w;
			P.predecessor[u] = w;
			if(Verbose) {
			    printf("\tdest %d has lower cost %g with predecessor %d\n", u, cost_u2w, w);
			}
		    }
		}
	    }
	} else { // if not found, add to W and ensure shortestDist is FLT_MAX (infinity)
	    for(i = 0; i < G->vertices; i++) {
		if(!W[i] && P.shortestDist[i] == FLT_MAX) {
		    W[i] = TRUE;
		    confirmed++;
		    count_added++;
		    if(Verbose) 
			printf("%d: %d confirmed with cost infinity\n", count_added, i);
		}
	    }
	}	    
    }
    if(Verbose) {
	printf("Found %d nodes, including source\n\n", count_added);
    }
    return P; 
}

/* this void function is a private helper function used by graph_path_print
   that recursively prints all vertices in path P
*/
void graph_rec_path_print(paths_t P, int v) 
{
    if(P.source == v) { //base condition
	printf("%d", v);
	return;
    }
    graph_rec_path_print(P, P.predecessor[v]);
    printf(" -- %d", v); 
}

/* this void function takes in a path P and a destination vertice, dest. It 
   uses private helper function graph_rec_path_print to print all 
   vertices v in path P from source to dest
*/
void graph_path_print(paths_t P, int dest)
{
    // not connceted
    if(P.shortestDist[dest] >= FLT_MAX) {
	printf("No path from %d to %d was found\n", P.source, dest);
	return;
    }
    // connected
    printf("The cost from %d to %d is %g and follows the path:\n\t",
	P.source, dest, P.shortestDist[dest]);
    graph_rec_path_print(P, dest);
    printf("\n");
}

/* this function deallocates the space allocated for path_t type arrays
*/
void graph_path_destruct(paths_t P)
{
    free(P.shortestDist);
    free(P.predecessor);
}

/* this void fucntion uses dijkstras shortes path algorithm and public function
   graph_shortest_path to find the largest shortest path in the graph aka the 
   diameter. It prints the resulting path and cost
*/
void graph_diameter_print(graph_t* G)
{
    int size = G->vertices;
    int i, j, index, connected;
    paths_t tmp, largest, delete[G->vertices];
    double largeNum; 
    largeNum = 0;
    connected = 1;

    graph_debug_validate(G);    

    //finds largest shortest path < FLT_MAX
    for(i = 0; i < size; i++) {
	tmp = graph_shortest_path(G, i);
	delete[i] = tmp;
	for(j = 0; j < size; j++) {
	    if(tmp.shortestDist[j] < FLT_MAX && tmp.shortestDist[j] > largeNum) {
		largest = tmp;
		largeNum = tmp.shortestDist[j];
		index = j;
	    } else if(tmp.shortestDist[j] == FLT_MAX) {
		connected = 0;
	    }
	}
    }
    // print results
    if(!connected) {
	printf("The graph contains at least one pair of vertices without a path\n");
    }
    printf("The graph contains a diameter of %g from %d to %d following the path:\n"
	,largeNum, largest.source, index);
    graph_rec_path_print(largest, index);
    printf("\n");
    
    //deallocate path space
    for(i = 0; i < G->vertices; i++) {
	graph_path_destruct(delete[i]);
    }
}

/* this public function validates the graph G and all its weights
*/
void graph_debug_validate(graph_t* G)
{
    int i, j;

    assert(G->vertices > 0);
    assert(G->array != NULL);

    for(i = 0; i < G->vertices; i++) {
	assert(G->array[i] != NULL);
	for(j = 0; j < G->vertices; j++) {
	    if(i != j)
		assert(G->array[i][j] > 0);
	}
	assert(G->array[i][i] == 0);
    }
    
}

/* this function is a validation print that is used when verbose is enabled.
   it takes in a graph_t type pointer and prints the adjacency matrix
*/
void graph_debug_print(graph_t* G) 
{
    graph_debug_validate(G);

    int i, j;
    printf("\n~~~~~~~~~~ Adjacency Matrix with %d vertices ~~~~~~~~~~\n\n", G->vertices);
    printf("     ");
    for(i = 0; i < G->vertices; i++) {
	printf("%4d: ", i);
    }
    printf("\n\n");	
    for(i = 0; i < G->vertices; i++) {
	printf("%3d:\t", i);
	for(j = 0; j < G->vertices; j++) {
	    if(G->array[i][j] >= FLT_MAX)
		printf("inf   ");
	    else
		printf("%-2.2f  ", G->array[i][j]);
	}
	printf("\n");
    }
}

