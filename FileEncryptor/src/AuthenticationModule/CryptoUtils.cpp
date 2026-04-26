#include "CryptoUtils.h"
#include <iostream>
#include <iomanip>
#include <vector>

using Block = std::array<unsigned char, 16>;

unsigned char CryptoUtils::rotate_left(unsigned char x, int bits) {
    return (x << bits) | (x >> (8 - bits));
}

Block CryptoUtils::getHash(std::string input) {
    // 1. Подготовка
    std::vector<unsigned char> data(input.begin(), input.end());
    for (auto& c : data) c = Sbox[c];

    // 2. Расширение с зависимостью от позиции
    std::vector<unsigned char> expanded;
    for (size_t i = 0; i < data.size(); i++) {
        for (size_t j = 0; j < 4; j++) {
            unsigned char next = (data[i] * cubic_polynomial[j] +
                data[(i + 1) % data.size()]) ^ (i * j * 0x9e37);
            expanded.push_back(Sbox[next]);
        }
    }

    // 3. Инициализация состояния
    Block state = { 0 };
    for (size_t i = 0; i < sizeof(size_t); i++) {
        state[i] = (data.size() >> (i * 8)) & 0xFF;
    }

    // 4. Компрессия
    for (int i = 0; i < expanded.size(); i++) {
        int p = i % 16;
        state[p] ^= expanded[i];
        state[(p + 3) % 16] += expanded[i];
        state[(p + 7) % 16] ^= rotate_left(state[p], 2);
    }

    // 5. Финальное перемешивание (4 раунда)
    for (int r = 0; r < 4; r++) {
        for (int i = 0; i < 16; i++) {
            state[i] = Sbox[state[i] ^ (r * 0x55)];
            state[(i + 1) % 16] ^= state[i];
            state[i] = rotate_left(state[i], (r + 1) % 7 + 1);
        }
        // Перекрестное перемешивание
        for (int i = 0; i < 8; i++) {
            std::swap(state[i], state[15 - i]);
            state[i] ^= state[15 - i];
        }
    }
    return state;
}

Block CryptoUtils::getMixing(Block& hardwarePrint, Block inputHash) {
    // Начальное смешивание
    Block state = inputHash;
    for (size_t i = 0; i < 16; i++) {
        state[i] ^= hardwarePrint[i];
        state[i] = Sbox[state[i]];
    }

    // Раунды
    for (int round = 0; round < 8; round++) {
        // 1. Перемешивание с hardwarePrint (асимметрия)
        for (size_t i = 0; i < 16; i++) {
            state[i] ^= hardwarePrint[(i + round) % 16];
            state[i] += round_const[round];
        }
        // 2. S-box слой
        for (size_t i = 0; i < 16; i++) {
            state[i] = Sbox[state[i]];
        }
        // 3. Диффузионный слой (перестановка + линейное смешивание)
        Block temp = state;
        for (size_t i = 0; i < 16; i++) {
            state[perm[i]] = temp[i];
        }
        for (size_t i = 0; i < 16; i++) {
            state[i] ^= state[(i + 1) % 16];
            state[i] ^= rotate_left(state[(i + 5) % 16], round % 7 + 1);
        }
        // 4. Обратная связь от hash
        if (round % 2 == 0) {
            for (size_t i = 0; i < 16; i++) {
                state[i] ^= state[(i + round) % 16];
                state[i] = rotate_left(state[i], round % 5 + 1);
            }
        }
        else {
            for (size_t i = 0; i < 16; i++) {
                state[i] += state[(i * round) % 16];
            }
        }
    }

    // Финальная раундовая зависимость
    for (int round = 0; round < 3; round++) {
        for (size_t i = 0; i < 16; i++) {
            state[i] = Sbox[state[i] ^ round_const[round + 8]];
            state[(i + 3) % 16] ^= state[i];
            state[i] = rotate_left(state[i], round + 1);
        }
    }
    return state;
}