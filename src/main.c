#include "main.h"
int main(void)
{
    srand(time(NULL));
    // Tests on square matrix
    int_matrix_t m1;
    int_matrix_t m2;
    int_matrix_allocate(&m1, 10, 10);
    int_matrix_allocate(&m2, 10,10);
    int_matrix_print(&m1, stdout);
    for (size_t i=0; i<m1.nb_rows; i++){
        for (size_t j=0; j<m1.nb_cols; j++){
            if (i==j){
                int_matrix_set_cell(&m1, i, j, 1);
                int_matrix_set_cell(&m2, i, j, 2);
            }
        }
    }
    int_matrix_print(&m1, stdout);
    int_matrix_print(&m2, stdout);
    int_matrix_add(&m1, &m2);
    int_matrix_print(&m1, stdout);
    int_matrix_t m3 = int_matrix_mult(&m1, &m2);
    int_matrix_print(&m3, stdout);
    
    int_matrix_free(&m1);
    int_matrix_free(&m2);
    int_matrix_free(&m3);
    
    // Tests on vectors
    int_matrix_t v1;
    int_matrix_t v2;
    int_matrix_allocate(&v1, 1, 10);
    int_matrix_allocate(&v2, 10, 1);
    for (size_t i=0; i<v1.nb_cols; i++){
        int_matrix_set_cell(&v1, 0, i, 1);
        int_matrix_set_cell(&v2, i, 0, -1000);
    }
    int_matrix_print(&v1, stdout);
    int_matrix_print(&v2, stdout);
    int_matrix_t v3 = int_matrix_mult(&v2, &v1);
    int_matrix_print(&v3, stdout);
    int_matrix_free(&v1);
    int_matrix_free(&v2);
    int_matrix_free(&v3);
    train_dataset_t *train_dataset = train_read_files(IMAGES_TRAIN_PATH, LABELS_TRAIN_PATH);
    train_data_print(train_dataset, 4, stdout);
    free(train_dataset);
    test_dataset_t *test_dataset = test_read_files(IMAGES_TEST_PATH, LABELS_TEST_PATH);
    test_data_print(test_dataset, 4, stdout);
    free(test_dataset);
}