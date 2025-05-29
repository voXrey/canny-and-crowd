#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>


#include "image.h"
#include "image_usage.h"
#include "queue.h"
#include "priority_queue.h"
#include "logging.h"
#include "common.h"


// Créer un noyau gaussien de taille size et d'écart-type sigma
kernel_t create_gaussian_kernel(int size, double sigma) {
    log_debug("Création d'un noyau gaussien de taille %d et d'écart-type %.2f", size, sigma);
    kernel_t kernel = {
        .size = size,
        .data = (double**) malloc(sizeof(double*) * size)
    };
    double mean = size / 2;
    double sum = 0.0;
    for (int x = 0; x < size; x++) {
        kernel.data[x] = (double*) malloc(sizeof(double) * size);
        for (int y = 0; y < size; y++) {
            kernel.data[x][y] = exp(-0.5 * (pow((x - mean) / sigma, 2.0) + pow((y - mean) / sigma, 2.0)))
                                / (2 * M_PI * sigma * sigma);
            sum += kernel.data[x][y];
        }
    }
    // Normalisation du noyau
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            kernel.data[i][j] /= sum;
        }
    }
    log_debug("Noyau gaussien créé de taille %d et d'écart-type %.2f", size, sigma);
    return kernel;
}

// Créer le noyau de Sobel selon l'axe des x
kernel_t create_sobel_kernel_x() {
    log_debug("Création du noyau de Sobel selon l'axe des x");
    kernel_t kernel = {
        .size = 3,
        .data = (double**) malloc(sizeof(double*) * 3)
    };
    for (int i = 0; i < 3; i++) {
        kernel.data[i] = (double*) malloc(sizeof(double) * 3);
    }

    kernel.data[0][0] = -1; kernel.data[0][1] = -2; kernel.data[0][2] = -1;
    kernel.data[1][0] = 0; kernel.data[1][1] = 0; kernel.data[1][2] = 0;
    kernel.data[2][0] = 1; kernel.data[2][1] = 2; kernel.data[2][2] = 1;

    log_debug("Noyau de Sobel créé selon l'axe des x");

    return kernel;
}

// Créer le noyau de Sobel selon l'axe des y
kernel_t create_sobel_kernel_y() {
    log_debug("Création du noyau de Sobel selon l'axe des y");
    kernel_t kernel = {
        .size = 3,
        .data = (double**) malloc(sizeof(double*) * 3)
    };
    for (int i = 0; i < 3; i++) {
        kernel.data[i] = (double*) malloc(sizeof(double) * 3);
    }

    kernel.data[0][0] = -1; kernel.data[0][1] = 0; kernel.data[0][2] = 1;
    kernel.data[1][0] = -2; kernel.data[1][1] = 0; kernel.data[1][2] = 2;
    kernel.data[2][0] = -1; kernel.data[2][1] = 0; kernel.data[2][2] = 1;

    log_debug("Noyau de Sobel créé selon l'axe des y");

    return kernel;
}

// Appliquer un filtre de Sobel à un image_t (avec extension de l'image) et calcule les gradients
void image_apply_sobel(image_t image, image_t* gradient_x, image_t* gradient_y) {
    log_debug("Application du filtre de Sobel à l'image : %s", image.name);
    kernel_t kernel_x = create_sobel_kernel_x();
    kernel_t kernel_y = create_sobel_kernel_y();

    image_t copy1 = image_copy(image);
    image_t copy2 = image_copy(image);

    *gradient_x = image_apply_filter(copy1, kernel_x);
    *gradient_y = image_apply_filter(copy2, kernel_y);

    // Variables pour la normalisation
    double g_max = 0.;
    double g_min = 1.;

    // Première étape de calcul
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            image.pixels[i][j] = sqrt(pow(gradient_x->pixels[i][j], 2) + pow(gradient_y->pixels[i][j], 2));
        }
    }

    // Normalisation
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            if (image.pixels[i][j] < g_min) g_min = image.pixels[i][j];
            if (image.pixels[i][j] > g_max) g_max = image.pixels[i][j];
        }
    }
    if (g_max != g_min) {
        for (int i = 0; i < image.rows; i++) {
            for (int j = 0; j < image.cols; j++) {
                image.pixels[i][j] = (image.pixels[i][j] - g_min)/(g_max - g_min);
            }
        }
    }


    image_free(copy1);
    image_free(copy2);
    kernel_free(kernel_x);
    kernel_free(kernel_y);

    log_debug("Filtre de Sobel appliqué à l'image : %s", image.name);
}


