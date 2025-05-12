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

    image_t original_image = image_read(argv[1]);

    image_t image = original_image;
    image_show(image);

    // Si l'image est trop grande on la réduit en-dessous d'une image en 1280x720
    int n1 = image.rows / 720;
    int n2 = image.cols / 1280;
    int n = n1 > n2 ? n1 : n2;
    if (n > 1) {
        image = image_resize(image, n);
    }
    //image_show(image);

    // Conversion en niveaux de gris pour simplifier les opérations
    grey_image_t grey_image = image_to_grey_image(image);
    image_free(image);
    //grey_image_show(grey_image);

    // Flou gaussien
    kernel_t kernel = create_gaussian_kernel(5, 1.0);
    grey_image_t blured_image = grey_image_apply_filter(grey_image, kernel);
    grey_image_free(grey_image);
    //grey_image_show(blured_image);

    // Sobel
    grey_image_t gradient_x, gradient_y;
    grey_image_t sobeled_image = blured_image;
    grey_image_apply_sobel(blured_image, &gradient_x, &gradient_y);
    //grey_image_show(sobeled_image);

    // Suppression des non maxima locaux
    grey_image_t direction = grey_image_compute_gradient_direction(gradient_x, gradient_y);

    grey_image_t non_maxima_deleted_image = grey_image_non_maxima_suppression(sobeled_image, direction);
    grey_image_free(sobeled_image);
    //grey_image_show(non_maxima_deleted_image);

    // Seuillage
    grey_image_t final_image = non_maxima_deleted_image;
    grey_image_double_threshold(final_image, 1/4., 1/8.);
    //grey_image_show(final_image);

    // Hystérésis et epaississement des contours
    grey_image_hysteresis(final_image);
    grey_image_thicken(final_image, n/2);
    grey_image_show(final_image);



    // Parcours
    position_t s = {.i = 965/n, .j = 741/n};
    position_t t = {.i = 2635/n, .j = 2420/n};
    queue_t* solution = solve_dijkstra(final_image, s, t);
    pixel_t pixel = {.r = 0, .g = 0, .b = 0};
    draw_solution(original_image, solution, pixel, n);


    return 0;
}