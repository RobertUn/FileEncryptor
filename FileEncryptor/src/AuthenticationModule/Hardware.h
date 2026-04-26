#ifndef HARDWARE_H
#define HARDWARE_H

#include <array>
#include <cstdint>
#include <string>

class Hardware {
private:
    static constexpr uint32_t RSMB = 0x52534D42;  // 'RSMB' в little-endian

    struct RawSMBIOSData {
        uint8_t     Used20CallingMethod;
        uint8_t     SMBIOSMajorVersion;
        uint8_t     SMBIOSMinorVersion;
        uint8_t     DmiRevision;
        uint32_t    Length;
        uint8_t     SMBIOSTableData[];  // здесь начинаются сами структуры
    };

public:
    std::array<unsigned char, 16> getHardwarePrint();  // данные в виде блока 
    std::string getHardwarePrintAsString();  // данные в виде строки
    bool isHardwareValid();  // проверка на валидность
};

#endif // HARDWARE_H