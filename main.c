#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

// Define DWT filter coefficients
double h[2] = {0.707106781186548, 0.707106781186548};
double g[2] = {0.707106781186548, -0.707106781186548};

// Function to perform 1D DWT using filter coefficients h and g
void dwt(double* input, double* output, int n) {
    int i, j, k;
    double temp[n];
    for (i = 0; i < n; i++) {
        temp[i] = 0;
        for (j = 0, k = i; j < 2; j++, k = (k + n - 1) % n) {
            temp[i] += h[j] * input[k];
            output[i] += g[j] * input[k];
        }
    }
    for (i = 0; i < n; i++) {
        input[i] = temp[i];
    }
}

// Function to compute the sum of absolute values of DWT coefficients for an image
void compute_dwt_feature(double* image, double* feature_vector, int width, int height) {
    int i, j, k;
    double row[width], col[height];
    double sum = 0.0;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            row[j] = image[i * width + j];
        }
        dwt(row, row, width);
        for (j = 0; j < width; j++) {
            image[i * width + j] = row[j];
            sum += fabs(row[j]); // add absolute value of DWT coefficient to the sum
        }
    }
    for (j = 0; j < width; j++) {
        for (i = 0; i < height; i++) {
            col[i] = image[i * width + j];
        }
        dwt(col, col, height);
        for (i = 0; i < height; i++) {
            image[i * width + j] = col[i];
            sum += fabs(col[i]); // add absolute value of DWT coefficient to the sum
        }
    }
    * feature_vector = sum ;
}
double euclidean_distance(double* feature_vector1, double* feature_vector2, int size) {
    int i;
    double distance = 0;
    for (i = 0; i < size; i++) {
        distance += pow(feature_vector1[i] - feature_vector2[i], 2);
    }
    distance = sqrt(distance);
    return distance;
}

// Main function
int main(int argc, char** argv) {
    // Read images using STB library
    int width, height, channels;
    unsigned char* image1_data = stbi_load("16.jpg", &width, &height, &channels, 0);
    unsigned char* image2_data = stbi_load("17.jpg", &width, &height, &channels, 0);
    if (!image1_data || !image2_data) {
        printf("Error: could not load images.\n");
        return 1;
    }

    // Convert images to grayscale and normalize pixel values
    double image1[width * height], image2[width * height];
    int i, j;
    for (i = 0; i < height; i++) {
        for    (j = 0; j < width; j++) {
        int index = (i * width + j) * channels;
        double gray = 0.299 * image1_data[index] + 0.587 * image1_data[index + 1] + 0.114 * image1_data[index + 2];
        image1[i * width + j] = gray / 255.0;
        gray = 0.299 * image2_data[index] + 0.587 * image2_data[index + 1] + 0.114 * image2_data[index + 2];
        image2[i * width + j] = gray / 255.0;
    }
}

// Compute DWT coefficients for both images
double* feature_vector_1 = malloc(width * height * sizeof(double));
compute_dwt_feature(image1, feature_vector_1, width, height);

double* feature_vector_2 = malloc(width * height * sizeof(double));
compute_dwt_feature(image2, feature_vector_2, width, height);

// Compute Euclidean distance between the DWT coefficients of the two images
double distance = euclidean_distance(feature_vector_1, feature_vector_2, width * height);

// Free memory allocated for feature vectors
free(feature_vector_1);
free(feature_vector_2);

// Print the distance between the images
printf("Distance between the images: %f\n", distance);
if (distance<10000)
    printf("similar");
else
    printf("different");
// Free memory allocated by STB library
stbi_image_free(image1_data);
stbi_image_free(image2_data);

return 0;
}
