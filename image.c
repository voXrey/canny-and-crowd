#include <opencv2/opencv.hpp>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "priority_queue.h"
#include "queue.h"
#include "image_usage.h"
#include "image.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <image_path>\n", argv[0]);
        return -1;
    }

    fprintf(stderr, "Lecture de l'image : %s...\n", argv[1]);

    colored_image_t colored_image = image_read(argv[1]);    
    fprintf(stderr, "Image lue et convertie\n");
    
    colored_image_show(colored_image);

    image_t image = image_from_colored_image(colored_image);
    image_show(image);

    // Si l'image est trop grande on la réduit en-dessous d'une image en 1280x720
    fprintf(stderr, "Réduction de la taille de l'image...\n");
    int n1 = image.rows / 720;
    int n2 = image.cols / 1280;
    int n = n1 > n2 ? n1 : n2;
    if (n > 1) {
        image = image_resize(image, n);
    }
    image_show(image);


    // Flou gaussien
    fprintf(stderr, "Flou Gaussien...\n");
    kernel_t kernel = create_gaussian_kernel(5, 1.0);
    fprintf(stderr, "Noyau gaussien créé.\n");
    image_t blured_image = image_apply_filter(image, kernel);
    image_free(image);
    image_show(blured_image);

    // Sobel
    fprintf(stderr, "Filtre de Sobel\n");
    image_t gradient_x, gradient_y;
    image_t sobeled_image = blured_image;
    image_apply_sobel(blured_image, &gradient_x, &gradient_y);
    image_show(sobeled_image);

    // Suppression des non maxima locaux
    fprintf(stderr, "Suppression des maxima locaux\n");
    image_t direction = image_compute_gradient_direction(gradient_x, gradient_y);

    image_t non_maxima_deleted_image = image_non_maxima_suppression(sobeled_image, direction);
    image_free(sobeled_image);
    image_show(non_maxima_deleted_image);

    // Seuillage
    fprintf(stderr, "Seuillage\n");
    image_t final_image = non_maxima_deleted_image;
    image_double_threshold(final_image, 1/4., 1/8.);
    image_show(final_image);

    // Hystérésis et epaississement des contours
    fprintf(stderr, "Hystérésis et épaississment des contours\n");
    image_hysteresis(final_image);
    image_t img = image_thicken(final_image, n/2, 1.);
    image_free(final_image);
    image_show(img);


    // Parcours
    fprintf(stderr, "Parcours de l'image\n");
    position_t s = {.i = 965/n, .j = 741/n};
    position_t t = {.i = 2635/n, .j = 2420/n};
    queue_t* solution = solve_dijkstra(img, s, t);
    colored_pixel_t pixel = {.r = 0, .g = 0, .b = 0};
    draw_solution(colored_image, solution, pixel, n);

    colored_image_free(colored_image);
    image_free(img);
    queue_free(solution);

    return 0;
}