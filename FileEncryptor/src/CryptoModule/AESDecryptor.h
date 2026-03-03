#pragma once

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include "AES_128.h"
#include "AESUtils.h"
#include "SecureCleaner.h"

class AESDecryptor : public AES_128 {
private:
    AESUtils utils;
    SecureCleaner cleaner;
    RoundsKeys roundKeys;
    bool keysPrecomputed = false;

public:
    AESDecryptor() = default;
    ~AESDecryptor();

    void setKey(const Block& source_key);
    Block decryptBlock(const Block& ciphertext);
};

