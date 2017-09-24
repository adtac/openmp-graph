#include <stdio.h>
#include <omp.h>

#include "ompdist/vector.h"
#include "ompdist/graph.h"
#include "ompdist/graph_gen.h"
#include "ompdist/queues.h"
#include "ompdist/utils.h"

#include "config.h"

typedef struct {
    int x;
    int y;
} invitation;

typedef struct {
    int leader;
    int committee;
    int min_active;
    invitation invite;
} payload;

int min(int a, int b) {
    return a < b ? a : b;
}

/**
 * min_invitation - computes and sets the lexicographically smaller invitation
 * in `a`.
 *
 * @a: one invitation
 * @b: the other invitation
 */
void min_invitation(invitation* a, invitation* b) {
    if ((b->x < a->x) || (b->x == a->x && b->y < a->y)) {
        a->x = b->x;
        a->y = b->y;
        return;
    }
}

/**
 * initialize_graph - Initializes the graph with basic data.
 *
 * @g: a pointer to the graph object
 */
void initialize_graph(graph* g) {
    invitation default_invite = { g->N, g->N };

    for (int i = 0; i < g->N; i++) {
        node* cur = elem_at(&g->vertices, i);
        payload* data = malloc(sizeof(payload));

        data->leader = i;
        data->committee = g->N+1;
        data->min_active = g->N+1;
        data->invite = default_invite;

        cur->data = data;
    }
}

/**
 * do_polling - Performs the polling phase of the k-committee algorithm
 *
 * @g:         the graph itself
 * @K:         the maximum number of elements in a committee
 * @active_ql: a pointer to a queuelist object that's going to store `min_active`s
 */
void do_polling(graph* g, int K, queuelist* active_ql) {
    DEBUG("starting polling\n");

    DEBUG("starting K-1 rounds\n");
    for (int k = 0; k < K-1; k++) {
        DEBUG("round k = %d\n", k);

        /**
         * Broadcast each node's `min_active` to its neighbors.
         */
        DEBUG("broadcasting `min_active`s\n");
        #pragma omp parallel for schedule(SCHEDULING_METHOD)
        for (int i = 0; i < g->N; i++) {
            node* cur = elem_at(&g->vertices, i);
            payload* data = cur->data;

            if (data->committee == g->N+1)
                data->min_active = i;
            else
                data->min_active = g->N+1;

            for (int j = 0; j < cur->degree; j++) {
                node* neighbor = *((node**) elem_at(&cur->neighbors, j));
                enqueue(active_ql, neighbor->label, &data->min_active);
            }
        }

        /**
         * Receive all the broadcasted `min_active`s.
         */
        DEBUG("receiving broadcasted transmissions\n");
        #pragma omp parallel for schedule(SCHEDULING_METHOD)
        for (int i = 0; i < g->N; i++) {
            node* cur = elem_at(&g->vertices, i);
            payload* data = cur->data;

            while(!is_ql_queue_empty(active_ql, i)) {
                int* active = dequeue(active_ql, i);
                data->min_active = min(data->min_active, *active);
            }

            data->leader = min(data->leader, data->min_active);
        }
    }
}

/**
 * do_selection - Performs the selection phase of the k-committee algorithm
 *
 * @g:         the graph itself
 * @K:         the maximum number of elements in a committee
 * @invite_ql: a pointer to a queuelist object that's going to store invitations
 */
