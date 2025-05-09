#include <opencv2/opencv.hpp>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "priority_queue.h"
#include "queue.h"

struct position_s {
    int i;
    int j;
};
typedef struct position_s position_t;


struct pixel_s {
    double r;
    double g;
    double b;
};
typedef struct pixel_s pixel_t;

struct image_s {
    char* name;
    int rows;
    int cols;
    pixel_t** pixels;
};
typedef struct image_s image_t;

struct grey_image_s {
    char* name;
    int rows;
    int cols;
    double** pixels;
};
typedef struct grey_image_s grey_image_t;

struct kernel_s {
    int size;
    double* data; // matrice projetée sur une seule dimension
};
typedef struct kernel_s kernel_t;


// Copier un image_t en profondeur
image_t image_copy(image_t image) {
    image_t copy = image;
    copy.name = image.name;
    copy.rows = image.rows;
    copy.cols = image.cols;
    copy.pixels = (pixel_t**) malloc(sizeof(pixel_t*) * image.rows);
    for (int i = 0; i < image.rows; i++) {
        copy.pixels[i] = (pixel_t*) malloc(sizeof(pixel_t) * image.cols);
        for (int j = 0; j < image.cols; j++) {
            copy.pixels[i][j] = image.pixels[i][j];
        }
    }
    return copy;
}

// Copier un grey_image_t en profondeur
grey_image_t grey_image_copy(grey_image_t image) {
    grey_image_t copy = image;
    copy.name = image.name;
    copy.rows = image.rows;
    copy.cols = image.cols;
    copy.pixels = (double**) malloc(sizeof(double*) * image.rows);
    for (int i = 0; i < image.rows; i++) {
        copy.pixels[i] = (double*) malloc(sizeof(double) * image.cols);
        for (int j = 0; j < image.cols; j++) {
            copy.pixels[i][j] = image.pixels[i][j];
        }
    }
    return copy;
}


// Libérer un image_t
void image_free(image_t image) {
    for (int i = 0; i < image.rows; i++) {
        free(image.pixels[i]);
    }
    free(image.pixels);
}

// Libérer un grey_image_t
void grey_image_free(grey_image_t image) {
    for (int i = 0; i < image.rows; i++) {
        free(image.pixels[i]);
    }
    free(image.pixels);
}


// Convertir un cv::Mat en image_t
image_t image_from_mat(cv::Mat mat) {
    image_t image = {
        .name = NULL,
        .rows = mat.rows,
        .cols = mat.cols,
        .pixels = (pixel_t**) malloc(sizeof(pixel_t*) * mat.rows),
    };
    for (int i = 0; i < mat.rows; i++) {
        image.pixels[i] = (pixel_t*) malloc(sizeof(pixel_t) * mat.cols);
        for (int j = 0; j < mat.cols; j++) {
            cv::Vec3b color = mat.at<cv::Vec3b>(i, j);
            image.pixels[i][j] = (pixel_t) {
                .r = (double) color[2],
                .g = (double) color[1],
                .b = (double) color[0]
            };
        }
    }
    return image;
}

// Convertir un image_t en cv::Mat
cv::Mat image_to_cvmat(image_t image) {
    cv::Mat img(image.rows, image.cols, CV_8UC3);
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            cv::Vec3b color;
            color[0] = (uchar) image.pixels[i][j].b;
            color[1] = (uchar) image.pixels[i][j].g;
            color[2] = (uchar) image.pixels[i][j].r;
            img.at<cv::Vec3b>(i, j) = color;
        }
    }
    return img;
}

// Convertir un image_t en un grey_image_t
grey_image_t image_to_grey_image(image_t image) {
    grey_image_t grey_image = {
        .name = image.name,
        .rows = image.rows,
        .cols = image.cols,
        .pixels = (double**) malloc(sizeof(double*) * image.rows),
    };
    for (int i = 0; i < image.rows; i++) {
        grey_image.pixels[i] = (double*) malloc(sizeof(double) * image.cols);
        for (int j = 0; j < image.cols; j++) {
            grey_image.pixels[i][j] = (image.pixels[i][j].r + image.pixels[i][j].g + image.pixels[i][j].b) / 3. / 255;
        }
    }
    return grey_image;
}

