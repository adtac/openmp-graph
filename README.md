## Benchmarking Distributed Algorithms with OpenMP

In this repository, you'll find an implementation of various distributed
graph algorithms. Included are:

 - Shortest path tree (due to Bellman and Ford)
 - Shortest path tree (due to Dijkstra)
 - Byzantine consensus (due to Motwani and Raghavan)
 - Dominating set (due to Wattenhofer)
 - k-Committee (due to Wattenhofer)
 - Leader election (due to David Peleg)
 - Leader election (due to Hirschberg and Sinclair)
 - Leader election (due to Chang and Roberts)
 - Maximal independent set (due to Wattenhofer)
 - Vertex coloring (due Wattenhofer)

### Compilation

To compile the benchmark suite, `cd` into the `src` directory and do:

```bash
$ cmake .
$ make
```

This should produce a binary for each problem. These programs may take
arguments -- please see the source code for details.

### Logging

Modify the value of `LOG_LEVEL` in `utils.h` to tune the level of logging:

 - `#define LOG_LEVEL 0` - All output is suppressed.
 - `#define LOG_LEVEL 1` - Only information text is printed to screen. This
   is just the final message (whether the computed solution for each problem is
   correct or not).
 - `#define LOG_LEVEL 2` - If something went wrong, a warning message is
   printed. This includes a more detailed message on what went wrong in an
   incorrect solution. Using this automatically prints all information text too.
 - `#define LOG_LEVEL 3` - All debug statements are printed too. You probably
   don't need this. This will automatically print all information and warning
   text as well.

You'll need to recompile the entire suite whenever you change the logging level.
