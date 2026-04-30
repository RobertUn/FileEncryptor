#include "CryptoManager.h"

void CryptoManager::preCompute() {
    std::filesystem::path fullPath = std::filesystem::current_path() / "data/data.json";
    if (std::filesystem::exists(fullPath) && std::filesystem::is_regular_file(fullPath)) {
        input_file_path = "data/data.json";
        output_file_path = fn.addSuffix(input_file_path);
        mode_cryption = "Encryption";
    }
    else {
        std::filesystem::path suffixedPath = std::filesystem::current_path() / fn.addSuffix("data/data.json");
        if (!std::filesystem::exists(suffixedPath) && std::filesystem::is_regular_file(suffixedPath)) {
            throw std::runtime_error("No valid data file found");
        }
        output_file_path = "data/data.json";
        input_file_path = fn.addSuffix(output_file_path);
        mode_cryption = "Decryption";
    }
}

CryptoManager::CryptoManager(const std::string& input_key) : input_key(input_key), bc("data/backup") {
    preCompute();
}

void CryptoManager::processCrypto() {
    Block harware_print = hp.getHardwarePrint();
    Block hash_key = cu.getHash(input_key);
    Block mixing_key = cu.getMixing(harware_print, hash_key);
    FileProcess fp(input_file_path, output_file_path, mixing_key);

    bc.copyFile(input_file_path);
    if (mode_cryption == "Encryption") {
        fp.processEncryption();
    }
    else {
        fp.processDecryption();
    }

    try {
        std::filesystem::remove(input_file_path);
    }
    catch (const std::filesystem::filesystem_error& e) {
        throw std::runtime_error("Failed to delete input_file_path");
    }
}

bool CryptoManager::restoreLastFile() {
    std::string restore_file_path = bc.getLastCopiedPath();
    try {
        if (std::filesystem::exists(input_file_path)) {
            std::filesystem::remove(input_file_path);
        }
        // копируем + меняем имя на input_file_path
        std::filesystem::copy_file(restore_file_path, input_file_path);

        try {
            std::filesystem::remove(output_file_path);
        }
        catch (const std::filesystem::filesystem_error& e) {
            throw std::runtime_error("Failed to delete output_file_path");
        }

        return true;
    }
    catch (std::filesystem::filesystem_error& e) {
        throw std::runtime_error("Filesystem error");
        return false;
    }
}

bool CryptoManager::isFileEncrypted() {
    return mode_cryption == "Encryption";
}