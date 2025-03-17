#pragma once

#include <stdlib.h>

enum spin { SPIN_UP = 1, SPIN_DOWN = -1 };

typedef struct node {
    int index;
    enum spin val;
} node_t;

static inline node_t *new_node (int index, enum spin val) {
    node_t *node = (node_t *)malloc(sizeof(node_t));
    if (node == NULL) return NULL;
    node->index = index;
    node->val   = val;
    return node;
}

typedef struct edge {
    int index;
    node_t *from, *to;
    double weight;
} edge_t;

static inline edge_t *new_edge (node_t *from, node_t *to, double weight) {
    edge_t *edge = (edge_t *)malloc(sizeof(edge_t));
    if (edge == NULL) return NULL;
    edge->from   = from;
    edge->to     = to;
    edge->weight = weight;
    return edge;
}

#define DEFAULT_NODE_CAP 10
#define DEFAULT_EDGE_CAP 10
typedef struct {
    int nodes_cap;  /* Capacity of the nodes of the graph */
    int nodes_len;  /* Number of nodes */
    node_t **nodes; /* Array of nodes, sorted by index (nodes[i] == the ith node's pointer) */

    int edges_cap;  /* Capacity of the edges of the graph */
    int edges_len;  /* Number of edges */
    edge_t **edges; /* Array of edges, sorted by index */

    int const_map_len;
    double *const_map;

    double constant; /* Constant value */
} graph_t;

static inline graph_t *new_graph () {
    graph_t *graph     = (graph_t *)malloc(sizeof(graph_t));
    node_t **new_nodes = (node_t **)malloc(sizeof(node_t *) * DEFAULT_NODE_CAP);
    edge_t **new_edges = (edge_t **)malloc(sizeof(edge_t *) * DEFAULT_EDGE_CAP);

    if (graph == NULL || new_nodes == NULL || new_edges == NULL) return NULL;

    graph->nodes_cap     = DEFAULT_NODE_CAP;
    graph->nodes_len     = 0;
    graph->nodes         = new_nodes;
    graph->edges_cap     = DEFAULT_EDGE_CAP;
    graph->edges_len     = 0;
    graph->edges         = new_edges;
    graph->const_map_len = -1;
    graph->const_map     = NULL;
    graph->constant      = 0.0;

    return graph;
}

int push_back_node(graph_t *graph, node_t *node);
int push_back_edge(graph_t *graph, edge_t *edge);
node_t *find_node(graph_t *graph, int index);

void print_graph(graph_t *graph);
void free_graph(graph_t *graph);

/* Simulated annealing specific functions */

/* Push back an edge to the graph, given polynomial 1, polynomial 2 and coefficient */
int sa_push_back_edge(graph_t *graph, const int po1, const int po2, const double co);
/* Push back a constant map to the graph, given polynomial and coefficient */
int sa_push_back_constant_map(graph_t *graph, const int po, const double co);
/* Add a constant to the graph */
int sa_push_back_constant(graph_t *graph, const double co);

/* Get the hamiltonian energy of the graph */
double sa_hamiltonian_energy(graph_t *graph);
/* Get the hamiltonian energy difference of the graph if flip the node i */
double sa_energy_diff(graph_t *graph, const int i);
