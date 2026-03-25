#include <iostream>
#include <string>
#include <windows.h>
#include "CryptoModule/AESEncryptor.h"
#include "CryptoModule/AESDecryptor.h"
#include "cli/interface.cpp"

using namespace std;

bool copyToClipboard(std::string_view text) {
    if (!OpenClipboard(nullptr)) return false;

    struct ClipboardGuard {
        ~ClipboardGuard() { CloseClipboard(); }
    } guard;

    EmptyClipboard();

    // Правильное преобразование из CP1251 в UTF-16
    int wchars_needed = MultiByteToWideChar(
        1251,                    // CP1251 code page
        0,                       // flags
        text.data(),             // source string
        static_cast<int>(text.size()), // source length
        nullptr,                 // no output yet
        0                        // get required size
    );

    if (wchars_needed == 0) return false;

    std::vector<wchar_t> wtext(wchars_needed + 1); // +1 для null-terminator

    MultiByteToWideChar(
        1251,
        0,
        text.data(),
        static_cast<int>(text.size()),
        wtext.data(),
        wchars_needed
    );
    wtext[wchars_needed] = L'\0'; // гарантируем null-termination

    // Выделяем память для буфера
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, (wchars_needed + 1) * sizeof(wchar_t));
    if (!hMem) return false;

    wchar_t* pMem = static_cast<wchar_t*>(GlobalLock(hMem));
    if (!pMem) {
        GlobalFree(hMem);
        return false;
    }

    // Копируем строку
    wcscpy_s(pMem, wchars_needed + 1, wtext.data());
    GlobalUnlock(hMem);

    // Передаем буфер в буфер обмена
    if (!SetClipboardData(CF_UNICODETEXT, hMem)) {
        GlobalFree(hMem);
        return false;
    }

    return true;
}

int main() {
	// Настройка кодировки
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);
	setlocale(LC_ALL, "Russian");

	AESEncryptor aes128encrypt;
	Block key = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
	Block plaintext = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
	aes128encrypt.setKey(key);
	aes128encrypt.encryptBlock(plaintext);

	AESDecryptor aes128decrypt;
	Block ciphertext = { 0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30, 0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a };
	aes128decrypt.setKey(key);
	aes128decrypt.decryptBlock(ciphertext);

    // Установка размеров консоли (ширина: 170, высота: 50)
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole != INVALID_HANDLE_VALUE) {
        COORD bufferSize = { 170, 500 };
        SetConsoleScreenBufferSize(hConsole, bufferSize);

        SMALL_RECT windowSize = { 0, 0, 169, 49 };
        SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
    }

    ScreenManager manager;
    manager.run();
    return 0;
}
