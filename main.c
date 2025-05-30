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

    if (argc < 5 || 6 < argc) {
        log_fatal("Usage : %s <image> <movements-file> <weight0> <alpha> [compression]", argv[0]);
    }
    const char* movements_file_path = argv[2];
    int weight0 = atoi(argv[3]);
    int alpha = atoi(argv[4]);
    if (weight0 < 0 || alpha < 0) {
        log_fatal("Les poids doivent être positifs");
    }
    if (weight0 == 0 && alpha == 0) {
        log_fatal("Les poids ne peuvent pas être tous les deux nuls");
    }
    int n = 1;

    colored_image_t colored_image = image_read(argv[1]);    
    image_t image = image_from_colored_image(colored_image);

    if (argc == 6) {
        n = atoi(argv[5]);
        image_t past = image;
        image = image_resize(image, n);
        image_free(past);
    }

    // Application du filtre de Canny
    image_t canny_image = canny(image, 0.1, 0.2);
    
    // Epaississement de l'image
    image_t image_morpho = image_fermeture_morphologique(canny_image, 30/n);

    image_write(image_morpho, "presentation/image_morpho.jpg");
    image_write(image, "presentation/grey.jpg");
    colored_image_write(colored_image, "presentation/original.jpg");

    // Test sur les environnements
    environment_t env;
    circular_list_t* movements;
    
    env = env_from_image(image_morpho);
    env_initialiser_tableaux(&env);
    movements = load_movements(movements_file_path, n);
    start = clock();
    multiple_move_env_iterative_a_star(movements, &env, weight0, alpha, 10);
    end = clock();
    cpu_time_used = ((double) (end-start)) / CLOCKS_PER_SEC;
    log_info("A* modulo %d : %.3f secondes", 10, cpu_time_used);

    env_image_edit(image_morpho, env, 1);
    image_write(image_morpho, "pictures/image_resultat0.jpg");
    env_image_colored_edit(colored_image, env, n);
    colored_image_write(colored_image, "pictures/image_resultat.jpg");
    log_info("Image resultante ecrite dans pictures/image_resultat.jpg");

    free_movements(movements);
    env_liberer_tableaux(&env);
    env_free(env);
    

    image_free(canny_image);
    image_free(image_morpho);
    colored_image_free(colored_image);
    image_free(image);

    return 0;
}