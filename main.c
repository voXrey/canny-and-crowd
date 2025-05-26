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
        image_t past = image;
        image = image_resize(image, n);
        image_free(past);
    }

    // Application du filtre de Canny
    image_t canny_image = canny(image, 0.1, 0.2);
    
    // Epaississement de l'image
    image_t image_thickened = image_thicken(canny_image, 5, 1.);
    image_write(image_thickened, "pictures/image_traitee.jpg");
    log_info("Image traitee ecrite dans pictures/image_traitee.jpg");

    // Test sur les environnements
    environment_t env;
    circular_list_t* movements;
    
    env = env_from_image(image_thickened);
    env_initialiser_tableaux(&env);
    movements = load_movements(movements_file_path, n);
    start = clock();
    multiple_move_env_iterative_a_star(movements, &env, weight, 100);
    end = clock();
    cpu_time_used = ((double) (end-start)) / CLOCKS_PER_SEC;
    log_info("A* modulo %d : %.3f secondes", 100, cpu_time_used);

    env_image_colored_edit(colored_image, env, n);
    colored_image_write(colored_image, "pictures/image_resultat.jpg");
    log_info("Image resultante ecrite dans pictures/image_resultat.jpg");

    free_movements(movements);
    env_liberer_tableaux(&env);
    env_free(env);
    

    image_free(canny_image);
    image_free(image_thickened);
    colored_image_free(colored_image);
    image_free(image);

    return 0;
}