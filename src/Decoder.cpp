#include "Decoder.hpp"
#include "BitReaderWriter.hpp"
#include <queue>
#include <vector>
#include <string>   // Added explicitly
#include <fstream>  // Added explicitly for std::ofstream

Decoder::~Decoder() { delete root; }

bool Decoder::decompress(const std::string& inputPath, const std::string& outputPath) {
    BitReader reader(inputPath);
    auto& inStream = reader.getStream();
    if (!inStream.is_open()) return false;

    uint32_t magic;
    uint64_t totalBytes;
    uint16_t tableSize;

    inStream.read(reinterpret_cast<char*>(&magic), 4);
    if (magic != HUFF_MAGIC) return false;

    inStream.read(reinterpret_cast<char*>(&totalBytes), 8);
    inStream.read(reinterpret_cast<char*>(&tableSize), 2);

    std::ofstream outFile(outputPath, std::ios::binary);
    if (totalBytes == 0) return true; // Handled empty file

    freqTable.clear();
    for (int i = 0; i < tableSize; ++i) {
        uint8_t b;
        uint64_t f;
        inStream.read(reinterpret_cast<char*>(&b), 1);
        inStream.read(reinterpret_cast<char*>(&f), 8);
        freqTable[b] = f;
    }

    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, NodeComparator> minHeap;
    for (auto& pair : freqTable) {
        minHeap.push(new HuffmanNode(pair.first, pair.second));
    }

    while (minHeap.size() > 1) {
        auto left = minHeap.top(); minHeap.pop();
        auto right = minHeap.top(); minHeap.pop();
        auto parent = new HuffmanNode(left->frequency + right->frequency, left, right);
        minHeap.push(parent);
    }
    root = minHeap.top();

    HuffmanNode* current = root;
    uint64_t bytesDecoded = 0;
    bool bit;

    while (bytesDecoded < totalBytes && reader.readBit(bit)) {
        if (current->isLeaf()) { // Single unique element optimization fallback
            outFile.put(static_cast<char>(current->data));
            bytesDecoded++;
            current = root;
        }
        current = bit ? current->right : current->left;
        if (current->isLeaf()) {
            outFile.put(static_cast<char>(current->data));
            bytesDecoded++;
            current = root;
        }
    }
    return true;
}