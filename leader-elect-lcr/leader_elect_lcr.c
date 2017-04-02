#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int num_processes;
int* id;
int* received;
int* leader;
int* send;
int* status;

void send_msg(int i, int val) {
    send[i] = val;
}

void elect() {
    #pragma omp parallel for schedule(dynamic)
    for(int i = 0; i < num_processes; i++) {
        int next = (i+1) % num_processes;
        int send_val = send[i];
        send_msg(next, send_val);
    }

    #pragma omp parallel for schedule(dynamic)
    for(int i = 0; i < num_processes; i++) {
        if(received[i] > leader[i]) {
            leader[i] = received[i];
            send[i] = received[i];
        } else if(received[i] == id[i]) {
            status[i] = 1;
            leader[i] = id[i];
        }
    }
}

void transmit_leaders() {
    int elected_leader = 0;
    for(int i = 0; i < num_processes; i++) {
        if(status[i])
            elected_leader = i;
    }
    for(int i = 0; i < num_processes; i++) {
        leader[i] = elected_leader;
    }
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        fprintf(stderr, "no input file given\n");
        return 1;
    }

    FILE* f = fopen(argv[1], "r");
    if(f == NULL) {
        fprintf(stderr, "input file does not exist\n");
        return 1;
    }

    fscanf(f, "%d", &num_processes);

    printf("allocating stuff\n");
    id = (int*)malloc(sizeof(int) * num_processes);
    received = (int*)malloc(sizeof(int) * num_processes);
    leader = (int*)malloc(sizeof(int) * num_processes);
    send = (int*)malloc(sizeof(int) * num_processes);
    status = (int*)malloc(sizeof(int) * num_processes);

    int max = -1;
    printf("reading IDs\n");
    for(int i = 0; i < num_processes; i++) {
        fscanf(f, "%d", &id[i]);
        id[i]--;

        received[i] = id[i];
        send[i] = id[i];
        leader[i] = id[i];
        status[i] = 0;

        if(max < id[i])
            max = id[i];
    }

    printf("starting\n");
    for(int i = 0; i < num_processes; i++)
        elect();

    printf("transmitting leaders\n");
    transmit_leaders();

    if(leader[0] == max)
        printf("solution verified to be correct\n");
    else {
        for(int i = 0; i < num_processes; i++)
            printf("leader[%d] = %d\n", i, leader[i]);
        printf("incorrect solution\n");
    }
}
