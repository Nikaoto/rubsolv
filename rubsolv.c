/*
  rubsolv by Nika Otiashvili
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "config.h"
#include "colors.h"

#if POPULATION_SIZE % 2 == 1
#error "POPULATION_SIZE must be even"
#endif

#if (SAMPLING_TYPE == STOHASTIC || SAMPLING_TYPE == REM_STOHASTIC) && IMMEDIATE_REPOP == 1
#error "Intermediate population needed for given sampling type. Can't repopulate immediately"
#endif

typedef unsigned char uchar;

// sides.h
uchar sides[SIDE_COUNT][CELL_COUNT];
uchar solved_sides[SIDE_COUNT][CELL_COUNT];
//

// Regions
int NORTH_REGION[] = {0, 1, 2};
int SOUTH_REGION[] = {6, 7, 8};
int EAST_REGION[]  = {2, 5, 8};
int WEST_REGION[]  = {0, 3, 6};
int REV_NORTH_REGION[] = {2, 1, 0};
int REV_SOUTH_REGION[] = {8, 7, 6};
int REV_EAST_REGION[]  = {8, 5, 2};
int REV_WEST_REGION[]  = {6, 3, 0};

// Genes / actions
enum GENE_TYPE {
    NORTH_LEFT,     // 0x0 Rotate top side clockwise
    NORTH_RIGHT,    // 0x1 Rotate top side counter-clockwise
    EAST_UP,        // 0x2 Rotate right side upwards
    EAST_DOWN,      // 0x3 Rotate right side downwards
    SOUTH_LEFT,     // 0x4 Rotate bottom side clockwise
    SOUTH_RIGHT,    // 0x5 Rotate bottom side counter-clockwise
    WEST_UP,        // 0x6 Rotate left side upwards
    WEST_DOWN,      // 0x7 Rotate left side downwards
    FRONT_LEFT,     // 0x8 Rotate front side counter-clockwise
    FRONT_RIGHT,    // 0x9 Rotate front side clockwise
    BACK_LEFT,      // 0xA Rotate back side counter-clockwise
    BACK_RIGHT,     // 0xB Rotate back side clockwise
};

// Returns unbiased random integer in range [0, n)
int randint(int n)
{
    if (n - 1 == RAND_MAX) {
        return rand();
    }

    int r;
    int rand_end = (RAND_MAX / n) * n;
    while ((r = rand()) >= rand_end);
    return r % n;
}

// Returns random float in range [0.0, 1.0)
float randscale()
{
    int r;
    while ((r = rand()) == RAND_MAX);
    return (float) r / RAND_MAX;
}

uchar get_random_gene()
{
    return randint(1 << GENE_SIZE);
}

void random_chromo(uchar chromo[])
{
    for (size_t i = 0; i < CHROMO_LENGTH; i++) {
        chromo[i] = get_random_gene();
    }
}

void copy_chromo(uchar to[], uchar from[])
{
    for (int i = 0; i < CHROMO_LENGTH; i++) {
        to[i] = from[i];
    }
}

void print_chromo(uchar chromo[])
{
    size_t i = 0;
    while (i < CHROMO_LENGTH - 1) {
        printf("%x,", chromo[i++]);
    }
    printf("%x", chromo[i]);
}

#if CROSSOVER == 1
void crossover(uchar parent1[], uchar parent2[], uchar child1[], uchar child2[])
{
    if (randscale() > CROSSOVER_RATE) {
        return;
    }

    int crossover_point = randint(CHROMO_LENGTH);

    // Copy first part
    for (int i = 0; i < crossover_point; i++) {
        child1[i] = parent2[i];
        child2[i] = parent1[i];
    }

    // Copy second part
    for (int i = crossover_point; i < CHROMO_LENGTH; i++) {
        child1[i] = parent1[i];
        child2[i] = parent2[i];
    }
}
#else
void crossover(uchar parent1[], uchar parent2[], uchar child1[], uchar child2[])
{
    copy_chromo(child1, parent1);
    copy_chromo(child2, parent2);
}
#endif

#if MUTATE == 1
void mutate(uchar chromo[])
{
    for (int i = 0; i < CHROMO_LENGTH; i++) {
        if (randscale() > MUTATION_RATE) {
            continue;
        }

        // Flip nth bit in gene
        uchar n = randint(GENE_SIZE);
        uchar mask = 1 << n;
        chromo[i] = (~chromo[i] & mask) | (chromo[i] & ~mask);
    }
}
#else
void mutate(uchar chromo[]) {}
#endif

#if COLOR_CELLS == 1
void print_cell(uchar cell)
{
    switch(cell) {
    case 'r': printf("%s%s%s", RED, CELL_STR, END); break;
    case 'b': printf("%s%s%s", BLUE, CELL_STR, END); break;
    case 'g': printf("%s%s%s", GREEN, CELL_STR, END); break;
    case 'y': printf("%s%s%s", YELLOW, CELL_STR, END); break;
    case 'o': printf("%s%s%s", ORANGE, CELL_STR, END); break;
    case 'w': printf("%s%s%s", WHITE, CELL_STR, END); break;
    default: printf("%c ", cell); break;
    }
}
#else
inline void print_cell(uchar cell)
{
    printf("%c ", cell);
}
#endif

void draw_cube(uchar sides[SIDE_COUNT][CELL_COUNT])
{
    // Draw side 2
    for (size_t x = 0; x < CUBE_SIZE; x++) {
        // Indent
        for (size_t i = 0; i < CUBE_SIZE; i++)
            printf(CELL_STR);
        // Row contents
        for (size_t i = 0; i < CUBE_SIZE; i++) {
            print_cell(sides[2][x * CUBE_SIZE + i]);
        }
        printf("\n");
    }

    // Draw each of the sides 1, 0, 4 and 5 row by row
    size_t side_i[SIDE_COUNT - 2] = {1, 0, 4, 5};
    for (size_t j = 0; j < CUBE_SIZE; j++) {
        for (size_t x = 0; x < SIDE_COUNT-2; x++) {
            for (size_t i = 0; i < CUBE_SIZE; i++) {
                print_cell(sides[side_i[x]][x / (SIDE_COUNT-2) + i + j * CUBE_SIZE]);
            }
        }
        printf("\n");
    }

    // Draw side 3
    for (size_t x = 0; x < CUBE_SIZE; x++) {
        // Indent
        for (size_t i = 0; i < CUBE_SIZE; i++)
            printf(CELL_STR);
        // Row contents
        for (size_t i = 0; i < CUBE_SIZE; i++) {
            print_cell(sides[3][x * CUBE_SIZE + i]);
        }
        printf("\n");
    }
}

// Flip across diagonal which resembles backslash (\)
void flip_side_diag_back(uchar cells[])
{
    int i, j = 0;
    int compl_k; // index of cell with which k is swapped
    uchar tmp; // temp storage
    for (int k = 0; k < CELL_COUNT; k++) {
        i = k / CUBE_SIZE;
        j = k % CUBE_SIZE;
        if (j < i)
            continue;
        compl_k = i + j * CUBE_SIZE;
        tmp = cells[k];
        cells[k] = cells[compl_k];
        cells[compl_k] = tmp;
    }
}

// Flip across diagonal which resembles forward slash (/)
void flip_side_diag_forw(uchar cells[])
{
    int i, j = 0;
    int compl_k; // index of cell with which k is swapped
    uchar tmp; // temp storage
    for (int k = 0; k < CELL_COUNT; k++) {
        i = k / CUBE_SIZE;
        j = k % CUBE_SIZE;
        // -1 for strict ">"
        // another -1 because j can be size-1 at max
        if (j > CUBE_SIZE - i - 2)
            continue;
        compl_k = (CUBE_SIZE - i - 1) + (CUBE_SIZE - j - 1) * CUBE_SIZE;
        tmp = cells[k];
        cells[k] = cells[compl_k];
        cells[compl_k] = tmp;
    }
}

// Flip across horizontal line at the center
void flip_side_horiz(uchar cells[])
{
    const int half = CUBE_SIZE / 2;
    int i, j = 0;
    int compl_k;
    uchar tmp;
    for (int k = 0; k < CUBE_SIZE; k++) {
        i = k / CUBE_SIZE;
        if (i == half)
            continue;
        j = k % CUBE_SIZE;
        compl_k = (CUBE_SIZE - i - 1) * CUBE_SIZE + j;
        tmp = cells[k];
        cells[k] = cells[compl_k];
        cells[compl_k] = tmp;
    }
}

void rotate_side_right(uchar cells[CELL_COUNT])
{
    flip_side_diag_forw(cells);
    flip_side_horiz(cells);
}

void rotate_side_left(uchar cells[])
{
    flip_side_diag_back(cells);
    flip_side_horiz(cells);
}

void copy_side(uchar to[], uchar from[])
{
    for (int i = 0; i < CELL_COUNT; i++)
        to[i] = from[i];
}

void copy_all_sides(uchar to[][CELL_COUNT], uchar from[][CELL_COUNT])
{
    for (int i = 0; i < SIDE_COUNT; i++)
        copy_side(to[i], from[i]);
}

void copy_side_region(uchar to[], int to_indices[], uchar from[], int from_indices[])
{
    for (int i = 0; i < CUBE_SIZE; i++) {
        to[to_indices[i]] = from[from_indices[i]];
    }
}

void swap_regions(uchar side1[], int side1_region[], uchar side2[], int side2_region[])
{
    uchar tmp_side[CELL_COUNT];
    copy_side_region(tmp_side, side1_region, side1, side1_region);
    copy_side_region(side1, side1_region, side2, side2_region);
    copy_side_region(side2, side2_region, tmp_side, side1_region);
}

void north_right(uchar sides[][CELL_COUNT])
{
    swap_regions(sides[5], NORTH_REGION, sides[4], NORTH_REGION);
    swap_regions(sides[4], NORTH_REGION, sides[0], NORTH_REGION);
    swap_regions(sides[0], NORTH_REGION, sides[1], NORTH_REGION);
    rotate_side_left(sides[2]);
}

void north_left(uchar sides[][CELL_COUNT])
{
    swap_regions(sides[1], NORTH_REGION, sides[0], NORTH_REGION);
    swap_regions(sides[0], NORTH_REGION, sides[4], NORTH_REGION);
    swap_regions(sides[4], NORTH_REGION, sides[5], NORTH_REGION);
    rotate_side_right(sides[2]);
}

void south_right(uchar sides[][CELL_COUNT])
{
    swap_regions(sides[5], SOUTH_REGION, sides[4], SOUTH_REGION);
    swap_regions(sides[4], SOUTH_REGION, sides[0], SOUTH_REGION);
    swap_regions(sides[0], SOUTH_REGION, sides[1], SOUTH_REGION);
    rotate_side_right(sides[3]);
}

void south_left(uchar sides[][CELL_COUNT])
{
    swap_regions(sides[1], SOUTH_REGION, sides[0], SOUTH_REGION);
    swap_regions(sides[0], SOUTH_REGION, sides[4], SOUTH_REGION);
    swap_regions(sides[4], SOUTH_REGION, sides[5], SOUTH_REGION);
    rotate_side_left(sides[3]);
}

void east_down(uchar sides[][CELL_COUNT])
{
    swap_regions(sides[2], EAST_REGION, sides[0], EAST_REGION);
    swap_regions(sides[2], EAST_REGION, sides[3], EAST_REGION);
    swap_regions(sides[2], EAST_REGION, sides[5], REV_WEST_REGION);
    rotate_side_left(sides[4]);
}

void east_up(uchar sides[][CELL_COUNT])
{
    swap_regions(sides[2], EAST_REGION, sides[3], EAST_REGION);
    swap_regions(sides[2], EAST_REGION, sides[0], EAST_REGION);
    swap_regions(sides[3], EAST_REGION, sides[5], REV_WEST_REGION);
    rotate_side_right(sides[4]);
}

void west_down(uchar sides[][CELL_COUNT])
{
    swap_regions(sides[2], WEST_REGION, sides[0], WEST_REGION);
    swap_regions(sides[2], WEST_REGION, sides[3], WEST_REGION);
    swap_regions(sides[2], WEST_REGION, sides[5], REV_EAST_REGION);
    rotate_side_right(sides[1]);
}

void west_up(uchar sides[][CELL_COUNT])
{
    swap_regions(sides[2], WEST_REGION, sides[3], WEST_REGION);
    swap_regions(sides[2], WEST_REGION, sides[0], WEST_REGION);
    swap_regions(sides[3], WEST_REGION, sides[5], REV_EAST_REGION);
    rotate_side_left(sides[1]);
}

void front_left(uchar sides[][CELL_COUNT])
{
    uchar tmp_side[CELL_COUNT];
    copy_side_region(tmp_side, EAST_REGION, sides[1], EAST_REGION); // 1->tmp
    copy_side_region(sides[1], REV_EAST_REGION, sides[2], SOUTH_REGION); // 2->1
    copy_side_region(sides[2], SOUTH_REGION, sides[4], WEST_REGION); // 4->2
    copy_side_region(sides[4], REV_WEST_REGION, sides[3], NORTH_REGION); // 3->4
    copy_side_region(sides[3], NORTH_REGION, tmp_side, EAST_REGION); // tmp->3
    rotate_side_left(sides[0]);
}

void front_right(uchar sides[][CELL_COUNT])
{
    uchar tmp_side[CELL_COUNT];
    copy_side_region(tmp_side, EAST_REGION, sides[1], EAST_REGION); // 1->tmp
    copy_side_region(sides[1], EAST_REGION, sides[3], NORTH_REGION); // 3->1
    copy_side_region(sides[3], REV_NORTH_REGION, sides[4], WEST_REGION); // 4->3
    copy_side_region(sides[4], WEST_REGION, sides[2], SOUTH_REGION); // 2->4
    copy_side_region(sides[2], REV_SOUTH_REGION, tmp_side, EAST_REGION); // tmp->2
    rotate_side_right(sides[0]);
}

// With side 0 facing the camera
void back_right(uchar sides[][CELL_COUNT])
{
    uchar tmp_side[CELL_COUNT];
    copy_side_region(tmp_side, WEST_REGION, sides[1], WEST_REGION); // 1->tmp
    copy_side_region(sides[1], WEST_REGION, sides[3], SOUTH_REGION); // 3->1
    copy_side_region(sides[3], SOUTH_REGION, sides[4], REV_EAST_REGION); // 4->3
    copy_side_region(sides[4], EAST_REGION, sides[2], NORTH_REGION); // 2->4
    copy_side_region(sides[2], REV_NORTH_REGION, tmp_side, WEST_REGION); // tmp->2
    rotate_side_left(sides[5]);
}

// With side 0 facing the camera
void back_left(uchar sides[][CELL_COUNT])
{
    uchar tmp_side[CELL_COUNT];
    copy_side_region(tmp_side, WEST_REGION, sides[1], WEST_REGION); // 1->tmp
    copy_side_region(sides[1], REV_WEST_REGION, sides[2], NORTH_REGION); // 2->1
    copy_side_region(sides[2], NORTH_REGION, sides[4], EAST_REGION); // 4->2
    copy_side_region(sides[4], EAST_REGION, sides[3], REV_SOUTH_REGION); // 3->4
    copy_side_region(sides[3], SOUTH_REGION, tmp_side, WEST_REGION); // tmp->3
    rotate_side_right(sides[5]);
}

int are_sides_same(uchar sides1[][CELL_COUNT], uchar sides2[][CELL_COUNT])
{
    for (int i = 0; i < SIDE_COUNT; i++) {
        for (int j = 0; j < CELL_COUNT; j++) {
            if (sides1[i][j] != sides2[i][j])
                return 0;
        }
    }
    return 1;
}

// Returns 0 if no action applied, 1 if applied
int apply_action(uchar gene, uchar sides[][CELL_COUNT])
{
    switch(gene) {
    case NORTH_LEFT:  north_left(sides); break;
    case NORTH_RIGHT: north_right(sides); break;
    case SOUTH_LEFT:  south_left(sides); break;
    case SOUTH_RIGHT: south_right(sides); break;
    case EAST_UP:     east_up(sides); break;
    case EAST_DOWN:   east_down(sides); break;
    case WEST_UP:     west_up(sides); break;
    case WEST_DOWN:   west_down(sides); break;
    case FRONT_LEFT:  front_left(sides); break;
    case FRONT_RIGHT: front_right(sides); break;
    case BACK_LEFT:   back_left(sides); break;
    case BACK_RIGHT:  back_right(sides); break;
    default: return 0;
    }

    return 1;
}

int get_similarity_score(uchar sides1[][CELL_COUNT], uchar sides2[][CELL_COUNT])
{
    int score = 1;
#if CROSS_BONUS > 0
    int cross = 0;
#endif
    for (int s = 0; s < SIDE_COUNT; s++) {
        for (int k = 0; k < CELL_COUNT; k++) {
            if (k != 4 && sides1[s][k] == sides2[s][k]) {
                score = score REWARD_ACTION REWARD_MULT;
            }
#if CROSS_BONUS > 0
            if (k % 2 == 1) {
                if (sides1[s][k] == sides2[s][4])
                    cross += 1;
                else
                    cross = 0;
            }
#endif
        }
    }

#if CROSS_BONUS > 0
    if (cross > 3) {
        score += CROSS_BONUS;
    }
#endif
    return score;
}

int calculate_fitness(uchar chromo[], uchar sides[][CELL_COUNT],
                      uchar desired_sides[][CELL_COUNT])
{
    uchar tmp_sides[SIDE_COUNT][CELL_COUNT];
    copy_all_sides(tmp_sides, sides);
    for (int i = 0; i < CHROMO_LENGTH; i++) {
        int applied = apply_action(chromo[i], tmp_sides);
        if (applied && are_sides_same(tmp_sides, solved_sides))
            return SOLUTION_REWARD;
    }
    return get_similarity_score(tmp_sides, solved_sides);
}

// Stohastic sampling roulette
void ss_roulette(uchar population[][CHROMO_LENGTH], uchar children[][CHROMO_LENGTH],
                 int fitnesses[], float avg_fitness, int total_fitness)
{
    float roulette_hand = randscale() * avg_fitness;
    for (int j = 0; j < POPULATION_SIZE; j++) {
        float point = fitnesses[0];
        for (int i = 0; i < POPULATION_SIZE; i++) {
            if (point > roulette_hand) {
                copy_chromo(children[j], population[i]);
                break;
            }
            point += fitnesses[i];
        }
        roulette_hand += avg_fitness;
    }
}

// Remainder stohastic sampling roulette
void rss_roulette(uchar population[][CHROMO_LENGTH], uchar children[][CHROMO_LENGTH],
                  float avg_fitness, float avg_rel_fitnesses[], float avg_rel_total_fitness)
{
    float roulette_hand = randscale() * avg_fitness;
    for (int j = 0; j < POPULATION_SIZE; j++) {
        float point = avg_rel_fitnesses[0];
        for (int i = 0; i < POPULATION_SIZE; i++) {
            if (point > roulette_hand) {
                copy_chromo(children[j], population[i]);
                break;
            }
            point += avg_rel_fitnesses[i];
        }
        roulette_hand += avg_fitness;
    }
}

int roulette(int fitnesses[], int total_fitness)
{
    int roulette_hand = randint(total_fitness + 1);
    int point = fitnesses[0];
    for (int i = 1; i < POPULATION_SIZE; i++) {
        if (point > roulette_hand) {
            return i;
        }
        point += fitnesses[i];
    }
    return 0;
}

int superfit_score = 0;
uchar superfit[CHROMO_LENGTH];
int generation_count;
uchar population[POPULATION_SIZE][CHROMO_LENGTH];
uchar children[POPULATION_SIZE][CHROMO_LENGTH];
int fitnesses[POPULATION_SIZE];

int m = 0;
void mark()
{
    printf("MARK %i\n", m++);
}


int main(void)
{
    srand(time(NULL));
    printf("Current sides:\n");
    draw_cube(sides);
    printf("\nDesired sides:\n");
    draw_cube(solved_sides);

    // Generate random population
    for (int i = 0; i < POPULATION_SIZE; i++) {
        random_chromo(population[i]);
    }

    int total_fitness = 0;
    int fittest_i = 0;
    int solution_found = 0;
    uchar sides_buf[SIDE_COUNT][CELL_COUNT];
    while (!solution_found && generation_count < MAX_GENERATIONS) {
        total_fitness = 0;
        fittest_i = 0;
        generation_count += 1;

        // Evaluation
        for (int i = 0; i < POPULATION_SIZE; i++) {
            // Calculate fitness. If solved, return
            int fit = calculate_fitness(population[i], sides, solved_sides);
            if (fit == SOLUTION_REWARD) {
                printf("\nSolution found!\n");
                solution_found = 1;
                fittest_i = i;
                break;
            }

            // Assign fitness
            fitnesses[i] = fit;
            total_fitness += fitnesses[i];

            // Remember index of fittest chromosome
            if (fitnesses[fittest_i] < fit) {
                fittest_i = i;
            }

            // Save superfit
            if (superfit_score < fitnesses[fittest_i]) {
                superfit_score = fitnesses[fittest_i];
                copy_chromo(superfit, population[fittest_i]);
            }
        }

        // Sampling
#if SAMPLING_TYPE == ROULETTE
        for (int i = 0; i < POPULATION_SIZE; i += 2) {
            int parent1 = roulette(fitnesses, total_fitness);
            int parent2 = roulette(fitnesses, total_fitness);
#if IMMEDIATE_REPOP == 0
            copy_chromo(children[i], population[parent1]);
            copy_chromo(children[i + 1], population[parent2]);
#elif IMMEDIATE_REPOP == 1
            // Find weakest 2
            int weakest1 = 0, weakest2 = 1;
            for (int j = 2; j < POPULATION_SIZE; j++) {
                if (fitnesses[j] < fitnesses[weakest1]) {
                    weakest2 = weakest1;
                    weakest1 = j;
                } else if (fitnesses[j] < fitnesses[weakest2]) {
                    weakest2 = fitnesses[j];
                }
            }

            // Replace weakest 2 with new children
            crossover(population[parent1], population[parent2],
                      population[weakest1], population[weakest2]);

            // Mutate
            mutate(population[weakest1]);
            mutate(population[weakest2]);

            // Update total fitness
            total_fitness -= fitnesses[weakest1];
            total_fitness -= fitnesses[weakest2];
            fitnesses[weakest1] = calculate_fitness(population[weakest1], sides, solved_sides);
            fitnesses[weakest2] = calculate_fitness(population[weakest2], sides, solved_sides);
            total_fitness += fitnesses[weakest1];
            total_fitness += fitnesses[weakest2];

            // Update fittest in generation
            if (fitnesses[fittest_i] < fitnesses[weakest1]) {
                fittest_i = weakest1;
            }
            if (fitnesses[fittest_i] < fitnesses[weakest2]) {
                fittest_i = weakest2;
            }

            // Save superfit
            if (superfit_score < fitnesses[fittest_i]) {
                superfit_score = fitnesses[fittest_i];
                copy_chromo(superfit, population[fittest_i]);
            }
#endif
        }
#elif SAMPLING_TYPE == STOHASTIC
        float avg_fitness = total_fitness / POPULATION_SIZE;
        ss_roulette(population, children, fitnesses, avg_fitness, total_fitness);
#elif SAMPLING_TYPE == REM_STOHASTIC
        float avg_fitness = total_fitness / POPULATION_SIZE;
        float avg_rel_total_fitness = 0;
        float avg_rel_fitnesses[POPULATION_SIZE]; // average-relative fitnesses
        for (int i = 0; i < POPULATION_SIZE; i++) {
            float fit = fitnesses[i] / avg_fitness;
            avg_rel_fitnesses[i] = fit;
            avg_rel_total_fitness += fit;
        }
        rss_roulette(population, children, avg_fitness, avg_rel_fitnesses, avg_rel_total_fitness);
#endif

#if IMMEDIATE_REPOP == 0
        // Repopulate from intermediate generation
        for (int i = 0; i < POPULATION_SIZE; i += 2) {
            crossover(children[i], children[i + 1],
                      population[i], population[i + 1]);
            mutate(population[i]);
            mutate(population[i + 1]);
        }
#endif

        // Draw resulting cube of the fittest chromo in current generation
        if (generation_count % DRAW_EVERY == 0) {
            copy_all_sides(sides_buf, sides);
            for (int i = 0; i < CHROMO_LENGTH; i++)
                apply_action(population[fittest_i][i], sides_buf);
            printf("\n Generation %i fittest (%i):\n", generation_count, fitnesses[fittest_i]);
            draw_cube(sides_buf);
            print_chromo(population[fittest_i]);
        }
    }

    copy_all_sides(sides_buf, sides);
    for (int i = 0; i < CHROMO_LENGTH; i++)
        apply_action(population[fittest_i][i], sides_buf);
    printf("\n Generation %i fittest (%i):\n", generation_count, fitnesses[fittest_i]);
    draw_cube(sides_buf);
    print_chromo(population[fittest_i]);


    if (solution_found) {
        printf("\nSOLVED\n");
    }

    // Draw superfit result
    copy_all_sides(sides_buf, sides);
    for (int i = 0; i < CHROMO_LENGTH; i++)
        apply_action(superfit[i], sides_buf);
    printf("\n Superfit chromosome (%i): \n", superfit_score);
    draw_cube(sides_buf);
    printf("\n");
    print_chromo(superfit);
    printf("\n");
}
