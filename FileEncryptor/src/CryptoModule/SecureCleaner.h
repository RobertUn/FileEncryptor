#pragma once

#include <cstddef>

class SecureCleaner {
public:
    static void wipe(void* ptr, size_t size);

    // Шаблонный метод для удобства
    template<typename T>
    static void wipe(T& obj) {
        wipe(&obj, sizeof(T));
    }
};

