#include <opencv2/opencv.hpp>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "image.h"
#include "logging.h"

// Fonctions pratiques

// Copier une image en niveaux de gris
image_t image_copy(image_t image) {
    log_debug("Copie de l'image : %s", image.name);
    image_t copy = {
        .name = image.name,
        .rows = image.rows,
        .cols = image.cols,
        .pixels = (pixel_t**) malloc(sizeof(pixel_t*) * image.rows),
    };
    for (int i = 0; i < image.rows; i++) {
        copy.pixels[i] = (pixel_t*) malloc(sizeof(pixel_t) * image.cols);
        for (int j = 0; j < image.cols; j++) {
            copy.pixels[i][j] = image.pixels[i][j];
        }
    }
    log_debug("Image copiée : %s", image.name);
    return copy;
}

// Copier une image colorée
colored_image_t colored_image_copy(colored_image_t image) {
    log_debug("Copie de l'image colorée : %s", image.name);
    colored_image_t copy = {
        .name = image.name,
        .rows = image.rows,
        .cols = image.cols,
        .pixels = (colored_pixel_t**) malloc(sizeof(colored_pixel_t*) * image.rows),
    };
    for (int i = 0; i < image.rows; i++) {
        copy.pixels[i] = (colored_pixel_t*) malloc(sizeof(colored_pixel_t) * image.cols);
        for (int j = 0; j < image.cols; j++) {
            copy.pixels[i][j] = image.pixels[i][j];
        }
    }
    log_debug("Image colorée copiée : %s", image.name);
    return copy;
}

// Fonctions de gestion de la mémoire

// Libérer la mémoire d'une image en niveaux de gris
void image_free(image_t image) {
    log_debug("Libération de la mémoire de l'image : %s", image.name);
    for (int i = 0; i < image.rows; i++) {
        free(image.pixels[i]);
    }
    free(image.pixels);
    log_debug("Mémoire de l'image libérée : %s", image.name);
}

// Libérer la mémoire d'une image colorée
void colored_image_free(colored_image_t image) {
    log_debug("Libération de la mémoire de l'image colorée : %s", image.name);
    for (int i = 0; i < image.rows; i++) {
        free(image.pixels[i]);
    }
    free(image.pixels);
    log_debug("Mémoire de l'image colorée libérée : %s", image.name);
}

// Libérer la mémoire d'un noyau de convolution
void kernel_free(kernel_t kernel) {
    log_debug("Libération de la mémoire d'un noyau de convolution");
    for (int i = 0; i < kernel.size; i++) {
        free(kernel.data[i]);
    }
    free(kernel.data);
    log_debug("Mémoire du noyau de convolution libérée");
}

// Fonctions de conversion

// Convertir un cv::Mat en colored_image_t
colored_image_t colored_image_from_mat(cv::Mat mat) {
    log_debug("Conversion d'un cv::Mat en colored_image_t");
    if (mat.empty()) log_fatal("Erreur lors de la conversion : cv::Mat vide");

    colored_image_t image = {
        .name = NULL,
        .rows = mat.rows,
        .cols = mat.cols,
        .pixels = (colored_pixel_t**) malloc(sizeof(colored_pixel_t*) * mat.rows),
    };
    for (int i = 0; i < mat.rows; i++) {
        image.pixels[i] = (colored_pixel_t*) malloc(sizeof(colored_pixel_t) * mat.cols);
        for (int j = 0; j < mat.cols; j++) {
            cv::Vec3b color = mat.at<cv::Vec3b>(i, j);
            image.pixels[i][j] = (colored_pixel_t) {
                .r = (double) color[2],
                .g = (double) color[1],
                .b = (double) color[0]
            };
        }
    }
    log_debug("Conversion réussie : colored_image_t créé");
    return image;
}

