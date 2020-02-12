/*
  rubsolv by Nika Otiashvili
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "colors.h"

#define CUBE_SIZE         3
#define SIDE_COUNT        6
#define GENE_SIZE         3
#define CHROMO_LENGTH     50
#define MAX_GENERATIONS   10000
#define POPULATION_SIZE   1500
#define CELL_STR          "  "
#define COLOR_CELLS       1
#define SOLUTION_REWARD   100000
#define MUTATION_RATE     0.01f * GENE_SIZE
#define CROSSOVER_RATE    0.4f
#define REWARD_ACTION     +
#define REWARD_MULT       1
#define DRAW_EVERY        100
#define RAND_END          (int) ((RAND_MAX / n) * n)

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
    NORTH_LEFT,     // 000 Rotate top side clockwise
    NORTH_RIGHT,    // 001 Rotate top side counter-clockwise
    EAST_UP,        // 010 Rotate right side upwards
    EAST_DOWN,      // 011 Rotate right side downwards
    SOUTH_LEFT,     // 100 Rotate bottom side clockwise
    SOUTH_RIGHT,    // 101 Rotate bottom side counter-clockwise
    WEST_UP,        // 110 Rotate left side upwards
    WEST_DOWN,      // 111 Rotate left side downwards
};

// Returns unbiased random integer in range [0, n)
int randint(int n)
{
    if (n - 1 == RAND_MAX) {
        return rand();
    }

    int r;
    while ((r = rand()) >= RAND_END);
    return r % n;
}

// Returns random float in range [0.0, 1.0)
float randscale()
{
    int r;
    while ((r = rand()) == RAND_MAX);
    return (float) r / RAND_MAX;
}

char get_random_gene()
{
    return 1 << randint(GENE_SIZE);
}

void random_chromo(char chromo[])
{
    for (size_t i = 0; i < CHROMO_LENGTH; i++) {
        chromo[i] = get_random_gene();
    }
}

void copy_chromo(char to[], char from[])
{
    for (int i = 0; i < CHROMO_LENGTH; i++) {
        to[i] = from[i];
    }
}

void print_chromo(char chromo[])
{
    size_t i = 0;
    while (i < CHROMO_LENGTH - 1)
        printf("%X", chromo[i++]);
    printf("%X", chromo[++i]);
}

void crossover(char parent1[], char parent2[], char child1[], char child2[])
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

void mutate(char chromo[])
{
    for (int i = 0; i < CHROMO_LENGTH; i++) {
        if (randscale() > MUTATION_RATE) {
            continue;
        }

        int n = randint(GENE_SIZE);
        int mask = ~(1 << n);
        // Flip nth bit in chromo[i]
        chromo[i] = (~chromo[i] & mask) | (chromo[i] & ~mask);
    }
}

const size_t CELL_COUNT = CUBE_SIZE * CUBE_SIZE;
char sides[SIDE_COUNT][CELL_COUNT] = {
    // Side 0
    { 'w', 'w', 'w',
      'w', 'w', 'w',
      'w', 'w', 'w', },

    // Side 1
    { 'g', 'g', 'r',
      'y', 'b', 'r',
      'y', 'o', 'b', },

    // Side 2
    { 'y', 'o', 'o',
      'r', 'r', 'o',
      'g', 'b', 'g', },

    // Side 3
    { 'o', 'g', 'r',
      'y', 'o', 'r',
      'r', 'y', 'y', },

    // Side 4
    { 'o', 'g', 'b',
      'o', 'g', 'y',
      'b', 'b', 'o', },

    // Side 5
    { 'y', 'b', 'r',
      'b', 'y', 'r',
      'g', 'g', 'b', },
};
char solved_sides[SIDE_COUNT][CELL_COUNT] = {
    // Side 0
    { 'w', 'w', 'w',
      'w', 'w', 'w',
      'w', 'w', 'w', },

    // Side 1
    { 'b', 'b', 'b',
      'b', 'b', 'b',
      'b', 'b', 'b', },

    // Side 2
    { 'r', 'r', 'r',
      'r', 'r', 'r',
      'r', 'r', 'r', },

    // Side 3
    { 'o', 'o', 'o',
      'o', 'o', 'o',
      'o', 'o', 'o', },

    // Side 4
    { 'g', 'g', 'g',
      'g', 'g', 'g',
      'g', 'g', 'g', },

    // Side 5
    { 'y', 'y', 'y',
      'y', 'y', 'y',
      'y', 'y', 'y', },
};

/* Numbered side for debugging

   {'0', '1', '2',
   '3', '4', '5',
   '6', '7', '8', },

   Alphabetized side

   {'A', 'B', 'C',
   'D', 'E', 'F',
   'G', 'H', 'I', },

*/

