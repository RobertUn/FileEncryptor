#include "FileProcess.h"

FileProcess::FileProcess(const std::string& input_file_path,
    const std::string& output_file_path,
    const Block& source_key) : 
    input_file_path(input_file_path), 
    output_file_path(output_file_path), 
    source_key(source_key) {
}

std::pair<Chunk, size_t> FileProcess::removePadding(Chunk& chunk) {
    unsigned char paddingValue = chunk[15];

    // Проверяем paddingValue
    if (paddingValue == 0 || paddingValue > 16) {
        throw std::runtime_error("Invalid padding value");
    }

    size_t dataSize = 16 - paddingValue;

    // Проверяем структуру паддинга
    for (size_t i = dataSize; i < 16; i++) {
        if (chunk[i] != paddingValue) {
            throw std::runtime_error("Invalid padding structure");
        }
    }

    return { chunk, dataSize };  // Возвращаем чанк И реальный размер
}

void FileProcess::processEncryption() {
    encryptor = std::make_unique<AESEncryptor>();
    encryptor->setKey(source_key);

    std::ofstream outFile(output_file_path, std::ios::binary);
    if (!outFile) throw std::runtime_error("Cannot open output file");

    std::vector<Chunk> plainChunks;

    // Читаем все данные
    while (true) {
        Buffer& inputBuffer = fr.getBuffer(input_file_path);
        while (fr.hasMoreChunksInBuffer()) {
            plainChunks.push_back(fr.getChunk(inputBuffer));
        }
        if (fr.isEOF()) break;
    }

    // Добавляем паддинг к последнему чанку
    if (!plainChunks.empty()) {
        Chunk& lastChunk = plainChunks.back();
        size_t lastChunkDataSize = lastChunk.size();

        // Для AES блок 16 байт, паддинг по PKCS#7
        unsigned char paddingNeeded = 16 - (lastChunkDataSize % 16);
        if (paddingNeeded == 0) paddingNeeded = 16; // Полный блок паддинга

        // Расширяем последний чанк до 16 байт с паддингом
        for (size_t i = lastChunkDataSize; i < 16; i++) {
            lastChunk[i] = paddingNeeded;
        }
    }

    // Шифруем и записываем
    for (const auto& plainChunk : plainChunks) {
        Chunk encryptedChunk = encryptor->encryptBlock(plainChunk);
        outFile.write(reinterpret_cast<const char*>(encryptedChunk.data()), 16);
    }
}

void FileProcess::processDecryption() {
    decryptor = std::make_unique<AESDecryptor>();
    decryptor->setKey(source_key);

    std::ofstream outFile(output_file_path, std::ios::binary);
    if (!outFile) throw std::runtime_error("Cannot open output file");

    std::vector<Chunk> allDecryptedChunks;

    // Сначала дешифруем все блоки
    while (true) {
        Buffer& inputBuffer = fr.getBuffer(input_file_path);

        while (fr.hasMoreChunksInBuffer()) {
            Chunk encryptedChunk = fr.getChunk(inputBuffer);
            Chunk decryptedChunk = decryptor->decryptBlock(encryptedChunk);
            allDecryptedChunks.push_back(decryptedChunk);
        }

        if (fr.isEOF()) break;
    }

    // Обрабатываем последний блок с паддингом
    if (!allDecryptedChunks.empty()) {
        // Последний блок
        auto [lastChunk, dataSize] = removePadding(allDecryptedChunks.back());

        // Записываем все блоки, кроме последнего, целиком
        for (size_t i = 0; i < allDecryptedChunks.size() - 1; i++) {
            outFile.write(reinterpret_cast<char*>(allDecryptedChunks[i].data()),
                16);
        }

        // Записываем последний блок без паддинга
        outFile.write(reinterpret_cast<char*>(lastChunk.data()), dataSize);
    }
}