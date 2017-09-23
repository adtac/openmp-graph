#include "ompdist/graph_gen.h"
#include "ompdist/tree_gen.h"
#include "ompdist/utils.h"

#include "config.h"

int main() {
    graph* g = generate_new_tree(1000);

    free_graph(g);

    return 0;
}
