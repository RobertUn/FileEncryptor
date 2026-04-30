#pragma once

#include "CryptoManager.h"
#include "../FileModule/FileUtils/FileName.h"
#include "../FileModule/FileProcess.h"
#include "../FileModule/FileUtils/Backup.h"
#include "CryptoUtils.h"
#include "Hardware.h"
#include <string>
#include <filesystem>
#include <iostream>

class CryptoManager {
private:
    FileName fn;
    Hardware hp;
    CryptoUtils cu;
    std::string input_file_path;
    std::string output_file_path;
    std::string input_key;
    std::string mode_cryption;

    Backup bc;

    void preCompute();

public:
    CryptoManager(const std::string& input_key);

    void processCrypto();

    bool restoreLastFile();

    bool isFileEncrypted();
};