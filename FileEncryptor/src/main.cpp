#include <iostream>
#include <string>
#include <windows.h>
#include "crypto/AESEncryptor.h"
#include "crypto/AESDecryptor.h"
#include "data/data.cpp"

using namespace std;

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

	DataManager dm("data.json");
	dm.load();

    // Добавляем новый элемент
	Item new_item;
	
	new_item.id = -1;
	new_item.name = "Проект Альфа";
	new_item.description = "Исследовательский проект";
	new_item.url = "https://example.com";
	//dm.addItem(new_item);
	dm.sortItemsById();
	dm.addItem("a", "hui", "porn.ru");
    // Сохраняем
	dm.save();
}