void do_selection(graph* g, int K, queuelist* invite_ql) {
    DEBUG("starting selection\n");

    DEBUG("creating initial invitations\n");
    #pragma omp parallel for schedule(SCHEDULING_METHOD)
    for (int i = 0; i < g->N; i++) {
        node* cur = elem_at(&g->vertices, i);
        payload* data = cur->data;

        if (data->leader == i) {
            data->invite.x = i;
            data->invite.y = data->min_active;
        }
        else {
            data->invite.x = g->N+1;
            data->invite.y = g->N+1;
        }
    }

    DEBUG("starting K-1 rounds\n");
    for (int k = 0; k < K-1; k++) {
        DEBUG("round k = %d\n", k);

        /**
         * Broadcast invitations to neighbors.
         */
        DEBUG("broadcasting invitations\n");
        #pragma omp parallel for schedule(SCHEDULING_METHOD)
        for (int i = 0; i < g->N; i++) {
            node* cur = elem_at(&g->vertices, i);
            payload* data = cur->data;

            for (int j = 0; j < cur->degree; j++) {
                node* neighbor = *((node**) elem_at(&cur->neighbors, j));
                enqueue(invite_ql, neighbor->label, &data->invite);
            }
        }

        /**
         * Of all the invitations we've received, choose the lexicographically
         * smallest one.
         */
        DEBUG("receiving broadcasted invitations\n");
        #pragma omp parallel for schedule(SCHEDULING_METHOD)
        for (int i = 0; i < g->N; i++) {
            node* cur = elem_at(&g->vertices, i);
            payload* data = cur->data;

            while (!is_ql_queue_empty(invite_ql, i)) {
                invitation* invite = dequeue(invite_ql, i);
                min_invitation(&data->invite, invite);
            }

            // make sure the invite is for us
            if (data->invite.y == i && data->invite.x == data->leader)
                data->committee = data->leader;
        }
    }
}

/**
 * legalize_committees - A final sanity check to make sure there aren't any
 * illegal committees with IDs larger than the number of vertices.
 *
 * @g: the graph itself
 */
void legalize_committees(graph* g) {
    DEBUG("making sure there aren't any illegal committees\n");
    #pragma omp parallel for schedule(SCHEDULING_METHOD)
    for (int i = 0; i < g->N; i++) {
        node* cur = elem_at(&g->vertices, i);
        payload* data = cur->data;

        if (data->committee >= g->N)
            data->committee = i;
    }
}

/**
 * verify_and_print_solution - Verifies that the generated committee is legal
 * and prints the solution.
 *
 * @g: the graph itself
 * @K: the maximum number of elements in a committee
 */
void verify_and_print_solution(graph* g, int K) {
    int correct = 1;

    int* committee_count = malloc(g->N * sizeof(int));
    for (int i = 0; i < g->N; i++) {
        node* cur = elem_at(&g->vertices, i);
        payload* data = cur->data;

        if (data->committee >= g->N) {
            correct = 0;
            WARN("%d apparently belongs to a non-existant committee %d\n", i, data->committee);
            goto end;
        }

        committee_count[data->committee]++;
        INFO("%d->committee = %d\n", i, data->committee);
    }

    for (int i = 0; i < g->N; i++) {
        if (committee_count[i] > K) {
            WARN("committee %d has too many members (%d > %d)\n", i, committee_count[i], K);
            correct = 0;
        }
    }

end:
    free(committee_count);

    if (correct)
        INFO("Produced solution is correct\n");
    else
        INFO("Produced solution is incorrect\n");
}

/**
 * Based on Roger Wattenhofer's Principles of Distributed Computing's
 * section 23.4.2 on k-Committee election.
 */
int main(int argc, char* argv[]) {
    int N = 16;
    int M = 64;
    int K = 4;

    if (argc > 1) {
        sscanf(argv[1], "%d", &N);
        sscanf(argv[2], "%d", &M);
        sscanf(argv[3], "%d", &K);
    }

    graph* g = generate_new_connected_graph(N, M);

    initialize_graph(g);

    queuelist* active_ql = new_queuelist(N, sizeof(int));
    queuelist* invite_ql = new_queuelist(N, sizeof(invitation));

    for (int k = 0; k < K; k++) {
        DEBUG("phase k = %d\n", k);
        do_polling(g, K, active_ql);
        do_selection(g, K, invite_ql);
    }
    legalize_committees(g);

    verify_and_print_solution(g, K);

    free_queuelist(invite_ql);
    free_queuelist(active_ql);
}
