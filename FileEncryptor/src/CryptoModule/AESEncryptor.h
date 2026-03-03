#pragma once

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include "AES_128.h"
#include "AESUtils.h"
#include "SecureCleaner.h"

class AESEncryptor : public AES_128 {
private:
    AESUtils utils;
    SecureCleaner cleaner;
    RoundsKeys roundKeys;
    bool keysPrecomputed = false;

public:
    AESEncryptor() = default;
    ~AESEncryptor();

    void setKey(const Block& source_key);
    Block encryptBlock(const Block& plaintext);
};

