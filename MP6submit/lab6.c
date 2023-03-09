/* lab6.c
 * lab6: Dijkstra Algorithm
 * Cameron L Burroughs
 * burrou5
 * C17134631
 * ECE 2230, Fall 2020
 * MP6
 *
 * This file contains drivers
 * 
 * These drivers build graphs to be searched by the dijkstra
 * algorithm and has the following specifiers:
 *	-g [1|2|3|4]	specifies graph type
 *	-n N		N number of verticies in graph
 *	-a R		approx number of adjacent values 0 < R < N
 *	-h [1|2|3]	Graph Operation
 *	-s S		Num of source vertex 0 <= S < N
 *	-d D		Num of destination vertex 0 <= D < N
 *	-v		Enable verbose output
 *	-r <seed>	seed for the uniform ran num generator
 * 
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>
#include <float.h>

#include "graph.h"

#define TRUE 1
#define FALSE 0

int Seed = 10212018;
int GraphType = -1;
int Verbose = FALSE;
int Vertices = 7;
int Adjacent = 3;
int Operation = -1;
int Source = 1;
int Destination = 5;

void build_strong(graph_t* G);
void build_random(graph_t* G);

void getCommandLine(int argc, char** argv);

int main(int argc, char **argv)
{
    getCommandLine(argc, argv);
    assert(Source <= Destination);

    printf("Seed: %d\n", Seed);
    srand48(Seed);

    clock_t start, end;
   
    graph_t* G;

    // weakly connected, directed graph
    if(GraphType == 1) {
	G = graph_construct(7);
	assert(G->vertices == 7);
	graph_add_edge(G, 1, 2, 3);
	graph_add_edge(G, 1, 6, 5);
	graph_add_edge(G, 2, 3, 7);
	graph_add_edge(G, 2, 4, 3);
	graph_add_edge(G, 2, 6, 10);
	graph_add_edge(G, 3, 4, 5);
	graph_add_edge(G, 3, 5, 1);
	graph_add_edge(G, 4, 5, 6);
	graph_add_edge(G, 5, 6, 7);
	graph_add_edge(G, 6, 0, 4);
	graph_add_edge(G, 6, 2, 5);
	graph_add_edge(G, 6, 3, 8);
	graph_add_edge(G, 6, 4, 9);

    // directed graph with symmetric weights
    } else if(GraphType == 2) {	
	G = graph_construct(10);
	assert(G->vertices == 10);
	graph_add_edge(G, 0, 1, 2);
	graph_add_edge(G, 0, 2, 1);
	graph_add_edge(G, 0, 3, 6);
	graph_add_edge(G, 1, 0, 2);
	graph_add_edge(G, 1, 4, 1);
	graph_add_edge(G, 1, 8, 3);
	graph_add_edge(G, 2, 0, 1);
	graph_add_edge(G, 2, 4, 3);
	graph_add_edge(G, 2, 5, 7);
	graph_add_edge(G, 3, 0, 6);
	graph_add_edge(G, 3, 5, 1);
	graph_add_edge(G, 3, 9, 5);
	graph_add_edge(G, 4, 1, 1);
	graph_add_edge(G, 4, 2, 3);
	graph_add_edge(G, 4, 6, 4);
	graph_add_edge(G, 5, 2, 7);
	graph_add_edge(G, 5, 3, 1);
	graph_add_edge(G, 5, 7, 4);
	graph_add_edge(G, 6, 4, 4);
	graph_add_edge(G, 6, 7, 5);
	graph_add_edge(G, 6, 8, 1);
	graph_add_edge(G, 7, 5, 4);
	graph_add_edge(G, 7, 6, 5);
	graph_add_edge(G, 7, 9, 1);
	graph_add_edge(G, 8, 1, 3);
	graph_add_edge(G, 8, 6, 1);
	graph_add_edge(G, 8, 9, 4);
	graph_add_edge(G, 9, 3, 5);
	graph_add_edge(G, 9, 7, 1);
	graph_add_edge(G, 9, 8, 4);

    // strongly connected graph
    } else if(GraphType == 3) {
	G = graph_construct(Vertices);
	build_strong(G);

    // randomly connected graph
    } else if(GraphType == 4) {
	G = graph_construct(Vertices);
	build_random(G);
    }

    start = clock();
    // shortest path
    if(Operation == 1) {
        assert(Source < Vertices && Source >= 0);
        assert(Destination < Vertices && Destination >= 0);
	paths_t P = graph_shortest_path(G, Source);
	graph_path_print(P, Destination);
	graph_path_destruct(P);
	if(Verbose)
	    graph_debug_print(G);
	graph_destruct(G);
    // diameter
    } else if(Operation == 2) {
	graph_diameter_print(G);
	if(Verbose)
	    graph_debug_print(G);
	graph_destruct(G);
    // multiple link-disjoint paths
    } else if(Operation == 3) {

















        assert(Source < Vertices && Source >= 0);
        assert(Destination < Vertices && Destination >= 0);
	graph_link_disjoint(G, Source, Destination);
	if(Verbose)
	    graph_debug_print(G);
	graph_destruct(G);



    }

    end = clock();
    
    printf("\n***** TIME TO EXECUTE: %g ms *****\n\n",1000*((double)(end-start))/CLOCKS_PER_SEC);

    return 0;
}

/* this function is a helper function used to build a strong 
   graph with N vertices
*/
void build_strong(graph_t* G) 
{ 
    double weight;
    int i, j, size;
    size = G->vertices;

    for(i = 0; i < size; i++) {
	for(j = 0; j < size; j++) {
	    if(i == j) {
		G->array[i][j] = 0;
	    } else {
		weight = fabs((i - j)/1.5);
		weight += pow(i - j + 1.5, 2);
		weight += (1.5*j);
		G->array[i][j] = weight;
	    }
	}
    }
}