void print_cell(char cell)
{
    if (COLOR_CELLS) {
        switch(cell) {
        case 'r': printf("%s%s%s", RED, CELL_STR, END); break;
        case 'b': printf("%s%s%s", BLUE, CELL_STR, END); break;
        case 'g': printf("%s%s%s", GREEN, CELL_STR, END); break;
        case 'y': printf("%s%s%s", YELLOW, CELL_STR, END); break;
        case 'o': printf("%s%s%s", ORANGE, CELL_STR, END); break;
        case 'w': printf("%s%s%s", WHITE, CELL_STR, END); break;
        default: printf("%c ", cell); break;
        }
    } else {
        printf("%c ", cell);
    }
}

void draw_cube(char sides[SIDE_COUNT][CELL_COUNT]) {
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
    const size_t side_count = SIDE_COUNT - 2;
    size_t side_i[side_count] = {1, 0, 4, 5};
    for (size_t j = 0; j < CUBE_SIZE; j++) {
        for (size_t x = 0; x < side_count; x++) {
            for (size_t i = 0; i < CUBE_SIZE; i++) {
                print_cell(sides[side_i[x]][x / side_count + i + j * CUBE_SIZE]);
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
void flip_side_diag_back(char cells[])
{
    int i, j = 0;
    int compl_k; // index of cell with which k is swapped
    char tmp; // temp storage
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
void flip_side_diag_forw(char cells[])
{
    int i, j = 0;
    int compl_k; // index of cell with which k is swapped
    char tmp; // temp storage
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
void flip_side_horiz(char cells[])
{
    const int half = CUBE_SIZE / 2;
    int i, j = 0;
    int compl_k;
    char tmp;
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

void rotate_side_right(char cells[CELL_COUNT])
{
    flip_side_diag_forw(cells);
    flip_side_horiz(cells);
}

void rotate_side_left(char cells[])
{
    flip_side_diag_back(cells);
    flip_side_horiz(cells);
}

void copy_side(char to[], char from[])
{
    for (int i = 0; i < CELL_COUNT; i++)
        to[i] = from[i];
}

void copy_all_sides(char to[][CELL_COUNT], char from[][CELL_COUNT])
{
    for (int i = 0; i < SIDE_COUNT; i++)
        copy_side(to[i], from[i]);
}

void copy_side_region(char to[], int to_indices[], char from[], int from_indices[])
{
    for (int i = 0; i < CUBE_SIZE; i++) {
        to[to_indices[i]] = from[from_indices[i]];
    }
}

void swap_regions(char side1[], int side1_region[], char side2[], int side2_region[])
{
    char tmp_side[CELL_COUNT];
    copy_side_region(tmp_side, side1_region, side1, side1_region);
    copy_side_region(side1, side1_region, side2, side2_region);
    copy_side_region(side2, side2_region, tmp_side, side1_region);
}

void north_right(char sides[][CELL_COUNT])
{
    swap_regions(sides[5], NORTH_REGION, sides[4], NORTH_REGION);
    swap_regions(sides[4], NORTH_REGION, sides[0], NORTH_REGION);
    swap_regions(sides[0], NORTH_REGION, sides[1], NORTH_REGION);
    rotate_side_left(sides[2]);
}

void north_left(char sides[][CELL_COUNT])
{
    swap_regions(sides[1], NORTH_REGION, sides[0], NORTH_REGION);
    swap_regions(sides[0], NORTH_REGION, sides[4], NORTH_REGION);
    swap_regions(sides[4], NORTH_REGION, sides[5], NORTH_REGION);
    rotate_side_right(sides[2]);
}

void south_right(char sides[][CELL_COUNT])
{
    swap_regions(sides[5], SOUTH_REGION, sides[4], SOUTH_REGION);
    swap_regions(sides[4], SOUTH_REGION, sides[0], SOUTH_REGION);
    swap_regions(sides[0], SOUTH_REGION, sides[1], SOUTH_REGION);
    rotate_side_right(sides[3]);
}

void south_left(char sides[][CELL_COUNT])
{
    swap_regions(sides[1], SOUTH_REGION, sides[0], SOUTH_REGION);
    swap_regions(sides[0], SOUTH_REGION, sides[4], SOUTH_REGION);
    swap_regions(sides[4], SOUTH_REGION, sides[5], SOUTH_REGION);
    rotate_side_left(sides[3]);
}

void east_down(char sides[][CELL_COUNT])
{
    swap_regions(sides[2], EAST_REGION, sides[0], EAST_REGION);
    swap_regions(sides[2], EAST_REGION, sides[3], EAST_REGION);
    swap_regions(sides[2], EAST_REGION, sides[5], REV_WEST_REGION);
    rotate_side_left(sides[4]);
}

void east_up(char sides[][CELL_COUNT])
{
    swap_regions(sides[2], EAST_REGION, sides[3], EAST_REGION);
    swap_regions(sides[2], EAST_REGION, sides[0], EAST_REGION);
    swap_regions(sides[3], EAST_REGION, sides[5], REV_WEST_REGION);
    rotate_side_right(sides[4]);
}

void west_down(char sides[][CELL_COUNT])
{
    swap_regions(sides[2], WEST_REGION, sides[0], WEST_REGION);
    swap_regions(sides[2], WEST_REGION, sides[3], WEST_REGION);
    swap_regions(sides[2], WEST_REGION, sides[5], REV_EAST_REGION);
    rotate_side_right(sides[1]);
}

void west_up(char sides[][CELL_COUNT])
{
    swap_regions(sides[2], WEST_REGION, sides[3], WEST_REGION);
    swap_regions(sides[2], WEST_REGION, sides[0], WEST_REGION);
    swap_regions(sides[3], WEST_REGION, sides[5], REV_EAST_REGION);
    rotate_side_left(sides[1]);
}

int are_sides_same(char sides1[][CELL_COUNT], char sides2[][CELL_COUNT])
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
int apply_action(char gene, char sides[][CELL_COUNT])
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
    default: return 0;
    }

    return 1;
}

// TODO not portable across different solved_sides
char color_table[256];
char opposites[SIDE_COUNT] = {5, 4, 3, 2, 1, 0};

int get_similarity_score(char sides1[][CELL_COUNT], char sides2[][CELL_COUNT])
{
    int score = 1;
    for (int s = 0; s < SIDE_COUNT; s++) {
        for (int k = 0; k < CELL_COUNT; k++) {
            if (k != 4 && sides1[s][k] == sides2[s][k]) {
                score = score REWARD_ACTION REWARD_MULT;
                if (k % 2 == 1) score += REWARD_ACTION REWARD_MULT;
            }
        }
    }
    return score;
}

int calculate_fitness(char chromo[], char sides[][CELL_COUNT],
                      char desired_sides[][CELL_COUNT])
{
    char tmp_sides[SIDE_COUNT][CELL_COUNT];
    copy_all_sides(tmp_sides, sides);
    for (int i = 0; i < CHROMO_LENGTH; i++) {
        int applied = apply_action(chromo[i], tmp_sides);
        if (applied && are_sides_same(tmp_sides, solved_sides))
            return SOLUTION_REWARD;
    }

    return get_similarity_score(tmp_sides, solved_sides);
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
char superfit[CHROMO_LENGTH];
int generation_count;
char population[POPULATION_SIZE][CHROMO_LENGTH];
char children[POPULATION_SIZE][CHROMO_LENGTH];
int fitnesses[POPULATION_SIZE];

int main(void)
{
    srand(time(NULL));

    color_table['w'] = 0;
    color_table['b'] = 1;
    color_table['r'] = 2;
    color_table['o'] = 3;
    color_table['g'] = 4;
    color_table['y'] = 5;

    printf("Current sides:\n");
    draw_cube(sides);
    printf("\nDesired sides:\n");
    draw_cube(solved_sides);

    generation_count += 1;

    // Generate random population
    for (int i = 0; i < POPULATION_SIZE; i++) {
        random_chromo(population[i]);
    }

    int total_fitness = 0;
    int fittest_i;
    int solution_found = 0;
    char sides_buf[SIDE_COUNT][CELL_COUNT];
    while (!solution_found && generation_count < MAX_GENERATIONS) {
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

        // Draw resulting cube of the fittest chromosome
        if (generation_count % DRAW_EVERY == 0) {
            copy_all_sides(sides_buf, sides);
            for (int i = 0; i < CHROMO_LENGTH; i++)
                apply_action(population[fittest_i][i], sides_buf);
            printf("\n Generation %i fittest (%i):\n", generation_count, fitnesses[fittest_i]);
            draw_cube(sides_buf);
        }

        // Select, crossover, and mutate
        for (int i = 0; i < POPULATION_SIZE; i += 2) {
            // Select 2 parents
            int parent1_i = roulette(fitnesses, total_fitness);
            int parent2_i = roulette(fitnesses, total_fitness);

            // Crossover (family consists of 2 parents and 2 children)
            crossover(
                population[parent1_i],
                population[parent2_i],
                children[i],
                children[i + 1]);

            // Mutate
            mutate(children[i]);
            mutate(children[i + 1]);
        }

        // Replace population with children
        for (int i = 0; i < POPULATION_SIZE; i++) {
            copy_chromo(population[i], children[i]);
        }

        total_fitness = 0;
        fittest_i = 0;
        generation_count += 1;
    }

    if (solution_found) {
        printf("\nSOLVED\n");
    }

    // Draw last result
    copy_all_sides(sides_buf, sides);
    for (int i = 0; i < CHROMO_LENGTH; i++)
        apply_action(population[fittest_i][i], sides_buf);
    printf("\n Generation %i fittest (%i):\n", generation_count, fitnesses[fittest_i]);
    draw_cube(sides_buf);

    // Draw superfit result
    copy_all_sides(sides_buf, sides);
    for (int i = 0; i < CHROMO_LENGTH; i++)
        apply_action(superfit[i], sides_buf);
    printf("\n Superfit chromosome (%i): \n", superfit_score);
    draw_cube(sides_buf);
    printf("\n");
}
