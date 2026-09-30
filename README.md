# np-completeness-by-example

Small programs and an animation to help me remember what NP-completeness means, using one concrete example: **scheduling exams as graph coloring**.

## The idea in five lines

- **Decision problem:** a yes/no question about an input.
- **P:** answers can be *found* in polynomial time. **NP:** answers can be *checked* in polynomial time.
- **Certificate:** the proof of a "yes" that can be checked quickly (here: a slot for every exam).
- **Reduction:** turn problem A into problem B so a solver for B answers A. If B is easy, A is easy. To show a new problem is hard, reduce a known hard one *to* it.
- **NP-complete:** in NP, and every NP problem reduces to it. Cook (1971) showed this for satisfiability (SAT).

## The example

| Scheduling world | Graph world |
|---|---|
| Exam | Dot (vertex) |
| Two exams share a student | Line (edge) between the dots |
| Time slot | Color |
| No student has two exams at once | No line joins two dots of the same color |

Question: can the exams be scheduled into K slots? Same question: can the graph be colored with K colors?

- Finding a coloring: backtracking, worst case about K^N tries (exponential).
- Checking a coloring: about N² steps (polynomial).

## Files

| File | What it is |
|---|---|
| `exam_coloring.c` | The C solver: strict C90, enums and a struct, prints the conflict grid after each added conflict |
| `exam_backtracking.html` | Animation: grid, graph, exam list and a Big-O chart. Open in a browser. |

The HTML file orders the exams Bio, History, Math, Physics, Chem, Art so that the solver actually has to backtrack. The C file uses the original order, where it never does.

## Run

```
gcc -std=c90 -pedantic -Wall -Wextra -O2 -o exam_coloring exam_coloring.c
./exam_coloring
```

## Things to remember

- The adjacency grid is symmetric: an edge sets both `[a][b]` and `[b][a]`. How the graph is stored is an implementation choice:

  | Storage | Description | Big-O (N exams, E conflicts) |
  |---|---|---|
  | Adjacency matrix (the grid used here) | N by N table of true/false; instant lookup, easy to print and inspect | Space O(N²), "do a and b conflict?" O(1) |
  | Adjacency list | Each exam keeps a list of its conflicting exams; stores only real conflicts, so it suits large sparse graphs | Space O(N + E), lookup O(N) worst case (one exam's list) |
  | Edge list | Just the list of conflicting pairs; simplest form, easy to build from input data | Space O(E), lookup O(E) (scan every pair) |

  The backtracking algorithm stays the same for all three. Only the "do these two conflict?" check changes.
- Reduction is not the same as splitting a problem into parts. It re-expresses the whole problem as another one.
- NP-hard does not mean "exponential". It means no known polynomial algorithm, and finding one would settle P vs NP.
- If a problem looks NP-complete: use approximation, heuristics, special structure, or a solver.

## Source

Stephen A. Cook, *The Complexity of Theorem-Proving Procedures*, Proc. 3rd ACM Symposium on Theory of Computing, 1971.