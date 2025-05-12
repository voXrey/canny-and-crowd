#ifndef IMAGE_H
#define IMAGE_H

#include <opencv2/opencv.hpp>

// Définition des structures

// Structure représentant un pixel coloré (RGB)
typedef struct colored_pixel_s {
    double r; // Rouge
    double g; // Vert
    double b; // Bleu
} colored_pixel_t;

// Structure représentant une image colorée
typedef struct colored_image_s {
    char* name;
    int rows;
    int cols;
    colored_pixel_t** pixels;
} colored_image_t;

// Structure représentant un pixel en niveaux de gris (double entre 0 et 1)
typedef double pixel_t;

// Structure représentant une image en niveaux de gris
typedef struct image_s {
    char* name;
    int rows;
    int cols;
    pixel_t** pixels;
} image_t;

// Structure représentant un noyau de convolution
typedef struct kernel_s {
    int size;
    double** data; // tableau de taille size*size
} kernel_t;

// Fonctions de gestion de la mémoire
void image_free(image_t image);
void colored_image_free(colored_image_t image);
void kernel_free(kernel_t kernel);

// Fonctions de conversion
colored_image_t colored_image_from_mat(cv::Mat mat);
cv::Mat cvmat_from_colored_image(colored_image_t colored_image);
cv::Mat cvmat_from_image(image_t image);
image_t image_from_colored_image(colored_image_t colored_image);

// Fonctions d'affichage
void colored_image_show(colored_image_t image);
void image_show(image_t image);

// Fonctions de manipulation d'images
image_t image_resize(image_t image, int scale);
image_t image_apply_filter(image_t image, kernel_t kernel);
void image_thicken(image_t image, int n, pixel_t intensity);

#endif // IMAGE_H