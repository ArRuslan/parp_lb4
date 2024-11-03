#include <bitset>
#include <iomanip>
#include <iostream>
#include <vector>
#include <omp.h>

#define MAT_SIZE 1024

using Matrix = std::vector<std::vector<int>>;
using BitMatrix = std::vector<std::bitset<MAT_SIZE>>;

Matrix addMatrices(const Matrix& A, const Matrix& B) {
    size_t rows = A.size();
    size_t cols = A[0].size();
    Matrix result(rows, std::vector<int>(cols));

    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            result[i][j] = A[i][j] + B[i][j];
        }
    }

    return result;
}

BitMatrix addBitMatrices(const BitMatrix& A, const BitMatrix& B) {
    BitMatrix result(MAT_SIZE);

    for (size_t i = 0; i < MAT_SIZE; ++i) {
        result[i] = A[i] ^ B[i];
    }

    return result;
}

Matrix multiplyMatrices(const Matrix& A, const Matrix& B) {
    size_t rows = A.size();
    size_t cols = B[0].size();
    size_t innerDim = B.size();
    Matrix result(rows, std::vector<int>(cols, 0));

    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            for (size_t k = 0; k < innerDim; ++k) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    return result;
}

BitMatrix multiplyBitMatrices(const BitMatrix& A, const BitMatrix& B) {
    BitMatrix result(MAT_SIZE);

    BitMatrix B_transpose(MAT_SIZE);
    for (size_t i = 0; i < MAT_SIZE; ++i) {
        for (size_t j = 0; j < MAT_SIZE; ++j) {
            B_transpose[i][j] = B[j][i];
        }
    }

    for (size_t i = 0; i < MAT_SIZE; ++i) {
        for (size_t j = 0; j < MAT_SIZE; ++j) {
            bool sum = false;
            for (size_t k = 0; k < MAT_SIZE; ++k) {
                sum ^= (A[i][k] & B_transpose[j][k]);
            }
            result[i][j] = sum;
        }
    }

    return result;
}

int main() {
    Matrix mat_a(MAT_SIZE, std::vector<int>(MAT_SIZE, 0));
    Matrix mat_b(MAT_SIZE, std::vector<int>(MAT_SIZE, 0));
    BitMatrix bit_mat_a(MAT_SIZE);
    BitMatrix bit_mat_b(MAT_SIZE);

    for(int i = 0; i < MAT_SIZE; i++) {
        for(int j = 0; j < MAT_SIZE; j++) {
            mat_a[i][j] = rand();
            mat_b[i][j] = rand();

            bit_mat_a[i][j] = rand() % 2;
            bit_mat_b[i][j] = rand() % 2;
        }
    }

    double start_time = omp_get_wtime();
    addMatrices(mat_a, mat_b);
    double end_time = omp_get_wtime();
    std::cout << "Matrix " << MAT_SIZE << "x" << MAT_SIZE << " add: " << std::fixed << std::setprecision(3) << end_time - start_time << " seconds\n";

    start_time = omp_get_wtime();
    addBitMatrices(bit_mat_a, bit_mat_b);
    end_time = omp_get_wtime();
    std::cout << "Bit matrix " << MAT_SIZE << "x" << MAT_SIZE << " add: " << std::fixed << std::setprecision(3) << end_time - start_time << " seconds\n";

    start_time = omp_get_wtime();
    multiplyMatrices(mat_a, mat_b);
    end_time = omp_get_wtime();
    std::cout << "Matrix " << MAT_SIZE << "x" << MAT_SIZE << " multiply: " << std::fixed << std::setprecision(3) << end_time - start_time << " seconds\n";

    start_time = omp_get_wtime();
    multiplyBitMatrices(bit_mat_a, bit_mat_b);
    end_time = omp_get_wtime();
    std::cout << "Bit matrix " << MAT_SIZE << "x" << MAT_SIZE << " multiply: " << std::fixed << std::setprecision(3) << end_time - start_time << " seconds\n";

    return 0;
}
