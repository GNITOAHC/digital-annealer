#include <stdio.h>
#include <stdlib.h>

#include "../include/argp.h"
#include "../include/color.h"
#include "./graph/annealer.h"
#include "./graph/graph.h"

#define MAX_LINE_LENGTH 100

option_t options[] = {
    {           "help",       no_argument, NULL, 'h',     NULL,                  "Display this information" },
    {        "version",       no_argument, NULL, 'v',     NULL,                           "Display version" },
    {           "qubo",       no_argument, NULL, 'q',     NULL,          "Specify that the graph is a QUBO" },
    {           "file", required_argument, NULL, 'f',   "FILE", "Path to the input hamiltonian description" },
    {      "spin-conf", required_argument, NULL, 's',   "FILE",          "Path to the initial spin configs" },
    {         "init-t", required_argument, NULL, 'i', "DOUBLE",      "Specify an initial temperature value" },
    {        "final-t", required_argument, NULL, 'I', "DOUBLE",        "Specify an final temperature value" },
    {            "tau", required_argument, NULL, 'u',    "TAU",                "Specify a tau for annealer" },
    {     "print-conf",       no_argument, NULL, 'p',     NULL,                  "Output the configuration" },
    { "print-progress",       no_argument, NULL, 'P',     NULL,              "Print the annealing progress" },
    {      "ans-count", required_argument, NULL, 'a',    "INT",             "Specify the number of answers" },
    {             NULL,                 0, NULL,   0,     NULL,                                        NULL },
};

typedef struct {
    int qubo;
    char *file, *spin_conf;
    double init_t, final_t;
    int tau;
    int print_conf, print_progress, ans_count;
} args_t;

short read_input_file(graph_t *graph, FILE *fp);
short read_input_qubo_file(graph_t *graph, FILE *fp);

int main (int argc, char **argv) {
    graph_t *graph = new_graph();
    if (graph == NULL) {
        fprintf(stderr, "Failed to allocate memory for the graph\n");
        return 1;
    }

    args_t args = { 0, "input.dat", "spin-conf.dat", 2.0, 0.0, 1000, 0, 0, 1 };

    int opt = 0, opt_index = 0;
    int valid = 1;

    struct option *long_options = long_opts(options);
    char *short_options         = short_opts(options);
    char *help_message          = help_mes(options);
    char *version_message       = "0.1.0";

    while ((opt = getopt_long(argc, argv, short_options, long_options, &opt_index)) != -1) {
        switch (opt) {
            case 'h': printf("%s\n", help_message); exit(1);
            case 'v': printf("Version %s\n", version_message); exit(1);
            case 'q': args.qubo = 1; break;
            case 'f': args.file = optarg; break;
            case 's': args.spin_conf = optarg; break;
            case 'i': args.init_t = atof(optarg); break;
            case 'I': args.final_t = atof(optarg); break;
            case 'u': args.tau = atoi(optarg); break;
            case 'p': args.print_conf = 1; break;
            case 'P': args.print_progress = 1; break;
            case 'a': args.ans_count = atoi(optarg); break;
            case '?': valid = 0; break;
            default: printf("opt = %d\n", opt); break;
        }
    }
    if (!valid) {
        fprintf(stderr, RED "Invalid option\n");
        exit(1);
    }
    free_options(long_options, short_options, help_message);

    FILE *fp = fopen(args.file, "r");
    if (fp == NULL) {
        perror("fopen");
        return 1;
    }
    read_input_file(graph, fp);
    fclose(fp);

    printf("Hamiltonian Energy: %f\n", sa_hamiltonian_energy(graph));
    printf("Simulated Annealing\n");
    sa_anneal(graph, args.init_t, args.final_t, args.tau, args.print_progress);
    printf("Energy: %f\n", sa_hamiltonian_energy(graph));

    free_graph(graph);

    return 0;
}

short read_input_file (graph_t *graph, FILE *fp) {
    char line[MAX_LINE_LENGTH] = { 0 };
    double po1, po2, co;

    while (fp != NULL && fgets(line, 10000, fp)) {
        int count = sscanf(line, "%lf %lf %lf", &po1, &po2, &co);

        switch (count) {
            case 3: sa_push_back_edge(graph, (int)po1, (int)po2, co); break;
            default: break;
        }
    }

    return 1;
}

short read_input_qubo_file (graph_t *graph, FILE *fp) {
    return 1;
}
