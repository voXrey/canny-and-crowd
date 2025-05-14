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

    // Application du filtre de Canny
    fprintf(stderr, "Application du filtre de Canny...\n");
    image_t canny_image = canny(image, 0.1, 0.2);

    // Parcours
    fprintf(stderr, "Parcours de l'image\n");
    position_t s = {.i = 965/n, .j = 741/n};
    position_t t = {.i = 2635/n, .j = 2420/n};
    queue_t* solution = solve_dijkstra(canny_image, s, t);
    colored_pixel_t pixel = {.r = 0, .g = 0, .b = 0};
    draw_solution(colored_image, solution, pixel, n);

    colored_image_free(colored_image);
    image_free(canny_image);
    queue_free(solution);

    return 0;
}