// Calculer la direction des gradients
image_t image_compute_gradient_direction(image_t gradient_x, image_t gradient_y) {
    log_debug("Calcul de la direction des gradients");
    image_t direction = image_copy(gradient_x);

    for (int i = 0; i < gradient_x.rows; i++) {
        for (int j = 0; j < gradient_x.cols; j++) {
            direction.pixels[i][j] = atan2(gradient_x.pixels[i][j], gradient_y.pixels[i][j]);
        }
    }
    log_debug("Direction des gradients calculée");

    return direction;
}


// Suppression des non-maxima locaux
image_t image_non_maxima_suppression(image_t image, image_t direction) {
    log_debug("Suppression des non-maxima locaux");
    image_t result = image_copy(image);

    for (int i = 1; i < image.rows-1; i++) {
        for (int j = 1; j < image.cols-1; j++) {
            double angle = direction.pixels[i][j]; // angle en radians
            angle = fmod(angle + M_PI, M_PI); // angle entre 0 et pi

            double q = 0.0;
            double r = 0.0;

            if ((angle >= 0 && angle < M_PI/8) || (angle >= 7*M_PI/8 && angle < M_PI)) {
                q = image.pixels[i][j+1];
                r = image.pixels[i][j-1];
            } else if (angle >= M_PI/8 && angle < 3*M_PI/8) {
                q = image.pixels[i-1][j+1];
                r = image.pixels[i+1][j-1];
            } else if (angle >= 3*M_PI/8 && angle < 5*M_PI/8) {
                q = image.pixels[i-1][j];
                r = image.pixels[i+1][j];
            } else if (angle >= 5*M_PI/8 && angle < 7*M_PI/8) {
                q = image.pixels[i-1][j-1];
                r = image.pixels[i+1][j+1];
            }

            if (image.pixels[i][j] >= q && image.pixels[i][j] >= r) {
                result.pixels[i][j] = image.pixels[i][j];
            } else {
                result.pixels[i][j] = 0;
            }
        }
    }
    image_free(direction);

    log_debug("Suppression des non-maxima locaux terminée");
    return result;
}


// Appliquer un double seuil
void image_double_threshold(image_t image, double t_max, double t_min) {
    log_debug("Application d'un double seuil : t_max = %.2f, t_min = %.2f", t_max, t_min);
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            // Si l'intensité est assez forte on garde le pixel
            if (image.pixels[i][j] > t_max) {
                image.pixels[i][j] = 1.;
            }
            // Si elle est trop faible on le supprime
            else if (image.pixels[i][j] < t_min) {
                image.pixels[i][j] = 0.;
            }
            // Si elle est entre les deux seuils on regardera si un voisin est assez fort
            else {
                image.pixels[i][j] = 1/2.;
            }
        }
    }
    log_debug("Double seuil appliqué");
}

// Tracer les contours en contact avec un pixel fort
void image_hysteresis_aux(image_t image, bool** visited, int i, int j, queue_t* queue) {
    position_t* pos = (position_t*) malloc(sizeof(position_t));
    pos->i = i;
    pos->j = j;
    queue_enqueue(queue, (void*) pos);
    visited[i][j] = true;

    while (!queue_is_empty(queue)) {
        position_t* p = (position_t*) queue_dequeue(queue);
        image.pixels[p->i][p->j] = 1.; // Marquer le pixel comme fort

        // Vérifier les voisins
        for (int di = -1; di <= 1; di++) {
            for (int dj = -1; dj <= 1; dj++) {
                if (di == 0 && dj == 0) continue; // Ignorer le pixel central
                int ni = p->i + di;
                int nj = p->j + dj;

                if (ni >= 0 && ni < image.rows && nj >= 0 && nj < image.cols &&
                    !visited[ni][nj] && image.pixels[ni][nj] != 0) {
                    visited[ni][nj] = true;

                    position_t* new_pos = (position_t*) malloc(sizeof(position_t));
                    new_pos->i = ni;
                    new_pos->j = nj;

                    queue_enqueue(queue, (void*) new_pos);
                }
            }
        }
        free(p);
    }
}

