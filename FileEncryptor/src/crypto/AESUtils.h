#pragma once

#include <array>
#include "AES_128.h"

class AESUtils {
public:
    // Конвертация данных
    static State convertToMatrix(const Block& block_data);
    static Block convertToBlock(const State& matrix_data);
};