// Convertir un colored_image_t en cv::Mat
cv::Mat cvmat_from_colored_image(colored_image_t colored_image) {
    log_debug("Conversion d'un colored_image_t en cv::Mat");

    cv::Mat mat(colored_image.rows, colored_image.cols, CV_8UC3);

    if (mat.empty()) log_fatal("Erreur lors de la conversion : cv::Mat vide");

    for (int i = 0; i < colored_image.rows; i++) {
        for (int j = 0; j < colored_image.cols; j++) {
            colored_pixel_t pixel = colored_image.pixels[i][j];
            cv::Vec3b color;
            color[0] = (uchar) (pixel.b); // Bleu
            color[1] = (uchar) (pixel.g); // Vert
            color[2] = (uchar) (pixel.r); // Rouge
            mat.at<cv::Vec3b>(i, j) = color;
        }
    }

    log_debug("Conversion réussie : cv::Mat créé");

    return mat;
}

// Convertir un image_t en cv::Mat
cv::Mat cvmat_from_image(image_t image) {
    log_debug("Conversion d'un image_t en cv::Mat");

    cv::Mat mat(image.rows, image.cols, CV_8UC1);

    if (mat.empty()) log_fatal("Erreur lors de la conversion : cv::Mat vide");

    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            // Convertir les pixels en valeurs entre 0 et 255
            mat.at<uchar>(i, j) = (uchar)(image.pixels[i][j] * 255.0);
        }
    }
    log_debug("Conversion réussie : cv::Mat créé");

    return mat;
}

// Convertir un colored_image_t en image_t (niveaux de gris avec pondérations)
image_t image_from_colored_image(colored_image_t colored_image) {
    log_debug("Conversion d'un colored_image_t en image_t : %s", colored_image.name);
    image_t image = {
        .name = colored_image.name,
        .rows = colored_image.rows,
        .cols = colored_image.cols,
        .pixels = (pixel_t**) malloc(sizeof(pixel_t*) * colored_image.rows),
    };

    for (int i = 0; i < colored_image.rows; i++) {
        image.pixels[i] = (pixel_t*) malloc(sizeof(pixel_t) * colored_image.cols);
        for (int j = 0; j < colored_image.cols; j++) {
            colored_pixel_t pixel = colored_image.pixels[i][j];
            // Pondérations standard pour convertir en niveaux de gris et normalisation
            image.pixels[i][j] = (0.299 * pixel.r + 0.587 * pixel.g + 0.114 * pixel.b) / 255.0;
        }
    }
    log_debug("Conversion réussie : %s", colored_image.name);
    return image;
}

// Fonctions de lecture et d'écriture d'images

// Lire une image en niveaux de gris
colored_image_t image_read(const char* path) {
    // Lire l'image avec OpenCV
    log_debug("Lecture de l'image : %s", path);

    cv::Mat mat = cv::imread(path, cv::IMREAD_COLOR);
    if (mat.empty()) log_fatal("Erreur lors de la lecture de l'image : %s", path);

    colored_image_t image = colored_image_from_mat(mat);
    image.name = strdup(path); // Dupliquer le nom du fichier
    return image;
}

// Ecrire une image en niveaux de gris
void image_write(image_t image, const char* path) {
    log_debug("Écriture de l'image : %s dans %s", image.name, path);

    cv::Mat img = cvmat_from_image(image);
    cv::imwrite(path, img);

    log_debug("Image écrite : %s dans %s", image.name, path);
}

// Ecrire une image colorée
void colored_image_write(colored_image_t image, const char* path) {
    log_debug("Écriture de l'image colorée : %s dans %s", image.name, path);

    cv::Mat img = cvmat_from_colored_image(image);
    cv::imwrite(path, img);

    log_debug("Image colorée écrite : %s dans %s", image.name, path);
}

// Fonctions d'affichage

// Afficher une image colorée
void colored_image_show(colored_image_t image) {
    log_debug("Affichage de l'image colorée : %s", image.name);

    cv::Mat img = cvmat_from_colored_image(image);
    cv::namedWindow(image.name, cv::WINDOW_NORMAL);
    cv::resizeWindow(image.name, 600, 600);
    cv::imshow(image.name, img);

    log_debug("Image colorée affichée (en attente d'action) : %s", image.name);
    cv::waitKey(0); // se ferme après un appui sur une touche
    log_debug("Fermeture de l'image colorée : %s", image.name);
}