// Convertir un image_t en un grey_image_t
image_t grey_image_to_image(grey_image_t grey_image) {
    image_t image = {
        .name = grey_image.name,
        .rows = grey_image.rows,
        .cols = grey_image.cols,
        .pixels = (pixel_t**) malloc(sizeof(pixel_t*) * grey_image.rows),
    };
    for (int i = 0; i < grey_image.rows; i++) {
        image.pixels[i] = (pixel_t*) malloc(sizeof(pixel_t) * grey_image.cols);
        for (int j = 0; j < grey_image.cols; j++) {
            double c = grey_image.pixels[i][j]*255;
            image.pixels[i][j] = (pixel_t) {
                .r = c,
                .g = c,
                .b = c
            };
        }
    }
    return image;
}


// Lire une image dans un fichier
image_t image_read(char* path) {
    cv::Mat mat = cv::imread(path, cv::IMREAD_COLOR);
    if (mat.empty()) {
        fprintf(stderr, "Impossible de lire l'image %s\n", path);
        exit(-1);
    }
    image_t image = image_from_mat(mat);
    image.name = path;
    return image;
}

// Sauvegarder une image dans un fichier
void image_write(image_t image, const char* path) {
    cv::Mat img = image_to_cvmat(image);
    cv::imwrite(path, img);
}

// Afficher une image
void image_show(image_t image) {
    cv::Mat img = image_to_cvmat(image);
    cv::namedWindow(image.name, cv::WINDOW_NORMAL);
    cv::imshow(image.name, img);
    cv::waitKey(0); // se ferme après un appui sur une touche
}

// Afficher une image en niveaux de gris
void grey_image_show(grey_image_t grey_image) {
    image_t image = grey_image_to_image(grey_image);
    cv::Mat img = image_to_cvmat(image);
    cv::namedWindow(image.name, cv::WINDOW_NORMAL);
    cv::imshow(image.name, img);
    cv::waitKey(0); // se ferme après un appui sur une touche
    image_free(image);
}


// Réduire la taille d'un image_t
image_t image_resize(image_t image, int scale) {
    assert(scale > 0);
    int new_rows = image.rows / scale;
    int new_cols = image.cols / scale;
    image_t scaled = {
        .name = image.name,
        .rows = new_rows,
        .cols = new_cols,
        .pixels = (pixel_t**) malloc(sizeof(pixel_t*) * new_rows),
    };
    // Parcours des pixels
    for (int i = 0; i < new_rows; i ++) {
        scaled.pixels[i] = (pixel_t *) malloc(sizeof(pixel_t) * new_cols);
        for(int j = 0; j < new_cols; j ++) {
            // On fait une moyenne des pixels voisins (en partant du pixel en haut à gauche)
            pixel_t pixel_moyen = {.r = 0, .g = 0, .b = 0};
            int count = 0;
            for (int x = 0; x < scale && i*scale+x < image.rows; x++) {
                for (int y = 0; y < scale && j*scale+y < image.cols; y++) {
                    pixel_t p = image.pixels[i*scale+x][j*scale+y];
                    pixel_moyen.r += p.r;
                    pixel_moyen.g += p.g;
                    pixel_moyen.b += p.b;
                    count++;
                }
            }
            pixel_moyen.r /= count;
            pixel_moyen.g /= count;
            pixel_moyen.b /= count;
            scaled.pixels[i][j] = pixel_moyen;
        }
    }
    return scaled;
}

