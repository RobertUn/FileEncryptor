#include "FileProcess.h"

FileProcess::FileProcess(const std::string& input_file_path,
    const std::string& output_file_path,
    const std::string& mode_cryption,
    const Block& source_key)
    : input_file_path(input_file_path)
    , output_file_path(output_file_path)
    , mode_cryption(mode_cryption)
    , source_key(source_key) {
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

void FileProcess::processFile() {
    // Проверяем режим и создаем нужный объект ОДИН РАЗ
    if (mode_cryption == "Encryption") {
        encryptor = std::make_unique<AESEncryptor>();
        encryptor->setKey(source_key);
    }
    else if (mode_cryption == "Decryption") {
        decryptor = std::make_unique<AESDecryptor>();
        decryptor->setKey(source_key);
    }

    // Открываем выходной файл ОДИН РАЗ
    std::ofstream outFile(output_file_path, std::ios::binary);
    if (!outFile) throw std::runtime_error("Cannot open output file");

    while (true) {
        // 1. Получить буфер (прочитает файл если нужно)
        Buffer& inputBuffer = fr.getBuffer(input_file_path);

        // 2. Обработать все чанки из ЭТОГО буфера
        while (fr.hasMoreChunksInBuffer()) {
            Chunk plainChunk = fr.getChunk(inputBuffer);
            Chunk processedChunk;

            if (mode_cryption == "Encryption") {
                processedChunk = encryptor->encryptBlock(plainChunk);
            }
            else if (mode_cryption == "Decryption") {
                processedChunk = decryptor->decryptBlock(plainChunk);

                // ТОЛЬКО ДЛЯ ДЕШИФРОВАНИЯ: проверяем последний чанк
                if (fr.isEOF() && !fr.hasMoreChunksInBuffer()) {
                    std::pair<Chunk, size_t> processedPair = removePadding(processedChunk);
                    outFile.write(reinterpret_cast<char*>(processedPair.first.data()),
                        processedPair.second);
                    break;
                }
            }

            fw.postChunk(processedChunk);

            // 3. Если буфер записи заполнился - сбросить в файл
            if (fw.isBufferFull()) {
                outFile.write(reinterpret_cast<char*>(fw.getBufferForWrite().data()),
                    fw.getBufferSize());
                fw.resetWriteBuffer();
            }
        }

        // 4. Проверка завершения
        if (fr.isEOF()) {
            // Записать остатки
            if (!fw.isBufferEmpty()) {
                outFile.write(reinterpret_cast<char*>(fw.getBufferForWrite().data()),
                    fw.getBufferSize());
            }
            break;
        }
    }
}