// Afficher une image en niveaux de gris
void image_show(image_t image) {
    log_debug("Affichage de l'image en niveaux de gris : %s", image.name);

    // Convertir l'image_t en cv::Mat
    cv::Mat mat = cvmat_from_image(image);

    // Afficher l'image avec OpenCV
    cv::namedWindow(image.name, cv::WINDOW_NORMAL);
    cv::resizeWindow(image.name, 600, 600);
    cv::imshow(image.name, mat);
    log_debug("Image en niveaux de gris affichée (en attente d'action): %s", image.name);
    cv::waitKey(0); // Se ferme après un appui sur une touche
    log_debug("Fermeture de l'image en niveaux de gris : %s", image.name);
}

// Fonctions de manipulation d'images

// Réduire la taille d'une image
image_t image_resize(image_t image, int scale) {
    log_debug("Redimensionnement de l'image : %s avec un facteur de réduction de %d", image.name, scale);
    if (scale <= 0) log_fatal("Erreur de redimensionnement : facteur de réduction invalide (%d)", scale);

    int new_rows = image.rows / scale;
    int new_cols = image.cols / scale;
    image_t scaled = {
        .name = image.name,
        .rows = new_rows,
        .cols = new_cols,
        .pixels = (pixel_t**) malloc(sizeof(pixel_t*) * new_rows),
    };
    // Parcours des pixels
    for (int i = 0; i < new_rows; i++) {
        scaled.pixels[i] = (pixel_t*) malloc(sizeof(pixel_t) * new_cols);
        for (int j = 0; j < new_cols; j++) {
            // Moyenne des pixels voisins
            pixel_t pixel_moyen = 0;
            int count = 0;
            for (int x = 0; x < scale && i * scale + x < image.rows; x++) {
                for (int y = 0; y < scale && j * scale + y < image.cols; y++) {
                    pixel_t p = image.pixels[i * scale + x][j * scale + y];
                    pixel_moyen += p;
                    count++;
                }
            }
            pixel_moyen /= count;
            scaled.pixels[i][j] = pixel_moyen;
        }
    }
    log_debug("Image redimensionnée : %s avec un facteur de réduction de %d", image.name, scale);
    return scaled;
}

// Appliquer un filtre à une image
image_t image_apply_filter(image_t image, kernel_t kernel) {
    log_debug("Application d'un filtre à l'image : %s", image.name);
    image_t result = image_copy(image);

    int border = kernel.size / 2;
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            pixel_t intensity = 0;
            for (int x = 0; x < kernel.size; x++) {
                for (int y = 0; y < kernel.size; y++) {
                    int xi = i + x - border;
                    int yj = j + y - border;
                    pixel_t pixel = (xi >= 0 && xi < image.rows && yj >= 0 && yj < image.cols)
                                    ? image.pixels[xi][yj]
                                    : 0; // Extension par zéro
                    intensity += pixel * kernel.data[x][y];
                }
            }
            result.pixels[i][j] = intensity;
        }
    }
    log_debug("Filtre appliqué à l'image : %s", image.name);
    return result;
}

// Epaissir certains pixels
image_t image_thicken(image_t image, int n, pixel_t intensity) {
    log_debug("Épaississement de l'image : %s, facteur de %d pour les pixels d'intensité %.3f",
                image.name, n, intensity);
    image_t copy = image_copy(image);

    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            if (image.pixels[i][j] == intensity) {
                for (int x = -n; x <= n; x++) {
                    for (int y = -n; y <= n; y++) {
                        if (i + x >= 0 && i + x < image.rows && j + y >= 0 && j + y < image.cols) {
                            copy.pixels[i + x][j + y] = intensity;
                        }
                    }
                }
            }
        }
    }
    log_debug("Épaississement terminé : %s", image.name);
    return copy;
}