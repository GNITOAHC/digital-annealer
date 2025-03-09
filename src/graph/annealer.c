#include "./annealer.h"
#include <math.h>
#include <stdio.h>

double sa_anneal (graph_t *graph, const double init_t, const double final_t, const int tau,
                  const int print_progress) {
    for (int i = 0; i <= tau; ++i) {
        const double t   = init_t * (1 - ((double)i / tau)) + final_t * ((double)i / tau);
        const int length = graph->nodes_len;

        for (int j = 0; j < length; ++j) {
            const int idx = graph->nodes[j]->index;

            const double delta_e = sa_energy_diff(graph, idx);
            double pi_accept     = 1.0 < exp(-delta_e / t) ? 1.0 : exp(-delta_e / t);

            /* if (isnan(t)) { pi_accept = 1.0; } */
            /* if (isnan(pi_accept)) { pi_accept = 1.0; } */
            /* if (isnan(pi_accept)) { printf("NAN\n"); } */

            if ((double)rand() / (double)RAND_MAX < pi_accept) {
                node_t *flipped_node = find_node(graph, idx);
                if (flipped_node != NULL) {
                    flipped_node->val = flipped_node->val == SPIN_UP ? SPIN_DOWN : SPIN_UP;
                }
            }
        }

        if (print_progress) printf("%f %f", t, sa_hamiltonian_energy(graph));
    }

    return sa_hamiltonian_energy(graph);
}
