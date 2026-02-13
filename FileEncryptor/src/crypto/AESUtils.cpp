#include "AESUtils.h"

State AESUtils::convertToMatrix(const Block& block_data) {
    State matrix_data;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            matrix_data[i][j] = block_data[i + 4 * j];
        }
    }
    return matrix_data;
}

Block AESUtils::convertToBlock(const State& matrix_data) {
    Block block_data;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            block_data[i + 4 * j] = matrix_data[i][j];
        }
    }
    return block_data;
}