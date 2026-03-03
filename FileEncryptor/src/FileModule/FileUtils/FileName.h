#ifndef FILENAME_H
#define FILENAME_H

#include <string>
#include <stdexcept>

class FileName {
private:
    static inline const std::string SUFFIX = ".UnRob4ik";

    static bool hasSuffix(const std::string& filename);

public:
    static std::string addSuffix(const std::string& original);
    static std::string removeSuffix(const std::string& masked);
};

#endif // FILENAME_H