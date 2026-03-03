#ifndef FILEIO_H
#define FILEIO_H

#include <ostream>
#include <fstream>
#include <array>
#include <string>
#include <filesystem>
#include <memory>
#include <cstring>  // для memcpy

// Типы данных должны быть объявлены ДО использования в классах
using Buffer = std::array<unsigned char, 4096>;
using Chunk = std::array<unsigned char, 16>;

class FileRead {
private:
    std::streampos fileCursorPosition = 0;
    std::streampos fileTotalSize = 0;
    size_t chunk_index = 0;
    Buffer cachedBuffer;
    bool bufferValid = false;
    bool fileOpened = false;
    size_t actualBufferSize = 0;

public:
    FileRead();

    Buffer& getBuffer(const std::string& file_path);
    Chunk getChunk(const Buffer& inputBuffer);
    size_t getActualBufferSize() const;
    bool hasMoreChunksInBuffer() const;
    bool isEOF() const;
    bool isBufferValid();
    void resetCursor();
    std::streampos getCursorPosition() const;
};

class FileWrite {
private:
    std::streampos fileCursorPosition = 0;
    size_t chunk_index = 0;
    Buffer cachedBuffer;

public:
    void postBuffer(const std::string& file_path);
    void postChunk(const Chunk& inputChunk);
    void resetCursor();
    Buffer getBufferForWrite();
    bool isBufferFull() const;
    bool isBufferEmpty() const;
    size_t getBufferSize() const;
    void resetWriteBuffer();
    std::streampos getCursorPosition() const;
};

#endif // FILEIO_H