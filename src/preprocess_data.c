#include "preprocess_data.h"

/**
 * Convert from the big endian format in the dataset if we're on a little endian
 * machine.
 */
uint32_t map_uint32(uint32_t in)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return (
        ((in & 0xFF000000) >> 24) |
        ((in & 0x00FF0000) >> 8) |
        ((in & 0x0000FF00) << 8) |
        ((in & 0x000000FF) << 24));
#else
    return in;
#endif
}

train_dataset_t *train_read_files(const char *images, const char *labels)
{
    FILE *stream;
    train_dataset_t *dataset = malloc(sizeof(train_dataset_t));
    if (dataset == NULL){
        fprintf(stderr, "Could not allocate");
        return NULL;
    }
    uint32_t magic_number_labels;
    uint32_t number_of_labels;
    uint32_t magic_number_images;
    uint32_t number_of_images;
    uint32_t number_of_rows;
    uint32_t number_of_columns;
    uint8_t raw[IMAGE_SIZE];

    // Reading labels file
    stream = fopen(labels, "rb");
    if (NULL == stream)
    {
        fprintf(stderr, "Could not open file: %s\n", labels);
        free(dataset);
        return NULL;
    }

    if (1 != fread(&magic_number_labels, sizeof(magic_number_labels), 1, stream))
    {
        fprintf(stderr, "Could not read label file magic number header from: %s\n", labels);
        fclose(stream);
        free(dataset);
        return NULL;
    }
    if (1 != fread(&number_of_labels, sizeof(number_of_labels), 1, stream))
    {
        fprintf(stderr, "Could not read label file number of labels from: %s\n", labels);
        fclose(stream);
        free(dataset);
        return NULL;
    }
    magic_number_labels = map_uint32(magic_number_labels);
    number_of_labels = map_uint32(number_of_labels);
    if (LABEL_MAGIC != magic_number_labels)
    {
        fprintf(stderr, "Invalid header read from label file: %s (%08X not %08X)\n", labels, magic_number_labels, LABEL_MAGIC);
        fclose(stream);
        free(dataset);
        return NULL;
    }
    if (TRAIN_SIZE > number_of_labels)
    {
        fprintf(stderr, "Error, too many labels samples asked");
        fclose(stream);
        free(dataset);
        return NULL;
    }
    if (TRAIN_SIZE != fread(dataset->label, 1, TRAIN_SIZE, stream))
    {
        fprintf(stderr, "Could not read %d labels from: %s\n", TRAIN_SIZE, labels);
        fclose(stream);
        free(dataset);
        return NULL;
    }
    fclose(stream);

    // Reading images
    stream = fopen(images, "rb");
    if (NULL == stream)
    {
        fprintf(stderr, "Could not open file: %s\n", images);
        free(dataset);
        return NULL;
    }
    if (1 != fread(&magic_number_images, sizeof(magic_number_images), 1, stream))
    {
        fprintf(stderr, "Could not read label file magic number header from: %s\n", images);
        fclose(stream);
        free(dataset);
        return NULL;
    }
    if (1 != fread(&number_of_images, sizeof(number_of_images), 1, stream))
    {
        fprintf(stderr, "Could not read label file number of labels from: %s\n", images);
        fclose(stream);
        free(dataset);
        return NULL;
    }
    if (1 != fread(&number_of_rows, sizeof(number_of_rows), 1, stream))
    {
        fprintf(stderr, "Could not read label file magic number header from: %s\n", images);
        fclose(stream);
        free(dataset);
        return NULL;
    }
    if (1 != fread(&number_of_columns, sizeof(number_of_columns), 1, stream))
    {
        fprintf(stderr, "Could not read label file magic number header from: %s\n", images);
        fclose(stream);
        free(dataset);
        return NULL;
    }
    magic_number_images = map_uint32(magic_number_images);
    number_of_images = map_uint32(number_of_images);
    number_of_rows = map_uint32(number_of_rows);
    number_of_columns = map_uint32(number_of_columns);
    if (IMAGE_MAGIC != magic_number_images)
    {
        fprintf(stderr, "Invalid header read from image file: %s (%08X not %08X)\n", images, magic_number_images, IMAGE_MAGIC);
        fclose(stream);
        free(dataset);
        return NULL;
    }

    if (IMAGE_WIDTH != number_of_rows)
    {
        fprintf(stderr, "Invalid number of image rows in image file %s (%d not %d)\n", images, number_of_rows, IMAGE_WIDTH);
    }

    if (IMAGE_HEIGHT != number_of_columns)
    {
        fprintf(stderr, "Invalid number of image columns in image file %s (%d not %d)\n", images, number_of_columns, IMAGE_HEIGHT);
    }
    if (TRAIN_SIZE > number_of_images)
    {
        fprintf(stderr, "Error, too many images samples asked");
        fclose(stream);
        free(dataset);
        return NULL;
    }

    for (int i = 0; i < TRAIN_SIZE; i++)
    {
        if (1 != fread(raw, sizeof(raw), 1, stream))
        {
            fprintf(stderr, "Could not read image %d\n", i);
            fclose(stream);
            free(dataset);
            return NULL;
        }

        // Normalisation to my dataset
        for (int j = 0; j < IMAGE_SIZE; j++)
        {
            dataset->images[i][j] = (float)raw[j] / 255.0f;
        }
    }
    fclose(stream);
    return dataset;
}

