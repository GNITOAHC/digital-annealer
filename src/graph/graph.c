#include "./graph.h"
#include <stdio.h>
#include <stdlib.h>

int push_back_node (graph_t *graph, node_t *node) {
    if (graph->nodes_len + 1 >= graph->nodes_cap) {
        const int new_cap  = graph->nodes_cap * 2;
        node_t **new_nodes = (node_t **)realloc(graph->nodes, sizeof(node_t *) * new_cap);
        if (new_nodes == NULL) return -1; /* Realloc failed */
        graph->nodes_cap = new_cap;
        graph->nodes     = new_nodes;
    }

    /* printf("Node %d\n", node->index); */
    graph->nodes[graph->nodes_len++] = node;
    return 1;
}

int push_back_edge (graph_t *graph, edge_t *edge) {
    if (graph->edges_len + 1 >= graph->edges_cap) {
        graph->edges_cap *= 2;
        edge_t **new_edges = (edge_t **)realloc(graph->edges, sizeof(edge_t *) * graph->edges_cap);
        graph->edges       = new_edges;
    }

    if (graph->edges == NULL) return -1; /* Realloc failed */

    graph->edges[graph->edges_len++] = edge;
    return 1;
}

node_t *find_node (graph_t *graph, int index) {
    for (int i = 0; i < graph->nodes_len; ++i) {
        if (graph->nodes[i]->index == index) return graph->nodes[i];
    }
    return NULL;
}

void print_graph (graph_t *graph) {
    printf("Nodes:\n");
    for (int i = 0; i < graph->nodes_len; i++) {
        printf("Node %d: val %d, idx %d\n", i, graph->nodes[i]->val, graph->nodes[i]->index);
    }
    printf("\nEdges:\n");
    for (int i = 0; i < graph->edges_len; i++) {
        printf("Edge %d: %d -> %d (%f)\n", i, graph->edges[i]->from->index,
               graph->edges[i]->to->index, graph->edges[i]->weight);
    }
}

void free_graph (graph_t *graph) {
    for (int i = 0; i < graph->nodes_len; i++) {
        free(graph->nodes[i]);
    }
    free(graph->nodes);
    for (int i = 0; i < graph->edges_len; i++) {
        free(graph->edges[i]);
    }
    free(graph->edges);

    free(graph->const_map);
    free(graph);
}

/* Simulated annealing specific functions */

int sa_push_back_edge (graph_t *graph, const int po1, const int po2, const double co) {
    node_t *n1 = find_node(graph, po1), *n2 = find_node(graph, po2);

    if (n1 == NULL) {
        if ((n1 = new_node(po1, SPIN_UP)) == NULL) return -1;
        if (push_back_node(graph, n1) == -1) return -1;
    }

    if (po1 == po2) n2 = n1; /* Self-loop */

    if (n2 == NULL) {
        if ((n2 = new_node(po2, SPIN_UP)) == NULL) return -1;
        if (push_back_node(graph, n2) == -1) return -1;
    }

    if (n1->index > n2->index) {
        node_t *tmp = n1;
        n1          = n2;
        n2          = tmp;
    }
    edge_t *ne = new_edge(n1, n2, co);

    return push_back_edge(graph, ne);
}

int sa_push_back_constant_map (graph_t *graph, const int po, const double co) {
    if (graph->const_map_len < po + 1) {
        double *new_const_map = (double *)realloc(graph->const_map, sizeof(double) * (po + 1));
        if (new_const_map == NULL) return -1;
        graph->const_map = new_const_map;
    }
    graph->const_map[po] += co;

    return 1;
}

int sa_push_back_constant (graph_t *graph, const double co) {
    graph->constant += co;
    return 1;
}

double sa_hamiltonian_energy (graph_t *graph) {
    double energy = 0.0;
    for (int i = 0; i < graph->edges_len; ++i) {
        energy += graph->edges[i]->weight * (double)graph->edges[i]->from->val *
                  (double)graph->edges[i]->to->val;
    }

    for (int i = 0; i < graph->const_map_len; ++i) {
        energy += graph->const_map[i] * (double)graph->nodes[i]->val;
    }

    energy += graph->constant;

    return energy;
}

double sa_energy_diff (graph_t *graph, const int i) {
    double diff           = 0.0;
    int self_loop_already = 0;
    for (int j = 0; j < graph->edges_len; ++j) {
        if (graph->edges[j]->from->index != i && graph->edges[j]->to->index != i) continue;

        if (graph->edges[j]->from->index == graph->edges[j]->to->index && !self_loop_already) {
            self_loop_already = 1;
            diff += graph->edges[j]->weight * (double)graph->edges[j]->from->val *
                    (double)graph->edges[j]->to->val;
            continue;
        }

        diff += graph->edges[j]->weight * (double)graph->edges[j]->from->val *
                (double)graph->edges[j]->to->val;
    }

    for (int j = 0; j < graph->const_map_len; ++j) {
        if (graph->nodes[j]->index == i) {
            diff += graph->const_map[j] * (double)graph->nodes[j]->val;
        }
    }

    return -2.0 * diff;
}
