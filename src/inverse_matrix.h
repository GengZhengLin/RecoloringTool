#ifndef INVERSE_MATRIX_H
#define INVERSE_MATRIX_H

#include<string.h>
void inverseMatrix(double input[5][5],double output[5][5],int dimension);
void swap(double input[5][5],double output[5][5],int dimension,int first_row,int second_row);
void reorderOutput(double output[5][5],int dimension);

#endif // INVERSE_MATRIX_H

