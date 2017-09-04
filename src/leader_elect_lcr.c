#include <stdio.h>
#include <omp.h>

#include "ompdist/election.h"

/**
 * Uses the distributed leader elect algorithm due to Chang and Roberts (1979)
 * that is used to determine the leader of a ring of nodes connected to one
 * another.
 *
 * Links:
 *   - https://en.wikipedia.org/wiki/Chang_and_Roberts_algorithm
 *   - http://www.cse.iitm.ac.in/~augustine/cs6100_even2012/slides/13_LeaderElection.pdf
 */
int main(int argc, char* argv[]) {
    int N = 16;
    if (argc > 1)
        sscanf(argv[1], "%d", &N);

    process* processes = generate_nodes(N);

    for (int i = 0; i < N; i++) {
#pragma omp parallel for schedule(dynamic)
        for (int i = 0; i < N; i++) {
            int next = (i+1) % N;
            processes[next].received = processes[i].send;
        }

#pragma omp parallel for schedule(dynamic)
        for (int i = 0; i < N; i++) {
            if (processes[i].received > processes[i].leader) {
                processes[i].send = processes[i].received;
                processes[i].leader = processes[i].received;
            }
            else if (processes[i].received == processes[i].id) {
                processes[i].leader = processes[i].id;
                processes[i].status = 1;
            }
        }
    }

    int chosen_id = -1;
    for (int i = 0; i < N; i++) {
        if (processes[i].status == 1) {
            chosen_id = i;
            break;
        }
    }

    if (chosen_id == -1) {
        printf("Incorrect: no solution found.\n");
        return 1;
    }

    for (int i = 0; i < N; i++)
        processes[i].leader = chosen_id;

    printf("Chosen leader: %d\n", chosen_id);

    return 0;
}
