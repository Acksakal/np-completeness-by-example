/* Exam scheduling as graph coloring (C90 version, with grid visualization).
 *
 * Problem A: can the exams fit into the time slots with no student clash?
 * Reduction: exam = vertex, shared student = edge, slot = color.
 * Problem B: can the graph be colored with that many colors? (solved below)
 *
 * Build: gcc -std=c90 -pedantic -Wall -Wextra -O2 -o exam_coloring exam_coloring_grid.c
 */
#include <stdio.h>

/* ---------- Named constants instead of magic numbers ---------- */

typedef enum { BOOL_FALSE = 0, BOOL_TRUE = 1 } Bool;

/* Each exam is a vertex. EXAM_COUNT is a handy "how many" at the end. */
typedef enum {
    EXAM_MATH,
    EXAM_PHYSICS,
    EXAM_CHEM,
    EXAM_BIO,
    EXAM_HISTORY,
    EXAM_ART,
    EXAM_COUNT
} Exam;

/* Each time slot is a color. SLOT_NONE (-1) means "not assigned yet". */
typedef enum {
    SLOT_NONE = -1,
    SLOT_1,
    SLOT_2,
    SLOT_3,
    SLOT_COUNT
} Slot;

static const char *EXAM_NAMES[EXAM_COUNT] = {
    "Math", "Physics", "Chem", "Bio", "History", "Art"
};

/* ---------- The "object": data plus the functions that use it ---------- */

typedef struct {
    Bool conflict[EXAM_COUNT][EXAM_COUNT]; /* BOOL_TRUE = a student takes both */
    Slot slot[EXAM_COUNT];                 /* chosen slot for each exam        */
} Scheduler;

/* "Constructor": start with no conflicts and no slots chosen. */
static void scheduler_init(Scheduler *s)
{
    int a, b;

    for (a = 0; a < EXAM_COUNT; a++) {
        s->slot[a] = SLOT_NONE;
        for (b = 0; b < EXAM_COUNT; b++) {
            s->conflict[a][b] = BOOL_FALSE;
        }
    }
}

/* Draw the conflict[][] array as a grid.
 *   .  no conflict
 *   x  conflict added earlier
 *   *  the two cells set by the latest call: [a][b] and [b][a]
 * Rows are the first index, columns are the second index. */
static void scheduler_print_grid(const Scheduler *s, Exam a, Exam b)
{
    int row, col;
    char mark;

    printf("  Added [%s][%s] and [%s][%s]\n",
           EXAM_NAMES[a], EXAM_NAMES[b], EXAM_NAMES[b], EXAM_NAMES[a]);

    printf("  %-8s", "");
    for (col = 0; col < EXAM_COUNT; col++) {
        printf("%-4.3s", EXAM_NAMES[col]);
    }
    printf("\n");

    for (row = 0; row < EXAM_COUNT; row++) {
        printf("  %-8s", EXAM_NAMES[row]);
        for (col = 0; col < EXAM_COUNT; col++) {
            if ((row == (int)a && col == (int)b) ||
                (row == (int)b && col == (int)a)) {
                mark = '*';
            } else if (s->conflict[row][col]) {
                mark = 'x';
            } else {
                mark = '.';
            }
            printf("%-4c", mark);
        }
        printf("\n");
    }
    printf("\n");
}

/* The reduction step: "a student takes both a and b" becomes one edge. */
static void scheduler_add_conflict(Scheduler *s, Exam a, Exam b)
{
    s->conflict[a][b] = BOOL_TRUE;
    s->conflict[b][a] = BOOL_TRUE;
    scheduler_print_grid(s, a, b);
}

/* Can exam e use slot c, given exams 0..e-1 already have slots? */
static Bool scheduler_can_use(const Scheduler *s, int e, Slot c)
{
    int other;

    for (other = 0; other < e; other++) {
        if (s->conflict[e][other] && s->slot[other] == c) {
            return BOOL_FALSE;
        }
    }
    return BOOL_TRUE;
}

/* FINDING a schedule: backtracking. Worst case tries about K^N options. */
static Bool scheduler_solve_from(Scheduler *s, int e)
{
    int c;

    if (e == EXAM_COUNT) {
        return BOOL_TRUE; /* every exam placed */
    }
    for (c = SLOT_1; c < SLOT_COUNT; c++) {
        if (scheduler_can_use(s, e, (Slot)c)) {
            s->slot[e] = (Slot)c;
            if (scheduler_solve_from(s, e + 1)) {
                return BOOL_TRUE;
            }
            /* otherwise: undo and try the next slot (backtrack) */
            s->slot[e] = SLOT_NONE;
        }
    }
    return BOOL_FALSE; /* no slot works for exam e */
}

static Bool scheduler_solve(Scheduler *s)
{
    return scheduler_solve_from(s, 0);
}

/* CHECKING a schedule (the "certificate"): about N*N steps, polynomial. */
static Bool scheduler_verify(const Scheduler *s)
{
    int a, b;

    for (a = 0; a < EXAM_COUNT; a++) {
        for (b = a + 1; b < EXAM_COUNT; b++) {
            if (s->conflict[a][b] && s->slot[a] == s->slot[b]) {
                return BOOL_FALSE;
            }
        }
    }
    return BOOL_TRUE;
}

static void scheduler_print(const Scheduler *s)
{
    int c, e;

    for (c = SLOT_1; c < SLOT_COUNT; c++) {
        printf("  Slot %d:", c + 1);
        for (e = 0; e < EXAM_COUNT; e++) {
            if (s->slot[e] == (Slot)c) {
                printf(" %s", EXAM_NAMES[e]);
            }
        }
        printf("\n");
    }
}

/* ---------- Using it ---------- */

int main(void)
{
    Scheduler s;

    scheduler_init(&s);

    printf("Legend: . none   x earlier conflict   * just added\n\n");

    /* Anna: Math, Physics, Chem */
    scheduler_add_conflict(&s, EXAM_MATH, EXAM_PHYSICS);
    scheduler_add_conflict(&s, EXAM_MATH, EXAM_CHEM);
    scheduler_add_conflict(&s, EXAM_PHYSICS, EXAM_CHEM);
    /* Ben: Physics, Chem, Bio */
    scheduler_add_conflict(&s, EXAM_CHEM, EXAM_BIO);
    scheduler_add_conflict(&s, EXAM_PHYSICS, EXAM_BIO);
    /* Carla: History, Math      Dan: History, Art */
    scheduler_add_conflict(&s, EXAM_HISTORY, EXAM_MATH);
    scheduler_add_conflict(&s, EXAM_HISTORY, EXAM_ART);

    if (!scheduler_solve(&s)) {
        printf("No schedule exists with %d slots.\n", (int)SLOT_COUNT);
        return 0;
    }

    printf("Schedule found:\n");
    scheduler_print(&s);
    printf("Verified in polynomial time: %s\n",
           scheduler_verify(&s) ? "yes" : "no");
    return 0;
}