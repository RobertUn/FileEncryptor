#include "CryptingManager.h"
#include "../FileModule/FileUtils/FileName.h"
#include "../FileModule/FileProcess.h"
#include "../FileModule/FileUtils/Backup.h"
#include <string>
#include <filesystem>
#include <iostream>

static inline const std::string TRUE_KEY = "";

class CryptingManager {
private:
	FileName fn;
	std::string input_file_path;
	std::string output_file_path;
	std::string input_key;
	std::string mode_cryption;

	Backup bc{ "data/backup" };

	void computePath() {
		std::filesystem::path fullPath = std::filesystem::current_path() / "data.json";
		if (std::filesystem::exists(fullPath) && std::filesystem::is_regular_file(fullPath)) {
			input_file_path = "data.json";
			output_file_path = fn.addSuffix(input_file_path);
			mode_cryption = "Encryption";
			bc.copyFile(input_file_path);
		}
		else {
			std::filesystem::path suffixedPath = std::filesystem::current_path() / fn.addSuffix("data.json");
			if (!std::filesystem::exists(suffixedPath) && std::filesystem::is_regular_file(suffixedPath)) {
				throw std::runtime_error("No valid data file found");
			}
			output_file_path = "data.json";
			input_file_path = fn.addSuffix(output_file_path);
			mode_cryption = "Decryption";
		}
	}

public:
	CryptingManager(const std::string& input_key) : input_key(input_key) {}

	//bool processCrypting() {}	
};
