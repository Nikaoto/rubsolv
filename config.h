#pragma once

// OK: inter, mut 0.02, crossov 0.7
// GOOD: imm, mut 0.02, crossov 0.7
// BETTER: imm, mut 0.024, crossov 0.62
// BETTER: imm, mut 0.0245, crossov 0.618
// BETTER: imm, mut 0.024, crossov 0.64
// OK: mut 0.002, crossov 0.3
// inter, mut 0.02, crossov 0.3

#define MUTATE            1
#define CROSSOVER         1
#define IMMEDIATE_REPOP   1
#define INTERMEDIATE_POP  0
#define STOHASTIC_SAMPL   0
#define AVG_FITNESS       0
#define CROSS_BONUS       0

#define CUBE_SIZE         3
#define CELL_COUNT        CUBE_SIZE * CUBE_SIZE
#define SIDE_COUNT        6
#define GENE_SIZE         4
#define CHROMO_LENGTH     20
#define MAX_GENERATIONS   10000
#define POPULATION_SIZE   1500
#define CELL_STR          "  "
#define COLOR_CELLS       1
#define SOLUTION_REWARD   100000
#define MUTATION_RATE     0.024f * GENE_SIZE
#define CROSSOVER_RATE    0.62f
#define REWARD_ACTION     +
#define REWARD_MULT       1
#define DRAW_EVERY        200
