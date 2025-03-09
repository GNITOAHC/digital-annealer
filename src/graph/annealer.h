#pragma once
#include "./graph.h"

double sa_anneal(graph_t *graph, const double init_t, const double final_t, const int tau,
                 const int print_progress);
