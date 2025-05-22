#ifndef IMAGE_USAGE_H
#define IMAGE_USAGE_H

#include "image.h"
#include "queue.h"
#include "priority_queue.h"
#include "common.h"


// Crée un noyau gaussien
kernel_t create_gaussian_kernel(int size, double sigma);

// Crée le noyau de Sobel pour l'axe x
kernel_t create_sobel_kernel_x();

// Crée le noyau de Sobel pour l'axe y
kernel_t create_sobel_kernel_y();

// Applique un filtre de Sobel pour calculer les gradients
void image_apply_sobel(image_t image, image_t* gradient_x, image_t* gradient_y);

// Calcule la direction des gradients
image_t image_compute_gradient_direction(image_t gradient_x, image_t gradient_y);

// Supprime les non-maxima locaux
image_t image_non_maxima_suppression(image_t image, image_t direction);

// Applique un double seuil à une image
void image_double_threshold(image_t image, double t_max, double t_min);

// Applique une hystérésis pour tracer les contours
void image_hysteresis(image_t image);

// Application du filtre de Canny
image_t canny(image_t image, double t_max, double t_min);

#endif // IMAGE_USAGE_H