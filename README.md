## Benchmarking Distributed Algorithms with OpenMP

In this repository, you'll find the implementation of various distributed
graph algorithms. Included are:

 - Shortest path tree (due to Bellman and Ford)
 - Shortest path tree (due to Dijkstra)
 - Byzantine consensus
 - Dominating set
 - k-Committee
 - Leader election (due to David Peleg)
 - Leader election (due to Hirschberg and Sinclair)
 - Leader election (due to Chang and Roberts)
 - Maximal independent set

### Compilation

To compile the benchmark suite, `cd` into the `src` directory and do:

```bash
$ cmake .
$ make
```

This should produce a binary for each problem. These programs may take
arguments -- please see the source code for details.
