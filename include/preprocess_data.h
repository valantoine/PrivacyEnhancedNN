#ifndef PREPROCESS_DATA_H
#define PREPROCESS_DATA_H
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>


#define LABEL_MAGIC 0x00000801
#define IMAGE_MAGIC 0x00000803
#define TEST_SIZE 10000
#define TRAIN_SIZE 60000
#define IMAGE_WIDTH 28
#define IMAGE_HEIGHT 28
#define IMAGE_SIZE 784


typedef struct{
    float image[IMAGE_SIZE];
    uint8_t label;
} image_t;

typedef struct{
    image_t *images;
    size_t size;
} dataset_t;



void image_raw_print(image_t *image, FILE *fd);

extern void image_print(image_t *image, FILE *fd);

extern dataset_t *dataset_read(const char *images, const char *labels, size_t size);

extern void dataset_free(dataset_t *dataset);

extern dataset_t *dataset_alloc(size_t size);

extern void dataset_shuffle(dataset_t *dataset);

#endif /*PREPROCESS_DATA_H*/