// Réduire la taille d'un grey_image_t
grey_image_t grey_image_resize(grey_image_t image, int scale) {
    assert(scale > 0);
    int new_rows = image.rows / scale;
    int new_cols = image.cols / scale;
    grey_image_t scaled = {
        .name = image.name,
        .rows = new_rows,
        .cols = new_cols,
        .pixels = (double**) malloc(sizeof(double*) * new_rows),
    };
    // Parcours des pixels
    for (int i = 0; i < new_rows; i ++) {
        scaled.pixels[i] = (double *) malloc(sizeof(double) * new_cols);
        for(int j = 0; j < new_cols; j ++) {
            // On fait une moyenne des pixels voisins (en partant du pixel en haut à gauche)
            double pixel_moyen = 0;
            int count = 0;
            for (int x = 0; x < scale && i*scale+x < image.rows; x++) {
                for (int y = 0; y < scale && j*scale+y < image.cols; y++) {
                    double p = image.pixels[i*scale+x][j*scale+y];
                    pixel_moyen += p;
                    count++;
                }
            }
            pixel_moyen /= count;
            scaled.pixels[i][j] = pixel_moyen;
        }
    }
    return scaled;
}


// Libérer un kernel_t
void kernel_free(kernel_t kernel) {
    free(kernel.data);
}

// Créer un noyau gaussien de taille size et d'écart-type sigma
kernel_t create_gaussian_kernel(int size, double sigma) {
    kernel_t kernel = {
        .size = size,
        .data = (double*) malloc(sizeof(double) * size * size)
    };
    double mean = size / 2;
    double sum = 0.0;
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            kernel.data[x * size + y] = exp(-0.5 * (pow((x - mean) / sigma, 2.0) + pow((y - mean) / sigma, 2.0))) / (2 * M_PI * sigma * sigma);
            sum += kernel.data[x * size + y];
        }
    }
    // Normalisation du noyau
    for (int i = 0; i < size * size; i++) {
        kernel.data[i] /= sum;
    }
    return kernel;
}

// Créer le noyau de Sobel selon l'axe des x
kernel_t create_sobel_kernel_x() {
    kernel_t kernel = {
        .size = 3,
        .data = (double*) malloc(sizeof(double) * 9)
    };
    kernel.data[0] = -1; kernel.data[1] = -2; kernel.data[2] = -1;
    kernel.data[3] = 0; kernel.data[4] = 0; kernel.data[5] = 0;
    kernel.data[6] = 1; kernel.data[7] = 2; kernel.data[8] = 1;
    return kernel;
}

// Créer le noyau de Sobel selon l'axe des y
kernel_t create_sobel_kernel_y() {
    kernel_t kernel = {
        .size = 3,
        .data = (double*) malloc(sizeof(double) * 9)
    };
    kernel.data[0] = -1; kernel.data[1] = 0; kernel.data[2] = 1;
    kernel.data[3] = -2; kernel.data[4] = 0; kernel.data[5] = 2;
    kernel.data[6] = -1; kernel.data[7] = 0; kernel.data[8] = 1;
    return kernel;
}



// Appliquer un filtre à un grey_image_t (avec extension de l'image)
grey_image_t grey_image_apply_filter(grey_image_t image, kernel_t kernel) {
    grey_image_t result = grey_image_copy(image);
    int border = kernel.size / 2; // on n'appliquera pas le filtre sur les pixels au bord de l'image
    // parcours des pixels de l'image
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            // parcours des pixels du noyau
            double intensity = 0;
            for (int x = 0; x < kernel.size; x++) {
                for (int y = 0; y < kernel.size; y++) {
                    double pixel;
                    // extension de l'image pour traiter les bords
                    if (i + x < border && j + y < border) {
                        pixel = image.pixels[0][0];
                    }
                    else if (i + x < border && j + y >= image.cols - border) {
                        pixel = image.pixels[0][image.cols - 1];
                    } 
                    else if (i + x >= image.rows - border && j + y < border) {
                        pixel = image.pixels[image.rows - 1][0];
                    } 
                    else if (i + x >= image.rows - border && j + y >= image.cols - border) {
                        pixel = image.pixels[image.rows - 1][image.cols - 1];
                    } 
                    else if (i + x < border) {
                        pixel = image.pixels[0][j + y - border];
                    } 
                    else if (i + x >= image.rows - border) {
                        pixel = image.pixels[image.rows - 1][j + y - border];
                    } 
                    else if (j + y < border) {
                        pixel = image.pixels[i + x - border][0];
                    } 
                    else if (j + y >= image.cols - border) {
                        pixel = image.pixels[i + x - border][image.cols - 1];
                    } 
                    // extension non nécessaire
                    else {
                        pixel = image.pixels[i + x - border][j + y - border];
                    }
                    intensity += pixel * kernel.data[x * kernel.size + y];
                }
            }
            result.pixels[i][j] = intensity;
        }
    }
    return result;
}

