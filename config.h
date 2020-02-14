#pragma once

#define MUTATE            1
#define CROSSOVER         1
#define IMMEDIATE_REPOP   1
#define INTERMEDIATE_POP  0
#define STOHASTIC_SAMPL   0
#define AVG_FITNESS       0

#define CUBE_SIZE         3
#define CELL_COUNT        CUBE_SIZE * CUBE_SIZE
#define SIDE_COUNT        6
#define GENE_SIZE         4
#define CHROMO_LENGTH     30
#define MAX_GENERATIONS   100000
#define POPULATION_SIZE   1100
#define CELL_STR          "  "
#define COLOR_CELLS       1
#define SOLUTION_REWARD   100000
#define MUTATION_RATE     0.005f * GENE_SIZE
#define CROSSOVER_RATE    0.3f
#define REWARD_ACTION     +
#define REWARD_MULT       1
#define DRAW_EVERY        2000
