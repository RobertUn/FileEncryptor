#ifndef BACKUP_H
#define BACKUP_H

#include <string>
#include <vector>
#include <utility>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

class Backup {
private:
    fs::path targetFolder;
    std::vector<std::pair<fs::path, fs::file_time_type>> filesWithDates;

    std::string generateUniqueName(const std::string& original_name);
    void getSortedFilesByDate();
    void cleanupOldFiles(int maxFiles);

public:
    explicit Backup(const fs::path& folderPath);

    bool copyFile(const std::string& sourcePath);
    std::string getLastCopiedPath();
};

#endif // BACKUP_H