// Appliquer un filtre de Sobel à un grey_image_t (avec extension de l'image) et calcule les gradients
void grey_image_apply_sobel(grey_image_t image, grey_image_t* gradient_x, grey_image_t* gradient_y) {
    kernel_t kernel_x = create_sobel_kernel_x();
    kernel_t kernel_y = create_sobel_kernel_y();

    grey_image_t copy1 = grey_image_copy(image);
    grey_image_t copy2 = grey_image_copy(image);
    *gradient_x = grey_image_apply_filter(copy1, kernel_x);
    *gradient_y = grey_image_apply_filter(copy2, kernel_y);
    grey_image_free(copy1);
    grey_image_free(copy2);

    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            image.pixels[i][j] = sqrt(pow(gradient_x->pixels[i][j], 2) + pow(gradient_y->pixels[i][j], 2));
        }
    }
    kernel_free(kernel_x);
    kernel_free(kernel_y);
}

// Calculer la direction des gradients
grey_image_t grey_image_compute_gradient_direction(grey_image_t gradient_x, grey_image_t gradient_y) {
    grey_image_t direction = grey_image_copy(gradient_x);

    for (int i = 0; i < gradient_x.rows; i++) {
        for (int j = 0; j < gradient_x.cols; j++) {
            direction.pixels[i][j] = atan2(gradient_x.pixels[i][j], gradient_y.pixels[i][j]);
        }
    }
    grey_image_free(gradient_x);
    grey_image_free(gradient_y);
    return direction;

}

// Suppression des non-maxima locaux
grey_image_t grey_image_non_maxima_suppression(grey_image_t image, grey_image_t direction) {
    grey_image_t result = grey_image_copy(image);

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
    grey_image_free(direction);
    return result;
}

// Appliquer un double seuil
void grey_image_double_threshold(grey_image_t image, double t_max, double t_min) {
    for (int i = 1; i < image.rows-1; i++) {
        for (int j = 1; j < image.cols-1; j++) {
            // Si l'intensité est assez forte on garde le pixel
            if (image.pixels[i][j] > t_max) {
                image.pixels[i][j] = 1;
            }
            // Si elle est trop faible on le supprime
            else if (image.pixels[i][j] < t_min) {
                image.pixels[i][j] = 0;
            }
            // Si elle est entre les deux seuils on regardera si un voisin est assez fort
            else {
                image.pixels[i][j] = 1/2.;
            }
        }
    }
}



void grey_image_hysteresis_aux(grey_image_t image, bool** visited, int i, int j, queue_t* queue) {
    position_t* pos = (position_t*) malloc(sizeof(position_t));
    pos->i = i;
    pos->j = j;
    queue_enqueue(queue, (void*) pos);
    visited[i][j] = true;

    while (!queue_is_empty(queue)) {
        position_t* p = (position_t*) queue_dequeue(queue);

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
void grey_image_hysteresis(grey_image_t image) {
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

    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            if (!visited[i][j] && image.pixels[i][j] == 1) {
                grey_image_hysteresis_aux(image, visited, i, j, queue);
            }
        }
    }

    // Libérer les ressources
    for (int i = 0; i < image.rows; i++) {
        free(visited[i]);
    }
    free(visited);
    queue_free(queue);
}


// Epaissir les contours
void grey_image_thicken(grey_image_t image, int n) {
    grey_image_t copy = grey_image_copy(image);

    for (int i = 0; i < image.rows-1; i++) {
        for (int j = 0; j < image.cols-1; j++) {
            if (copy.pixels[i][j] == 1) {
                for (int x = -n; x <= n; x++) {
                    for (int y = -n; y <= n; y++) {
                        if (i+x >= 0 && i+x < image.rows && j+y >= 0 && j+y < image.cols) {
                            image.pixels[i+x][j+y] = 1;
                        }
                    }
                }
            }
        }
    }
}


