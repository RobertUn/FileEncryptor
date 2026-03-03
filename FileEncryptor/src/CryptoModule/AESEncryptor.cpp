#include "AESEncryptor.h"

AESEncryptor::~AESEncryptor() {
    cleaner.wipe(&roundKeys, sizeof(roundKeys));
}

void AESEncryptor::setKey(const Block& source_key) {
    Key key = utils.convertToMatrix(source_key);
    // 0. Расширяем ключ и очищаем исходный
    roundKeys = KeyExpansion(key);
    keysPrecomputed = true;
    cleaner.wipe(&key, sizeof(key));
}

Block AESEncryptor::encryptBlock(const Block& plaintext) {
    if (!keysPrecomputed) throw std::runtime_error("Key not set");

    // 1. Преобразуем plaintext в State
    State state = utils.convertToMatrix(plaintext);

    // 2. Начальный раунд
    state = AddRoundKey(state, utils.convertToMatrix(roundKeys[0]));

    // 3. 9 основных раундов
    for (int round = 1; round <= 9; round++) {
        state = SubBytes(state);
        state = ShiftRows(state);
        state = MixColumns(state);
        state = AddRoundKey(state, utils.convertToMatrix(roundKeys[round]));
    }

    // 4. Финальный раунд
    state = SubBytes(state);
    state = ShiftRows(state);
    state = AddRoundKey(state, utils.convertToMatrix(roundKeys[10]));

    // 5. Преобразуем State обратно в байты
    Block ciphertext = utils.convertToBlock(state);

    // 6. Выводим результат
    std::cout << "\nЗашифрованный текст: ";
    for (int i = 0; i < 16; i++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
            << static_cast<int>(ciphertext[i]) << " ";
    }
    std::cout << std::dec << std::endl;

    cleaner.wipe(&state, sizeof(state));

    return ciphertext;
}