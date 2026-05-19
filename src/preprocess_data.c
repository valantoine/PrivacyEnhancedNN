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

dataset_t *dataset_alloc(size_t size)
{
    dataset_t *dataset = malloc(sizeof(dataset_t));
    if (dataset == NULL)
    {
        fprintf(stderr, "Could not allocate");
        return NULL;
    }
    dataset->images = malloc(size * sizeof(image_t));
    if (dataset->images == NULL)
    {
        fprintf(stderr, "Could not allocate");
        free(dataset);
        return NULL;
    }
    dataset->size = size;
    return dataset;
}

void dataset_free(dataset_t *dataset)
{
    if (dataset == NULL)
    {
        fprintf(stderr, "Could not free dataset");
        return;
    }
    free(dataset->images);
    free(dataset);
}

dataset_t *dataset_read(const char *images, const char *labels, size_t size)
{
    FILE *stream;
    dataset_t *dataset = dataset_alloc(size);
    uint32_t magic_number_labels;
    uint32_t number_of_labels;
    uint32_t magic_number_images;
    uint32_t number_of_images;
    uint32_t number_of_rows;
    uint32_t number_of_columns;
    uint8_t raw[IMAGE_SIZE];

    // Reading labels file
    stream = fopen(labels, "rb");
    if (stream == NULL)
    {
        fprintf(stderr, "Could not open file: %s\n", labels);
        dataset_free(dataset);
        return NULL;
    }

    if (fread(&magic_number_labels, sizeof(magic_number_labels), 1, stream) != 1)
    {
        fprintf(stderr, "Could not read label file magic number header from: %s\n", labels);
        fclose(stream);
        dataset_free(dataset);
        return NULL;
    }
    if (fread(&number_of_labels, sizeof(number_of_labels), 1, stream) != 1)
    {
        fprintf(stderr, "Could not read label file number of labels from: %s\n", labels);
        fclose(stream);
        dataset_free(dataset);
        return NULL;
    }
    magic_number_labels = map_uint32(magic_number_labels);
    number_of_labels = map_uint32(number_of_labels);
    if (LABEL_MAGIC != magic_number_labels)
    {
        fprintf(stderr, "Invalid header read from label file: %s (%08X not %08X)\n", labels, magic_number_labels, LABEL_MAGIC);
        fclose(stream);
        dataset_free(dataset);
        return NULL;
    }
    if (size > number_of_labels)
    {
        fprintf(stderr, "Error, too many labels samples asked");
        fclose(stream);
        dataset_free(dataset);
        return NULL;
    }
    // Copying labels
    for (size_t i = 0; i < size; i++)
    {
        if (fread(&dataset->images[i].label, 1, 1, stream) != 1)
        {
            fprintf(stderr, "Could not read label %zu\n", i);
            fclose(stream);
            dataset_free(dataset);
            return NULL;
        }
    }

    fclose(stream);

    // Reading images
    stream = fopen(images, "rb");
    if (NULL == stream)
    {
        fprintf(stderr, "Could not open file: %s\n", images);
        dataset_free(dataset);
        return NULL;
    }
    if (1 != fread(&magic_number_images, sizeof(magic_number_images), 1, stream))
    {
        fprintf(stderr, "Could not read label file magic number header from: %s\n", images);
        fclose(stream);
        dataset_free(dataset);
        return NULL;
    }
    if (1 != fread(&number_of_images, sizeof(number_of_images), 1, stream))
    {
        fprintf(stderr, "Could not read label file number of labels from: %s\n", images);
        fclose(stream);
        dataset_free(dataset);
        return NULL;
    }
    if (1 != fread(&number_of_rows, sizeof(number_of_rows), 1, stream))
    {
        fprintf(stderr, "Could not read label file magic number header from: %s\n", images);
        fclose(stream);
        dataset_free(dataset);
        return NULL;
    }
    if (1 != fread(&number_of_columns, sizeof(number_of_columns), 1, stream))
    {
        fprintf(stderr, "Could not read label file magic number header from: %s\n", images);
        fclose(stream);
        dataset_free(dataset);
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
        dataset_free(dataset);
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
    if (size > number_of_images)
    {
        fprintf(stderr, "Error, too many images samples asked");
        fclose(stream);
        dataset_free(dataset);
        return NULL;
    }
    // Copying image and normalization
    for (size_t i = 0; i < size; i++)
    {
        if (1 != fread(raw, sizeof(raw), 1, stream))
        {
            fprintf(stderr, "Could not read image %zu\n", i);
            fclose(stream);
            dataset_free(dataset);
            return NULL;
        }

        // Normalisation to my dataset
        for (int j = 0; j < IMAGE_SIZE; j++)
        {
            dataset->images[i].image[j] = (float)raw[j] / 255.0f;
        }
    }
    fclose(stream);
    return dataset;
}

void image_print(image_t *image, FILE *fd)
{
    fprintf(fd, "Label : %d\n", image->label);
    for (int j = 0; j < IMAGE_SIZE; j++)
    {
        if (j % IMAGE_HEIGHT == 0)
        {
            fprintf(fd, "\n");
        }
        if (image->image[j] != 0.0)
        {
            fprintf(fd, "*");
        }
        else
        {
            fprintf(fd, " ");
        }
    }
    fprintf(fd, "\n");
}

void dataset_shuffle(dataset_t *dataset)
{
    if (dataset->size > 1)
    {
        for (size_t i = 0; i < dataset->size - 1; i++)
        {
            size_t j = i + rand() / (RAND_MAX / (dataset->size - i) + 1);
            image_t temp = dataset->images[i];
            dataset->images[i] = dataset->images[j];
            dataset->images[j] = temp;
        }
    }
}