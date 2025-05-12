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

    colored_image_t colored_image = image_read(argv[1]);
    image_show(colored_image);

    image_t image = colored_image_to_image(colored_image);

    // Si l'image est trop grande on la réduit en-dessous d'une image en 1280x720
    int n1 = image.rows / 720;
    int n2 = image.cols / 1280;
    int n = n1 > n2 ? n1 : n2;
    if (n > 1) {
        image = image_resize(image, n);
    }
    //image_show(image);


    // Flou gaussien
    kernel_t kernel = create_gaussian_kernel(5, 1.0);
    image_t blured_image = image_apply_filter(image, kernel);
    image_free(image);
    //image_show(blured_image);

    // Sobel
    image_t gradient_x, gradient_y;
    image_t sobeled_image = blured_image;
    image_apply_sobel(blured_image, &gradient_x, &gradient_y);
    //image_show(sobeled_image);

    // Suppression des non maxima locaux
    image_t direction = image_compute_gradient_direction(gradient_x, gradient_y);

    image_t non_maxima_deleted_image = image_non_maxima_suppression(sobeled_image, direction);
    image_free(sobeled_image);
    //image_show(non_maxima_deleted_image);

    // Seuillage
    image_t final_image = non_maxima_deleted_image;
    image_double_threshold(final_image, 1/4., 1/8.);
    //image_show(final_image);

    // Hystérésis et epaississement des contours
    image_hysteresis(final_image);
    image_t img = image_thicken(final_image, n/2, 1.);
    free_image(final_image);
    image_show(img);


    // Parcours
    position_t s = {.i = 965/n, .j = 741/n};
    position_t t = {.i = 2635/n, .j = 2420/n};
    queue_t* solution = solve_dijkstra(img, s, t);
    pixel_t pixel = {.r = 0, .g = 0, .b = 0};
    draw_solution(colored_image, solution, pixel, n);

    return 0;
}