#include "config.h"

/*
   Numbered side for debugging

   { '0', '1', '2',
     '3', '4', '5',
     '6', '7', '8', },

   Alphabetized side

   { 'A', 'B', 'C',
     'D', 'E', 'F',
     'G', 'H', 'I', },
*/

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
     { 'o', 'w', 'o',
       'w', 'w', 'w',
       'o', 'w', 'r', },

     // Side 1
     { 'b', 'y', 'b',
       'r', 'b', 'o',
       'w', 'o', 'y', },

     // Side 2
     { 'r', 'r', 'g',
       'b', 'r', 'y',
       'w', 'b', 'y', },

     // Side 3
     { 'g', 'g', 'b',
       'b', 'o', 'g',
       'g', 'g', 'w', },

     // Side 4
     { 'b', 'o', 'y',
       'r', 'g', 'g',
       'w', 'r', 'r', },

     // Side 5
     { 'r', 'y', 'y',
       'o', 'y', 'b',
       'g', 'y', 'o', },
};

