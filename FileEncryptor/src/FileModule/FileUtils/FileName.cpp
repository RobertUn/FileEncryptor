#include "FileName.h"

bool FileName::hasSuffix(const std::string& filename) {
    return filename.size() > SUFFIX.size() &&
        filename.substr(filename.size() - SUFFIX.size()) == SUFFIX;
}

std::string FileName::addSuffix(const std::string& original) {
    return original + SUFFIX;
}

std::string FileName::removeSuffix(const std::string& masked) {
    if (!hasSuffix(masked)) {
        throw std::runtime_error("Not a masked file");
    }
    return masked.substr(0, masked.length() - SUFFIX.length());
}