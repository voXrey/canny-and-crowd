#ifndef CROWD_H
#define CROWD_H


#include "image.h"
#include "image_usage.h"


struct environment_s {
    int rows;
    int cols;
    int** agents;
};
typedef struct environment_s environment_t;

struct movement_s {
    position_t start;
    position_t target;
    int agents;
}
typedef struct movement_s movement_t;


#endif