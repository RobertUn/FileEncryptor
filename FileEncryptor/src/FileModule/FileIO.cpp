#include "FileIO.h"

// ==================== Реализация FileRead ====================

FileRead::FileRead() : actualBufferSize(0) {}

Buffer& FileRead::getBuffer(const std::string& file_path) {
    if (!bufferValid) {
        std::ifstream file(file_path, std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + file_path);
        }

        // Получаем размер файла при первом открытии
        if (!fileOpened) {
            file.seekg(0, std::ios::end);          // перейти в конец
            fileTotalSize = file.tellg();     // получить размер
            file.seekg(0, std::ios::beg);          // вернуться в начало
            fileOpened = true;
            fileCursorPosition = 0;
        }

        file.seekg(fileCursorPosition, std::ios::beg);
        file.read(reinterpret_cast<char*>(cachedBuffer.data()), cachedBuffer.size());

        actualBufferSize = file.gcount();  // Сохраняем сколько реально прочитано
        fileCursorPosition += actualBufferSize;
        bufferValid = true;
        chunk_index = 0;  // Сбрасываем индекс чанков для нового буфера

        file.close();
    }

    return cachedBuffer;
}

Chunk FileRead::getChunk(const Buffer& inputBuffer) {
    Chunk result;
    size_t start_pos = chunk_index * 16;
    size_t bytes_available = actualBufferSize - start_pos;

    if (bytes_available >= 16) {
        // Полный чанк - просто копируем
        std::memcpy(result.data(), inputBuffer.data() + start_pos, 16);
    }
    else {
        // Неполный чанк - применяем PKCS#7 padding
        if (bytes_available > 0) {
            std::memcpy(result.data(), inputBuffer.data() + start_pos, bytes_available);
        }

        // Заполняем оставшееся значение = количеству добавленных байтов
        unsigned char padding_value = 16 - bytes_available;
        for (size_t i = bytes_available; i < 16; i++) {
            result[i] = padding_value;
        }
    }

    chunk_index++;
    if (chunk_index * 16 >= actualBufferSize) {
        bufferValid = false;
        chunk_index = 0;
    }

    return result;
}

size_t FileRead::getActualBufferSize() const {
    return actualBufferSize;  // Просто возвращаем сохраненное значение
}

bool FileRead::hasMoreChunksInBuffer() const {
    if (!bufferValid) return false;  // Буфер не валиден - нет чанков

    // Чанки есть, если текущий индекс * 16 < реального размера буфера
    return (chunk_index * 16) < actualBufferSize;
}

bool FileRead::isEOF() const {
    // Конец файла если:
    // 1. Позиция чтения >= размера файла
    // 2. И текущий буфер полностью обработан (или невалиден)
    bool atEndOfFile = (fileCursorPosition >= fileTotalSize);
    bool bufferProcessed = (!bufferValid || (chunk_index * 16 >= actualBufferSize));

    return atEndOfFile && bufferProcessed;
}

bool FileRead::isBufferValid() {
    return bufferValid;
}

void FileRead::resetCursor() {
    fileCursorPosition = 0;
    chunk_index = 0;
    bufferValid = false;
}

std::streampos FileRead::getCursorPosition() const {
    return fileCursorPosition;
}

// ==================== Реализация FileWrite ====================

void FileWrite::postBuffer(const std::string& file_path) {
    std::ofstream file(file_path, std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + file_path);
    }

    file.seekp(fileCursorPosition, std::ios::beg);
    file.write(reinterpret_cast<char*>(cachedBuffer.data()), cachedBuffer.size());

    fileCursorPosition = file.tellp();
    file.close();
}

void FileWrite::postChunk(const Chunk& inputChunk) {
    std::copy(inputChunk.begin(), inputChunk.end(), cachedBuffer.begin() + chunk_index * 16);
    chunk_index++;
}

void FileWrite::resetCursor() {
    fileCursorPosition = 0;
    chunk_index = 0;
}

Buffer FileWrite::getBufferForWrite() {
    return cachedBuffer;
}

bool FileWrite::isBufferFull() const {
    return chunk_index == 256;
}

bool FileWrite::isBufferEmpty() const {
    return chunk_index == 0;
}

size_t FileWrite::getBufferSize() const {
    return chunk_index * 16;
}

void FileWrite::resetWriteBuffer() {
    chunk_index = 0;
}

std::streampos FileWrite::getCursorPosition() const {
    return fileCursorPosition;
}