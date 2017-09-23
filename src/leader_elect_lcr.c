#include <stdio.h>
#include <omp.h>

#include "ompdist/election.h"
#include "ompdist/utils.h"

#include "config.h"

/**
 * receive_leaders - Sets the next process' received leader to be the current
 * process' candidate.
 *
 * @processes: the list of processes
 * @n:         number of processes
 */
void receive_leaders(process* processes, int N) {
    #pragma omp parallel for schedule(SCHEDULING_METHOD)
    for (int i = 0; i < N; i++) {
        int next = (i+1) % N;
        processes[next].received = processes[i].send;
    }
}

/**
 * determine_leaders - Based on the received ID of potential leaders, decide
 * whether to propagate or not.
 *
 * @processes: the list of processes
 * @n:         number of processes
 */
void determine_leaders(process* processes, int N) {
    #pragma omp parallel for schedule(SCHEDULING_METHOD)
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

/**
 * identify_leader - Identifies a leader for the processes by looking at each
 * process' status field.
 *
 * @processes: the list of processes
 * @N:         number of processes
 *
 * Returns `chosen_id`, the id of the chosen leader.
 */
int identify_leader(process* processes, int N) {
    int chosen_id = -1;
    for (int i = 0; i < N; i++) {
        if (processes[i].status == 1) {
            chosen_id = i;
            break;
        }
    }

    return chosen_id;
}

/**
 * set_leader - Sets the leader for all processes to be `chosen_id`
 *
 * @processes: the list of processes
 * @N:         number of processes
 * @chosen_id: the finally chosen leader
 */
void set_leader(process* processes, int N, int chosen_id) {
    for (int i = 0; i < N; i++)
        processes[i].leader = chosen_id;
}

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
        receive_leaders(processes, N);
        determine_leaders(processes, N);
    }

    int chosen_id = identify_leader(processes, N);
    if (chosen_id == -1) {
        INFO("Incorrect: no solution found.\n");
        return 1;
    }

    set_leader(processes, N, chosen_id);
    INFO("Chosen leader: %d\n", chosen_id);

    return 0;
}
