#include <stdlib.h>

#include "common.h"

position_t** pred = NULL;
double** dis = NULL;
double** heuristique = NULL;
int** heuristique_propagation = NULL;
int** heuristique_in_queue = NULL;
int** visited = NULL;
position_t*** ptrs = NULL;