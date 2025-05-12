#include <opencv2/opencv.hpp>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "image.h"


// Fonctions de gestion de la mémoire

// Libérer la mémoire d'une image en niveaux de gris
void image_free(image_t image) {
    for (int i = 0; i < image.rows; i++) {
        free(image.pixels[i]);
    }
    free(image.pixels);
}

// Libérer la mémoire d'une image colorée
void colored_image_free(colored_image_t image) {
    for (int i = 0; i < image.rows; i++) {
        free(image.pixels[i]);
    }
    free(image.pixels);
}

// Libérer la mémoire d'un noyau de convolution
void kernel_free(kernel_t kernel) {
    for (int i = 0; i < kernel.size; i++) {
        free(kernel.data[i]);
    }
    free(kernel.data);
}

// Fonctions de conversion

// Convertir un cv::Mat en colored_image_t
colored_image_t colored_image_from_mat(cv::Mat mat) {
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
    return image;
}

// Convertir un colored_image_t en cv::Mat
cv::Mat cvmat_from_colored_image(colored_image_t colored_image) {
    cv::Mat mat(colored_image.rows, colored_image.cols, CV_8UC3);

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
    return mat;
}

// Convertir un image_t en cv::Mat
cv::Mat cvmat_from_image(image_t image) {
    cv::Mat mat(image.rows, image.cols, CV_8UC1);

    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            // Convertir les pixels en valeurs entre 0 et 255
            mat.at<uchar>(i, j) = (uchar)(image.pixels[i][j] * 255.0);
        }
    }

    return mat;
}

// Convertir un colored_image_t en image_t (niveaux de gris avec pondérations)
image_t image_from_colored_image(colored_image_t colored_image) {
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
            // Pondérations standard pour convertir en niveaux de gris
            image.pixels[i][j] = 0.299 * pixel.r + 0.587 * pixel.g + 0.114 * pixel.b;
        }
    }
    return image;
}

// Fonctions d'affichage

// Afficher une image colorée
void colored_image_show(colored_image_t image) {
    cv::Mat img = cvmat_from_colored_image(image);
    cv::namedWindow(image.name, cv::WINDOW_NORMAL);
    cv::imshow(image.name, img);
    cv::waitKey(0); // se ferme après un appui sur une touche
}

// Afficher une image en niveaux de gris
void image_show(image_t image) {
    // Convertir l'image_t en cv::Mat
    cv::Mat mat = cvmat_from_image(image);

    // Afficher l'image avec OpenCV
    cv::namedWindow(image.name ? image.name : "Image en niveaux de gris", cv::WINDOW_NORMAL);
    cv::imshow(image.name ? image.name : "Image en niveaux de gris", mat);
    cv::waitKey(0); // Se ferme après un appui sur une touche
}

// Fonctions de manipulation d'images

// Réduire la taille d'une image
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
    return scaled;
}

// Appliquer un filtre à une image
image_t image_apply_filter(image_t image, kernel_t kernel) {
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
                    intensity += pixel * kernel.data[x * kernel.size + y];
                }
            }
            result.pixels[i][j] = intensity;
        }
    }
    return result;
}

// Epaissir certains pixels
image_t image_thicken(image_t image, int n, pixel_t intensity) {
    image_t copy = image_copy(image);

    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            if (copy.pixels[i][j] == intensity) {
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
    return copy;
}