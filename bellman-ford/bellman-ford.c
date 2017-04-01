#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define INF 1e8
#define INIT_ALLOC_SIZE 1024

int num_nodes, root;

char **adj_mat;

int **neighbors;

int *degree;

int **temp_msg;
int *temp_msg_size, *temp_msg_alloc;

int **msg;
int *msg_size, *msg_alloc;

int *distance, *result;

int get_diameter() {
    result = (int *)malloc(sizeof(int) * num_nodes);
    for(int i = 0; i < num_nodes; i++)
        result[i] = INF;

    int alloc_size = INIT_ALLOC_SIZE;
    int *queue = (int *)malloc(sizeof(int) * alloc_size);
    int size = 0, cur = 0;
    queue[size++] = root;

    result[root] = 0;

    int max = -1;
    
    while(cur < size) {
        int top = queue[cur++];

        for(int i = 0; i < degree[top]; i++) {
            int v = neighbors[top][i];
            if(result[v] == INF) {
                result[v] = result[top] + 1;
                if(result[v] > max)
                    max = result[v];

                while(size+1 >= alloc_size) {
                    alloc_size *= 2;
                    queue = (int *)realloc(queue, sizeof(int) * alloc_size);
                }
                queue[size++] = v;
            }
        }
    }

    return max;
}

void set_distance(int cur, int dis) {
    #pragma omp critical
    {
        while(temp_msg_size[cur]+1 >= temp_msg_alloc[cur]) {
            temp_msg_alloc[cur] *= 2;
            temp_msg[cur] = (int *)realloc(temp_msg[cur], sizeof(int) * temp_msg_alloc[cur]);
        }
        temp_msg[cur][temp_msg_size[cur]++] = dis;
    }
}

void start() {
    #pragma omp parallel for schedule(dynamic)
    for(int i = 0; i < degree[root]; i++) {
        set_distance(neighbors[root][i], distance[root]+1);
    }
}

void bfs_form() {
    #pragma omp parallel for schedule(dynamic)
    for(int i = 0; i < num_nodes; i++) {
        for(int j = 0; j < temp_msg_size[i]; j++) {
            while(msg_size[i]+1 >= msg_alloc[i]) {
                msg_alloc[i] *= 2;
                msg[i] = (int *)realloc(msg[i], sizeof(int) * msg_alloc[i]);
            }
            msg[i][msg_size[i]++] = temp_msg[i][j];
        }
        
        temp_msg_size[i] = 0;
    }

    #pragma omp parallel for schedule(dynamic)
    for(int i = 0; i < num_nodes; i++) {
        int max = -1;
        for(int j = 0; j < msg_size[i]; j++) {
            if(msg[i][j] > max)
                max = msg[i][j];
        }

        if(max > 0 && max < distance[i]) {
            distance[i] = max;
            for(int j = 0; j < degree[i]; j++)
                set_distance(neighbors[i][j], max+1);
        }

        msg_size[i] = 0;
    }
}

int main(int argc, char *argv[]) {
    if(argc < 2) {
        fprintf(stderr, "no input file given\n");
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if(f == NULL) {
        fprintf(stderr, "input file does not exist\n");
        return 1;
    }

    fscanf(f, "%d\n%d\n", &num_nodes, &root);
    root--;

    printf("allocating adj_mat\n");
    adj_mat = (char **)malloc(sizeof(char *) * num_nodes);
    for(int i = 0; i < num_nodes; i++)
        adj_mat[i] = (char *)malloc(sizeof(char) * num_nodes);

    printf("allocating neighbors\n");
    neighbors = (int **)malloc(sizeof(int *) * num_nodes);
    for(int i = 0; i < num_nodes; i++)
        neighbors[i] = (int *)malloc(sizeof(int) * num_nodes);

    printf("allocating degree\n");
    degree = (int *)malloc(sizeof(int) * num_nodes);
    for(int i = 0; i < num_nodes; i++)
        degree[i] = 0;

    printf("allocating temp_msg stuff\n");
    temp_msg = (int **)malloc(sizeof(int *) * num_nodes);
    temp_msg_alloc = (int *)malloc(sizeof(int) * num_nodes);
    temp_msg_size = (int *)malloc(sizeof(int) * num_nodes);
    for(int i = 0; i < num_nodes; i++) {
        temp_msg_alloc[i] = INIT_ALLOC_SIZE;
        temp_msg_size[i] = 0;
        temp_msg[i] = (int *)malloc(sizeof(int) * temp_msg_alloc[i]);
    }

    printf("allocating msg stuff\n");
    msg = (int **)malloc(sizeof(int *) * num_nodes);
    msg_alloc = (int *)malloc(sizeof(int) * num_nodes);
    msg_size = (int *)malloc(sizeof(int) * num_nodes);
    for(int i = 0; i < num_nodes; i++) {
        msg_alloc[i] = INIT_ALLOC_SIZE;
        msg_size[i] = 0;
        msg[i] = (int *)malloc(sizeof(int) * msg_alloc[i]);
    }

    printf("allocating distance\n");
    distance = (int *)malloc(sizeof(int) * num_nodes);
    for(int i = 0; i < num_nodes; i++)
        distance[i] = INF;
    distance[root] = 0;

    printf("reading graph\n");
    char *str = (char *)malloc(sizeof(char) * (num_nodes+1));
    for(int i = 0; i < num_nodes; i++) {
        fscanf(f, "%s", str);
        for(int j = 0; j < num_nodes; j++) {
            if(adj_mat[i][j] = str[j] - '0')
                neighbors[i][degree[i]++] = j;
        }
    }
    free(str);

    printf("calculating diameter\n");
    int diameter = get_diameter();
    printf("diameter = %d\n", diameter);

    printf("starting\n");
    start();
    
    for(int i = 0; i < diameter; i++)
        bfs_form();
    printf("done\n");

    int correct = 1;
    for(int i = 0; i < num_nodes; i++) {
        if(result[i] != distance[i])
            correct = 0;
    }
    if(correct)
        printf("solution verified to be correct\n");
    else
        printf("solution incorrect\n");
}
