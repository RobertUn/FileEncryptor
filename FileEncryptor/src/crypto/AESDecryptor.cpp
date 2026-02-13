#include "AESDecryptor.h"

AESDecryptor::~AESDecryptor() {
    cleaner.wipe(&roundKeys, sizeof(roundKeys));
}

void AESDecryptor::setKey(const Block& source_key) {
    Key key = utils.convertToMatrix(source_key);
    // 0. Расширяем ключ и очищаем исходный
    roundKeys = KeyExpansion(key);
    keysPrecomputed = true;
    cleaner.wipe(&key, sizeof(key));
}

Block AESDecryptor::decryptBlock(const Block& ciphertext) {
    if (!keysPrecomputed) throw std::runtime_error("Key not set");

    // 1. Преобразуем ciphertext в State
    State state = utils.convertToMatrix(ciphertext);

    // 2. Начальный раунд
    state = AddRoundKey(state, utils.convertToMatrix(roundKeys[10]));
    state = InvShiftRows(state);
    state = InvSubBytes(state);

    // 3. 9 основных раундов
    for (int round = 9; round >= 1; round--) {
        state = AddRoundKey(state, utils.convertToMatrix(roundKeys[round]));
        state = InvMixColumns(state);
        state = InvShiftRows(state);
        state = InvSubBytes(state);
    }

    // 4. Финальный раунд
    state = AddRoundKey(state, utils.convertToMatrix(roundKeys[0]));

    // 5. Преобразуем State обратно в байты
    Block plaintext = utils.convertToBlock(state);

    // 6. Выводим результат
    std::cout << "\nРасшифрованный текст: ";
    for (int i = 0; i < 16; i++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
            << static_cast<int>(plaintext[i]) << " ";
    }
    std::cout << std::dec << std::endl;

    cleaner.wipe(&state, sizeof(state));

    return plaintext;
}