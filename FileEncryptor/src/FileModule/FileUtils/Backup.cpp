#include "Backup.h"
#include <chrono>
#include <ctime>
#include <random>
#include <algorithm>

Backup::Backup(const fs::path& folderPath) : targetFolder(folderPath) {
    if (!fs::exists(targetFolder)) {
        fs::create_directory(targetFolder);
    }
}

std::string Backup::generateUniqueName(const std::string& original_name) {
    auto now = std::chrono::system_clock::now();
    time_t tt = std::chrono::system_clock::to_time_t(now);

    tm local_tm;
#ifdef _WIN32
    localtime_s(&local_tm, &tt);
#else
    localtime_r(&tt, &local_tm);
#endif

    // Формат: backup_ГГГГ-ММ-ДД_ЧЧ-ММ-СС_случайное.bak
    char time_buffer[32];
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d_%H-%M-%S", &local_tm);

    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist(1000, 9999);

    return std::string("backup_") + time_buffer + "_" +
        std::to_string(dist(rng)) + ".bak";
}

void Backup::getSortedFilesByDate() {
    filesWithDates.clear();

    // Собираем все файлы
    std::vector<fs::path> all_files;
    for (const auto& entry : fs::directory_iterator(targetFolder)) {
        if (entry.is_regular_file()) {
            all_files.push_back(entry.path());
        }
    }

    // Сортируем по имени файла
    std::sort(all_files.begin(), all_files.end(),
        [](const fs::path& a, const fs::path& b) {
            return a.filename().string() < b.filename().string();
        });

    // Заполняем filesWithDates
    for (const auto& filepath : all_files) {
        filesWithDates.emplace_back(filepath, fs::last_write_time(filepath));
    }
}

void Backup::cleanupOldFiles(int maxFiles) {
    getSortedFilesByDate();

    if (filesWithDates.size() > maxFiles) {
        size_t filesToDelete = filesWithDates.size() - maxFiles;
        for (size_t i = 0; i < filesToDelete; ++i) {
            std::cout << "Удаляю старый бэкап: " << filesWithDates[i].first.filename() << std::endl;
            fs::remove(filesWithDates[i].first);
        }
    }
}

bool Backup::copyFile(const std::string& sourcePath) {
    try {
        // Получаем имя файла из пути
        std::string unique_name = generateUniqueName(sourcePath);

        // Путь для копии в текущей папке
        fs::path destination = targetFolder / unique_name;

        // Проверяем, существует ли исходный файл
        if (!fs::exists(sourcePath)) {
            std::cerr << "Исходный файл не существует: " << sourcePath << std::endl;
            return false;
        }

        // Копируем файл
        fs::copy_file(sourcePath, destination, fs::copy_options::overwrite_existing);
        std::cout << "Создан бэкап: " << unique_name << std::endl;

        // Удаляем старые
        cleanupOldFiles(3);

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка копирования: " << e.what() << std::endl;
        return false;
    }
}

std::string Backup::getLastCopiedPath() {
    getSortedFilesByDate();
    if (filesWithDates.empty()) {
        return {};
    }
    return filesWithDates.back().first.string();
}