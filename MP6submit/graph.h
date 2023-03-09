/* graph.h
 * Cameron L Burroughs
 * burrou5
 * C17134631
 * ECE 2230 Fall 2020
 * MP6
 * 
 * Public functions for Dijkstras graphs
 */

//coordinate struct definition for building random graphs
typedef struct coordinatesTag {
    double x;
    double y;
} coordinates_t;

//paths struct containing costs and predecessors
typedef struct pathsTag {
    int source;
    int* predecessor;
    double* shortestDist;
} paths_t; 

//graph struct containing graph array and size
typedef struct graphTag {
    int vertices;
    double **array;
} graph_t;

//public functions
graph_t* graph_construct(int NumVertices);
void graph_destruct(graph_t* G);
void graph_add_edge(graph_t* G, int src, int dest, int weight);
void graph_debug_print(graph_t* G);
void graph_debug_validate(graph_t* G);
void graph_path_print(paths_t P, int v);
void graph_path_destruct(paths_t P);
void graph_diameter_print(graph_t* G);
void graph_link_disjoint(graph_t*G, int src, int dest);
paths_t graph_shortest_path(graph_t* G, int path_src);