// Tracer les contours d'une image avec une hystérésis
void image_hysteresis(image_t image) {
    log_debug("Application de l'hystérésis sur l'image : %s", image.name);
    // Initialiser la matrice des pixels visités
    bool** visited = (bool**) malloc(sizeof(bool*) * image.rows);
    for (int i = 0; i < image.rows; i++) {
        visited[i] = (bool*) malloc(sizeof(bool) * image.cols);
        for (int j = 0; j < image.cols; j++) {
            visited[i][j] = false;
        }
    }

    // Initialiser la queue
    queue_t* queue = queue_create();

    // Pour chaque pixel fort, rendre fort les pixels faibles connectés
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            if (!visited[i][j] && image.pixels[i][j] == 1.) {
                image_hysteresis_aux(image, visited, i, j, queue);
            }
        }
    }

    // Supprimer les pixels faibles restant
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            if (image.pixels[i][j] != 1.) image.pixels[i][j] = 0.;
        }
    }

    // Libérer les ressources
    for (int i = 0; i < image.rows; i++) {
        free(visited[i]);
    }
    free(visited);
    queue_free(queue);

    log_debug("Hystérésis appliquée sur l'image : %s", image.name);
}

// Application du filtre de Canny
image_t canny(image_t image, double t_max, double t_min) {
    log_debug("Application du filtre de Canny sur l'image : %s", image.name);

    // Flou gaussien
    kernel_t kernel = create_gaussian_kernel(5, 1.0);
    image_t blured_image = image_apply_filter(image, kernel);
    kernel_free(kernel);

    // Appliquer le filtre de Sobel
    image_t gradient_x, gradient_y;
    image_apply_sobel(blured_image, &gradient_x, &gradient_y);

    // Calculer la direction des gradients
    image_t direction = image_compute_gradient_direction(gradient_x, gradient_y);
    image_free(gradient_x);
    image_free(gradient_y);

    // Suppression des non-maxima locaux
    image_t non_maxima = image_non_maxima_suppression(blured_image, direction);
    image_free(blured_image);

    // Appliquer un double seuil
    image_double_threshold(non_maxima, t_max, t_min);

    // Appliquer l'hystérésis
    image_hysteresis(non_maxima);

    log_debug("Filtre de Canny appliqué sur l'image : %s", image.name);

    return non_maxima;
}

// Rendre continue les contours de l'image
image_t fermeture_morphologique(image_t image, int size) {
    log_debug("Application de la fermeture morphologique sur l'image : %s", image.name);
    image_t result = image_copy(image);

    // Dilatation
    int mean = size / 2;
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            bool to_dilate = false;
            for (int x = -mean; x < mean && !to_dilate; x++) {
                for (int y = -mean; y < mean; y++) {
                    int ni = i + x;
                    int nj = j + y;
                    if (!(ni >= 0 && ni < image.rows && nj >= 0 && nj < image.cols)) continue;
                    if (image.pixels[ni][nj] > 0) {
                        to_dilate = true;
                        break;
                    }
                }
            }
            if (to_dilate) {
                result.pixels[i][j] = 1.0; // Dilater le pixel
            }
        }
    }

    // Erosion
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            bool to_erode = false;
            for (int x = -mean; x < mean && !to_erode; x++) {
                for (int y = -mean; y < mean; y++) {
                    int ni = i + x;
                    int nj = j + y;
                    if (!(ni >= 0 && ni < image.rows && nj >= 0 && nj < image.cols)) continue;
                    if (image.pixels[ni][nj] < 1.0) {
                        to_erode = true;
                        break;
                    }
                }
            }
            if (to_erode) {
                result.pixels[i][j] = 0.; // Eroder le pixel
            }
        }
    }

    log_debug("Fermeture morphologique appliquée sur l'image : %s", image.name);
    return result;
}