#ifndef COMMON_H
#define COMMON_H

struct movement_s {
    position_t start;
    position_t target;
    int agents;
};
typedef struct movement_s movement_t;


typedef struct position_s {
    int i;
    int j;
} position_t;


#endif