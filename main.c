#include <opencv2/opencv.hpp>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "priority_queue.h"
#include "queue.h"
#include "image_usage.h"
#include "image.h"
#include "logging.h"
#include "config.h"
#include "crowd.h"
#include "circular_list.h"
#include "common.h"
#include "csv.h"

int main(int argc, char** argv) {
    // Chargement de la configuration
    config_load("config.conf");
    clock_t start, end;
    double cpu_time_used;

    if (argc < 4 || 5 < argc) log_fatal("Usage : %s <image> <movements-file> <weight> [compression]", argv[0]);
    const char* movements_file_path = argv[2];
    int weight = atoi(argv[3]);
    int n = 1;

    colored_image_t colored_image = image_read(argv[1]);    
    image_t image = image_from_colored_image(colored_image);

    if (argc == 5) {
        n = atoi(argv[4]);
        image = image_resize(image, n);
    }

    // Application du filtre de Canny
    image_t canny_image = canny(image, 0.2, 0.4);
    
    // Epaississement de l'image
    image_t image_thickened = image_thicken(canny_image, 1, 1.);
    image_write(image_thickened, "pictures/image_traitee.jpg");


    // Test sur les environnements
    environment_t env = env_from_image(image_thickened);

    circular_list_t* movements = load_movements(movements_file_path, n);
    start = clock();
    multiple_move_env_iterative_a_star(movements, &env, weight);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    log_info("A* itératif : %f secondes", cpu_time_used);
    free_movements(movements);

    movements = load_movements(movements_file_path, n);
    start = clock();
    multiple_move_env_iterative_a_star_dijkstra(movements, &env, weight);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    log_info("A* Dijkstra : %f secondes", cpu_time_used);
    free_movements(movements);

    movements = load_movements(movements_file_path, n);
    start = clock();
    multiple_move_env_a_star(movements, &env, weight);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    log_info("A* norme 1 : %f secondes", cpu_time_used);
    free_movements(movements);
    
    
    env_image_colored_edit(colored_image, env, n);
    colored_image_show(colored_image);

    env_free(env);
    image_free(canny_image);
    image_free(image_thickened);

    return 0;
}