void train_data_print(train_dataset_t *dataset, int i, FILE *fd)
{
    fprintf(fd, "Label : %d\n", dataset->label[i]);
    for (int j = 0; j < IMAGE_SIZE; j++)
    {
        if (j % IMAGE_HEIGHT == 0)
        {
            fprintf(fd, "\n");
        }
        fprintf(fd, "%f ", dataset->images[i][j]);
    }
    fprintf(fd, "\n");
}

test_dataset_t *test_read_files(const char *images, const char *labels)
{
    FILE *stream;
    test_dataset_t *dataset = malloc(sizeof(test_dataset_t));
    if (dataset == NULL){
        fprintf(stderr, "Could not allocate");
        return NULL;
    }
    uint32_t magic_number_labels;
    uint32_t number_of_labels;
    uint32_t magic_number_images;
    uint32_t number_of_images;
    uint32_t number_of_rows;
    uint32_t number_of_columns;
    uint8_t raw[IMAGE_SIZE];

    // Reading labels file
    stream = fopen(labels, "rb");
    if (NULL == stream)
    {
        fprintf(stderr, "Could not open file: %s\n", labels);
        free(dataset);
        return NULL;
    }

    if (1 != fread(&magic_number_labels, sizeof(magic_number_labels), 1, stream))
    {
        fprintf(stderr, "Could not read label file magic number header from: %s\n", labels);
        fclose(stream);
        free(dataset);
        return NULL;
    }
    if (1 != fread(&number_of_labels, sizeof(number_of_labels), 1, stream))
    {
        fprintf(stderr, "Could not read label file number of labels from: %s\n", labels);
        fclose(stream);
        free(dataset);
        return NULL;
    }
    magic_number_labels = map_uint32(magic_number_labels);
    number_of_labels = map_uint32(number_of_labels);
    if (LABEL_MAGIC != magic_number_labels)
    {
        fprintf(stderr, "Invalid header read from label file: %s (%08X not %08X)\n", labels, magic_number_labels, LABEL_MAGIC);
        fclose(stream);
        free(dataset);
        return NULL;
    }
    if (TEST_SIZE > number_of_labels)
    {
        fprintf(stderr, "Error, too many labels samples asked");
        fclose(stream);
        free(dataset);
        return NULL;
    }
    if (TEST_SIZE != fread(dataset->label, 1, TEST_SIZE, stream))
    {
        fprintf(stderr, "Could not read %d labels from: %s\n", TEST_SIZE, labels);
        fclose(stream);
        free(dataset);
        return NULL;
    }
    fclose(stream);

    // Reading images
    stream = fopen(images, "rb");
    if (NULL == stream)
    {
        fprintf(stderr, "Could not open file: %s\n", images);
        free(dataset);
        return NULL;
    }
    if (1 != fread(&magic_number_images, sizeof(magic_number_images), 1, stream))
    {
        fprintf(stderr, "Could not read label file magic number header from: %s\n", images);
        fclose(stream);
        free(dataset);
        return NULL;
    }
    if (1 != fread(&number_of_images, sizeof(number_of_images), 1, stream))
    {
        fprintf(stderr, "Could not read label file number of labels from: %s\n", images);
        fclose(stream);
        free(dataset);
        return NULL;
    }
    if (1 != fread(&number_of_rows, sizeof(number_of_rows), 1, stream))
    {
        fprintf(stderr, "Could not read label file magic number header from: %s\n", images);
        fclose(stream);
        free(dataset);
        return NULL;
    }
    if (1 != fread(&number_of_columns, sizeof(number_of_columns), 1, stream))
    {
        fprintf(stderr, "Could not read label file magic number header from: %s\n", images);
        fclose(stream);
        free(dataset);
        return NULL;
    }
    magic_number_images = map_uint32(magic_number_images);
    number_of_images = map_uint32(number_of_images);
    number_of_rows = map_uint32(number_of_rows);
    number_of_columns = map_uint32(number_of_columns);
    if (IMAGE_MAGIC != magic_number_images)
    {
        fprintf(stderr, "Invalid header read from image file: %s (%08X not %08X)\n", images, magic_number_images, IMAGE_MAGIC);
        fclose(stream);
        free(dataset);
        return NULL;
    }

    if (IMAGE_WIDTH != number_of_rows)
    {
        fprintf(stderr, "Invalid number of image rows in image file %s (%d not %d)\n", images, number_of_rows, IMAGE_WIDTH);
    }

    if (IMAGE_HEIGHT != number_of_columns)
    {
        fprintf(stderr, "Invalid number of image columns in image file %s (%d not %d)\n", images, number_of_columns, IMAGE_HEIGHT);
    }
    if (TEST_SIZE > number_of_images)
    {
        fprintf(stderr, "Error, too many images samples asked");
        fclose(stream);
        free(dataset);
        return NULL;
    }

    for (int i = 0; i < TEST_SIZE; i++)
    {
        if (1 != fread(raw, sizeof(raw), 1, stream))
        {
            fprintf(stderr, "Could not read image %d\n", i);
            fclose(stream);
            free(dataset);
            return NULL;
        }

        // Normalisation to my dataset
        for (int j = 0; j < IMAGE_SIZE; j++)
        {
            dataset->images[i][j] = (float)raw[j] / 255.0f;
        }
    }
    fclose(stream);
    return dataset;
}

void test_data_print(test_dataset_t *dataset, int i, FILE *fd)
{
    fprintf(fd, "Label : %d\n", dataset->label[i]);
    for (int j = 0; j < IMAGE_SIZE; j++)
    {
        if (j % IMAGE_HEIGHT == 0)
        {
            fprintf(fd, "\n");
        }
        fprintf(fd, "%f ", dataset->images[i][j]);
    }
    fprintf(fd, "\n");
}