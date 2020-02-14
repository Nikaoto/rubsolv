#include "config.h"

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

char sides[SIDE_COUNT][CELL_COUNT] = {
     // Side 0
     { 'y', 'w', 'g',
       'w', 'w', 'w',
       'o', 'w', 'b', },

     // Side 1
     { 'o', 'y', 'g',
       'b', 'b', 'o',
       'w', 'y', 'y', },

     // Side 2
     { 'b', 'o', 'y',
       'o', 'r', 'b',
       'r', 'b', 'w', },

     // Side 3
     { 'g', 'g', 'w',
       'r', 'o', 'g',
       'o', 'o', 'w', },

     // Side 4
     { 'r', 'r', 'b',
       'r', 'g', 'g',
       'r', 'r', 'g', },

     // Side 5
     { 'r', 'g', 'y',
       'y', 'y', 'y',
       'o', 'b', 'b', },
};

char sides_2[SIDE_COUNT][CELL_COUNT] = {
     // Side 0
     { 'w', 'w', 'w',
       'w', 'w', 'r',
       'w', 'w', 'w', },

     // Side 1
     { 'b', 'o', 'b',
       'g', 'b', 'b',
       'b', 'b', 'b', },

     // Side 2
     { 'r', 'r', 'r',
       'y', 'r', 'o',
       'r', 'o', 'r', },

     // Side 3
     { 'o', 'r', 'o',
       'y', 'o', 'y',
       'o', 'g', 'o', },

     // Side 4
     { 'g', 'b', 'g',
       'g', 'g', 'b',
       'g', 'g', 'g', },

     // Side 5
     { 'y', 'y', 'y',
       'r', 'y', 'o',
       'y', 'w', 'y', },
};
