#pragma once
#include <string>
#include <unordered_map>
#include "HuffmanCommon.hpp"
#include <cstdint>
#include "HuffmanCommon.hpp"

class Encoder {
private:
    std::unordered_map<uint8_t, uint64_t> freqTable;
    std::unordered_map<uint8_t, std::string> huffmanCodes;
    HuffmanNode* root = nullptr;

    void generateCodes(HuffmanNode* node, const std::string& str);

public:
    ~Encoder();
    bool compress(const std::string& inputPath, const std::string& outputPath);
};