// Change these only if using different cube size
#define CUBE_SIZE         3
#define CELL_COUNT        CUBE_SIZE * CUBE_SIZE
#define SIDE_COUNT        6
#define GENE_SIZE         4
#define CELL_STR          "  "

// Sampling types (do not change)
#define ROULETTE          1
#define STOHASTIC         2
#define REM_STOHASTIC     3

// Display
#define COLOR_CELLS       1
#define DRAW_EVERY        200

// Play with these
#define SAMPLING_TYPE     ROULETTE
#define MUTATE            1
#define CROSSOVER         1
#define IMMEDIATE_REPOP   1
#define CROSS_BONUS       0
#define CHROMO_LENGTH     20
#define MAX_GENERATIONS   10000
#define POPULATION_SIZE   1000
#define SOLUTION_REWARD   100000
#define MUTATION_RATE     0.024f * GENE_SIZE
#define CROSSOVER_RATE    0.64f
#define REWARD_ACTION     +
#define REWARD_MULT       1
