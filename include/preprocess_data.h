#ifndef PREPROCESS_DATA_H
#define PREPROCESS_DATA_H
#include "lin_algebra_misc.h"

#define LABEL_MAGIC 0x00000801
#define IMAGE_MAGIC 0x00000803
#define TEST_SIZE 10000
#define TRAIN_SIZE 60000
#define IMAGE_WIDTH 28
#define IMAGE_HEIGHT 28
#define IMAGE_SIZE 784

typedef struct{
    float images[TRAIN_SIZE][IMAGE_SIZE];
    uint8_t label[TRAIN_SIZE];
} train_dataset_t;

typedef struct{
    float images[TEST_SIZE][IMAGE_SIZE];
    uint8_t label[TEST_SIZE];
} test_dataset_t;




extern train_dataset_t *train_read_files(const char *images, const char *labels);

extern void train_data_print(train_dataset_t *dataset, int i, FILE *fd);

extern test_dataset_t *test_read_files(const char *images, const char *labels);

extern void test_data_print(test_dataset_t *dataset, int i, FILE *fd);

#endif /*PREPROCESS_DATA_H*/