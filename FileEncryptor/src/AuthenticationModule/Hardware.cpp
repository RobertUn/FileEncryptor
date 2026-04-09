#include "Hardware.h"
#include <iostream>
#include <vector>
#include <cstring>
#include <windows.h>

std::array<unsigned char, 16> Hardware::getHardwarePrint() {
    // 1. Получить размер
    DWORD bufferSize = GetSystemFirmwareTable(RSMB, 0, NULL, 0);
    if (bufferSize == 0) {
        std::cerr << "Error getting SMBIOS table size" << std::endl;
        return {};
    }

    // 2. Выделить буфер
    std::vector<uint8_t> buffer(bufferSize);

    // 3. Получить данные
    DWORD bytesWritten = GetSystemFirmwareTable(RSMB, 0, buffer.data(), bufferSize);
    if (bytesWritten != bufferSize) {
        std::cerr << "Error reading SMBIOS table" << std::endl;
        return {};
    }

    // 4. Интерпретировать как RawSMBIOSData
    RawSMBIOSData* smbios = reinterpret_cast<RawSMBIOSData*>(buffer.data());

    // 5. Парсинг 
    uint8_t* currentPos = smbios->SMBIOSTableData;
    uint8_t* endPos = currentPos + smbios->Length;

    std::array<unsigned char, 16> hardwarePrint{};
    bool found = false;

    while (currentPos < endPos) {
        uint8_t type = currentPos[0];
        uint8_t length = currentPos[1];

        if (type == 1) {
            uint8_t* uuidStart = currentPos + 8;
            std::memcpy(hardwarePrint.data(), uuidStart, 16);
            found = true;
            break;
        }

        currentPos += length;

        // Пропустить строки
        while (currentPos < endPos && (*currentPos != 0 || *(currentPos + 1) != 0)) {
            currentPos++;
        }
        currentPos += 2;
    }

    if (!found) {
        std::cerr << "System Information structure not found" << std::endl;
        return {};
    }

    return hardwarePrint;
}

std::string Hardware::getHardwarePrintAsString() {
    auto print = getHardwarePrint();
    if (print.empty()) return "";

    return std::string(reinterpret_cast<char*>(print.data()), print.size());
}

bool Hardware::isHardwareValid() {
    auto print = getHardwarePrint();
    if (print.empty()) return false;

    // Проверка, что не все байты нулевые
    for (unsigned char byte : print) {
        if (byte != 0) return true;
    }
    return false;
}