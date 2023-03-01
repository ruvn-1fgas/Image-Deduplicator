#pragma once
#include <vector>
#include <string>

class ImageHash
{
private:
    std::vector<bool> _hash;

public:
    ImageHash(std::vector<bool> &binaryArray);
    operator std::string();
    operator std::vector<bool>();
    size_t operator-(const std::vector<bool> &other);
    bool operator==(const std::vector<bool> &other);
    bool operator!=(const std::vector<bool> &other);
    unsigned hash();
};