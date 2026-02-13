#pragma once

#include <array>
#include <algorithm>

//  Константы (объявлены как extern для использования в cpp файле)
extern const unsigned char Sbox[256];
extern const unsigned char MathMatrix[4][4];
extern const unsigned char Rcon[10];
extern const unsigned char InvSbox[256];
extern const unsigned char InvMathMatrix[4][4];

static constexpr int ROW = 4;
static constexpr int COL = 4;

//  Составные типы данных для программы
using State = std::array<std::array<unsigned char, COL>, ROW>;
using Column = std::array<unsigned char, COL>;
using Key = std::array<std::array<unsigned char, COL>, ROW>;
using Word = std::array<unsigned char, ROW>;
using RoundsKeys = std::array<std::array<unsigned char, 16>, 11>;
using Block = std::array<unsigned char, 16>;

class AES_128 {
private:
    //  Вспомогательные функции
    unsigned char xtime(unsigned char x);
    Column getColumn(State state, int colNumber);
    Column MatrixMultiply(const Column& col);
    Column InvMatrixMultiply(const Column& col);
    Word g_function(Word word, int round_index);

protected:
    //  Функции шифрования
    State SubBytes(State state);
    State ShiftRows(State state);
    State MixColumns(State state);

    //  Функции дешифрования
    State InvSubBytes(State state);
    State InvShiftRows(State state);
    State InvMixColumns(State state);

    //  Общие функции для работы с ключом
    State AddRoundKey(State state, const Key& key);
    RoundsKeys KeyExpansion(Key source_key);

public:
    // Конструктор
    AES_128() = default;
};
