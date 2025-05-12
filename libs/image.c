#include <opencv2/opencv.hpp>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "image.h"


typedef struct double pixel_t; // Double entre 0. et 1.

struct image_s {
    char* name;
    int rows;
    int cols;
    pixel_t** pixels;
};
typedef struct image_s image_t;

struct kernel_s {
    int size;
    double** data; // tableau de taille size*size
};
typedef struct kernel_s kernel_t;


void image_free(grey_image_t image) {
    for (int i = 0; i < image.rows; i++) {
        free(image.pixels[i]);
    }
    free(image.pixels);
}


image_t image_to_grey_image(image_t image) {
    image_t image = {
        .name = image.name,
        .rows = image.rows,
        .cols = image.cols,
        .pixels = (double**) malloc(sizeof(double*) * image.rows),
    };
    for (int i = 0; i < image.rows; i++) {
        image.pixels[i] = (double*) malloc(sizeof(double) * image.cols);
        for (int j = 0; j < image.cols; j++) {
            image.pixels[i][j] = (image.pixels[i][j].r + image.pixels[i][j].g + image.pixels[i][j].b) / 3. / 255;
        }
    }
    return image;
}