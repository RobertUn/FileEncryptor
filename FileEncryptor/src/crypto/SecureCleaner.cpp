#include "SecureCleaner.h"

void SecureCleaner::wipe(void* ptr, size_t size) {
    volatile unsigned char* p = static_cast<volatile unsigned char*>(ptr);
    for (size_t i = 0; i < size; i++) {
        p[i] = 0;
    }
}