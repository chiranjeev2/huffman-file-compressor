#include "BitReaderWriter.hpp"

BitWriter::BitWriter(const std::string& path) : out(path, std::ios::binary), buffer(0), bitCount(0) {}

BitWriter::~BitWriter() { flush(); }

void BitWriter::writeBit(bool bit) {
    buffer <<= 1;
    if (bit) buffer |= 1;
    bitCount++;
    if (bitCount == 8) {
        out.put(static_cast<char>(buffer));
        buffer = 0;
        bitCount = 0;
    }
}

void BitWriter::writeBits(const std::string& bitString) {
    for (char c : bitString) {
        writeBit(c == '1');
    }
}

void BitWriter::flush() {
    if (bitCount > 0) {
        buffer <<= (8 - bitCount);
        out.put(static_cast<char>(buffer));
        buffer = 0;
        bitCount = 0;
    }
}

BitReader::BitReader(const std::string& path) : in(path, std::ios::binary), buffer(0), bitCount(0) {}

bool BitReader::readBit(bool& bit) {
    if (bitCount == 0) {
        char ch;
        if (!in.get(ch)) return false;
        buffer = static_cast<uint8_t>(ch);
        bitCount = 8;
    }
    bit = (buffer & (1 << (bitCount - 1))) != 0;
    bitCount--;
    return true;
}