/* this function is a helper function used to build a random graph 
   with N vertices and R average adjacent vertices
*/
void build_random(graph_t* G)
{
    coordinates_t set[G->vertices];
    int i, j, V, R, min, max, adj;
    double D, C, M, F, avg;

    assert(Adjacent < Vertices && Adjacent > 0);

    V = G->vertices;
    R = Adjacent;
    C = sqrt(R/(V*M_PI));
    M = log10(1+pow(1/C,2));
    adj = max = avg = 0;
    min = (int)FLT_MAX;
   
    //set first and last vertice locations
    set[0].x = 0.25;
    set[0].y = 0;
    set[V-1].x = 0.75; 
    set[V-1].y = 0;
    
    //set random (x, y) locations
    for(i = 1; i < V-1; i++) {
	set[i].x = drand48(); 
	set[i].y = drand48(); 
    }
    
    //assigning weights
    for(i = 0; i < V; i++) {
        adj = 0;
	for(j = 0; j < V; j++) {
	    D = sqrt(pow(set[i].x - set[j].x,2) + pow(set[i].y - set[j].y,2));
	    if(i == j) {
		G->array[i][j] = 0;
	    } else if(D <= C) {
		F = log10(1 + pow(1/(D+(C/100)), 2));
		G->array[i][j] = M/F;
		adj++;
	    } else {
		G->array[i][j] = FLT_MAX;
	    }
	}
	if(adj > max) { max = adj; }
	else if(adj < min) { min = adj; }
	avg += adj;
    }
   avg = avg/V;
   
   // final print
   printf("~~~~~Adjacent Vertices~~~~~\n\tAverage: %g\n\t Minimum: %d\n\t", 
	avg, min);
   printf("Maximum: %d\n", max);
    
}

/* private function to obtain command line arguments */
void getCommandLine(int argc, char** argv)
{
    int c;
//    int index;

    while((c = getopt(argc, argv, "g:n:a:h:s:d:vr:")) != -1){
	switch(c) {
	    case 'g': GraphType = atoi(optarg);		break;
	    case 'n': Vertices = atoi(optarg);		break; 
    	    case 'a': Adjacent = atoi(optarg);		break;
	    case 'h': Operation = atoi(optarg);		break;
	    case 's': Source = atoi(optarg);
	    case 'd': Destination = atoi(optarg);	break;
	    case 'v': Verbose = TRUE;			break;
	    case 'r': Seed = atoi(optarg);		break;
	    case '?': 
		if(isprint(optopt))
		    fprintf(stderr, "Unknown Option: %c\n", optopt);
		else
		    fprintf(stderr, "Unknown option character: '\\x%x'.\n", optopt);
	    default:
		printf("Lab6 command line options\n");
		printf("General Options ---------\n");
		printf("   -g [1|2|3|4]     graph type\n");
		printf("   -n N             N number of verticies\n");
		printf("   -a R             R adjacent verticies\n");
		printf("   -h [1|2|3]       graph operation\n");
		printf("   -g [1|2|3|4]     graph type\n");
		printf("   -g [1|2|3|4]     graph type\n");
		printf("   -g [1|2|3|4]     graph type\n");
		printf("   -g [1|2|3|4]     graph type\n");
		printf("   -g [1|2|3|4]     graph type\n");
		printf("   -g [1|2|3|4]     graph type\n");
		printf("   -g [1|2|3|4]     graph type\n");
	}
    }
}

