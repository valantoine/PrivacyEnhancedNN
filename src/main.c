#include "main.h"
int main(void)
{
    srand(time(NULL));
    dataset_t *train_dataset = dataset_read(IMAGES_TRAIN_PATH, LABELS_TRAIN_PATH, TRAIN_SIZE);
    // image_print(&train_dataset->images[1], stdout);
    parameters_t *parameters = init_parameters();
    for (int i = 70; i < 100; i++)
    {
        image_print(&train_dataset->images[i], stdout);
    }

    train(train_dataset, parameters);
    dataset_free(train_dataset);

    dataset_t *test_dataset = dataset_read(IMAGES_TEST_PATH, LABELS_TEST_PATH, TEST_SIZE);
    in_matrix_t output_all;
    output_all.vectors = malloc(test_dataset->size * sizeof(in_vector_t));

    feed_forward(&output_all, parameters, test_dataset);
    float acc = accuracy(test_dataset, &output_all);
    fprintf(stdout, "\n Accuracy on test set : %f", acc);

    free(parameters);
    free(output_all.vectors);
    free(test_dataset);
}