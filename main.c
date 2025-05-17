#include <opencv2/opencv.hpp>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "priority_queue.h"
#include "queue.h"
#include "image_usage.h"
#include "image.h"
#include "logging.h"
#include "config.h"
#include "crowd.h"
#include "circular_list.h"

int main(int argc, char** argv) {
    // Chargement de la configuration
    config_load("config.conf");

    if (argc != 2) log_fatal("Usage : %s <image>", argv[0]);


    colored_image_t colored_image = image_read(argv[1]);    
    
    //colored_image_show(colored_image);

    image_t image = image_from_colored_image(colored_image);
    //image_show(image);

    // Si l'image est trop grande on la réduit en-dessous d'une image en 1280x720
    int n1 = image.rows / 720;
    int n2 = image.cols / 1280;
    int n = n1 > n2 ? n1 : n2;
    if (n > 1) {
        image = image_resize(image, n);
    }
    //image_show(image);

    // Application du filtre de Canny
    image_t canny_image = canny(image, 0.1, 0.2);
    
    // Epaississement de l'image
    image_t image_thickened = image_thicken(canny_image, 3, 1.);
    //image_show(image_thickened);
    image_write(image_thickened, "pictures/image_traitee.jpg");


    // Test sur les environnements
    environment_t env = env_from_image(image_thickened);

    position_t s1 = {.i = 1000/n, .j = 741/n};
    position_t t1 = {.i = 2635/n, .j = 2420/n};
    movement_t m1 = {.start = s1, .target = t1, .agents = 100};

    position_t s2 = {.i = 2700/n, .j = 700/n};
    position_t t2 = {.i = 1600/n, .j = 2400/n};
    movement_t m2 = {.start = s2, .target = t2, .agents = 100};


    move_env_a_star(m1, &env);
    
    env_image_colored_edit(colored_image, env, n);
    colored_image_show(colored_image);
    env_free(env);

    // TODO : résoudre le free des listes circulaires (segfault)

    /*
    // Parcours
    position_t s = {.i = 965/n, .j = 741/n};
    position_t t = {.i = 2635/n, .j = 2420/n};
    queue_t* solution = solve_dijkstra(image_thickened, s, t);
    colored_pixel_t pixel = {.r = 0, .g = 0, .b = 0};
    draw_solution(colored_image, solution, pixel, n);

    colored_image_free(colored_image);
    */

    image_free(canny_image);
    image_free(image_thickened);

    return 0;
}