/* PARCOURS DE L'IMAGE */
queue_t* solve_dijkstra(grey_image_t image, position_t s, position_t t) {
    // Initialiser les tableaux
    position_t** pred = (position_t**) malloc(sizeof(position_t*) * image.rows);
    double** dis = (double**) malloc(sizeof(double*) * image.rows);
    bool** visited = (bool**) malloc(sizeof(bool*) * image.rows);
    for (int i = 0; i < image.rows; i++) {
        pred[i] = (position_t*) malloc(sizeof(position_t) * image.cols);
        dis[i] = (double*) malloc(sizeof(double) * image.cols);
        visited[i] = (bool*) malloc(sizeof(bool*) * image.cols);
        for (int j = 0; j < image.cols; j++) {
            pred[i][j] = (position_t){.i = -1, .j = -1};
            dis[i][j] = INFINITY;
            visited[i][j] = false;
        }
    }
    dis[s.i][s.j] = 0;

    // Créer une file de priorité
    priority_queue_t* pq = pq_create(image.rows * image.cols);
    position_t* start = (position_t*) malloc(sizeof(position_t));
    start->i = s.i;
    start->j = s.j;
    pq_push(pq, 0, (void*) start);

    // Directions possibles
    int directions[4][2] = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1}, // Haut, Bas, Gauche, Droite
    };

    // Algorithme principal
    while (!pq_is_empty(pq)) {
        position_t* p = (position_t*) pq_pop(pq);

        if (visited[p->i][p->j]) continue;
        visited[p->i][p->j] = true;

        // Arrêter si on atteint la cible
        if (p->i == t.i && p->j == t.j) break;

        // Mettre à jour les voisins
        for (int d = 0; d < 4; d++) {
            int ni = p->i + directions[d][0];
            int nj = p->j + directions[d][1];

            if (ni >= 0 && ni < image.rows && nj >= 0 && nj < image.cols &&
                image.pixels[ni][nj] == 0 && !visited[ni][nj]) {
                double new_dist = dis[p->i][p->j] + 1;
                if (new_dist < dis[ni][nj]) {
                    dis[ni][nj] = new_dist;
                    pred[ni][nj] = *p;

                    position_t* pos = (position_t*) malloc(sizeof(position_t));
                    pos->i = ni;
                    pos->j = nj;

                    pq_push(pq, new_dist, (void*) pos);
                }
            }
        }
        free(p);
    }
    pq_free(pq);
    
    // Reconstruire le chemin
    queue_t* solution = queue_create();
    
    position_t* current = (position_t*) malloc(sizeof(position_t));
    current->i = t.i;
    current->j = t.j;
    while (current->i != -1 && current->j != -1) {
        queue_enqueue(solution, (void*) current);

        position_t pr = pred[current->i][current->j];
        current = (position_t*) malloc(sizeof(position_t));
        current->i = pr.i;
        current->j = pr.j;
    }
    free(current);

    // Libérer les ressources
    for (int i = 0; i < image.rows; i++) {
        free(pred[i]);
        free(dis[i]);
        free(visited[i]);
    }
    free(pred);
    free(dis);
    free(visited);

    return solution;
}

void draw_solution(image_t original_image, queue_t* solution, pixel_t pixel, int n) {
    // On crée une image de la même taille que l'image d'origine
    image_t copy = image_copy(original_image);
    
    // On parcourt la liste de maillons
    while (!queue_is_empty(solution)) {
        // On récupère le pixel courant
        position_t* p = (position_t*) queue_dequeue(solution);
        for (int i = p->i*n; i < (p->i+n)*n && i < copy.rows; i++) {
            for (int j = p->j*n; j < (p->j+n)*n && j < copy.cols; j++) {
                copy.pixels[i][j] = pixel;
            }
        }
    }

    // On affiche l'image
    image_show(copy);
    image_write(copy, "solution.png");

    // On libère la liste de maillons
    queue_free(solution);

    // On libère l'image
    image_free(copy);
}

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