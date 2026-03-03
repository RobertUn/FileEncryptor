#ifndef FILEPROCESS_H
#define FILEPROCESS_H

#include <string>
#include <memory>
#include <utility>
#include <fstream>
#include <stdexcept>

#include "FileIO.h"
#include "../CryptoModule/AESEncryptor.h"
#include "../CryptoModule/AESDecryptor.h"

class FileProcess {
private:
    std::string input_file_path;
    std::string output_file_path;
    std::string mode_cryption;
    Block source_key;
    FileRead fr;
    FileWrite fw;
    std::unique_ptr<AESEncryptor> encryptor;
    std::unique_ptr<AESDecryptor> decryptor;

public:
    FileProcess(const std::string& input_file_path,
        const std::string& output_file_path,
        const std::string& mode_cryption,
        const Block& source_key);

    std::pair<Chunk, size_t> removePadding(Chunk& chunk);
    void processFile();
};

#endif // FILEPROCESS_H