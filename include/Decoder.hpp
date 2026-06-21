#pragma once
#include <string>
#include <unordered_map>
#include <cstdint> // Added for safety
#include "HuffmanCommon.hpp"
class Decoder {
private:
    std::unordered_map<uint8_t, uint64_t> freqTable;
    HuffmanNode* root = nullptr;

public:
    ~Decoder();
    bool decompress(const std::string& inputPath, const std::string& outputPath);
};