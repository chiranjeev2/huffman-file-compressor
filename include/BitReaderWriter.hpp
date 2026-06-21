#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>

class BitWriter {
private:
    std::ofstream out;
    uint8_t buffer;
    int bitCount;

public:
    explicit BitWriter(const std::string& path);
    ~BitWriter();
    void writeBit(bool bit);
    void writeBits(const std::string& bitString);
    void flush();
    std::ofstream& getStream() { return out; }
};

class BitReader {
private:
    std::ifstream in;
    uint8_t buffer;
    int bitCount;

public:
    explicit BitReader(const std::string& path);
    bool readBit(bool& bit);
    std::ifstream& getStream() { return in; }
};