#include "Decoder.hpp"
#include "BitReaderWriter.hpp"
#include <queue>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm> // Required for deterministic sorting

Decoder::~Decoder() { delete root; }

bool Decoder::decompress(const std::string& inputPath, const std::string& outputPath) {
    std::cout << "[Decoder] Initializing extraction..." << std::endl;
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
    if (totalBytes == 0) return true;

    freqTable.clear();
    for (int i = 0; i < tableSize; ++i) {
        uint8_t b;
        uint64_t f;
        inStream.read(reinterpret_cast<char*>(&b), 1);
        inStream.read(reinterpret_cast<char*>(&f), 8);
        freqTable[b] = f;
    }

    // FIX 1: Mirror the exact deterministic sort used in the Encoder
    std::vector<std::pair<uint8_t, uint64_t>> sortedFreq;
    for (auto& pair : freqTable) {
        sortedFreq.push_back({pair.first, pair.second});
    }
    std::sort(sortedFreq.begin(), sortedFreq.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });

    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, NodeComparator> minHeap;
    for (auto& pair : sortedFreq) {
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

    while (bytesDecoded < totalBytes) {
        if (root->isLeaf()) { 
            outFile.put(static_cast<char>(root->data));
            bytesDecoded++;
            continue;
        }
        
        if (reader.readBit(bit)) {
            current = bit ? current->right : current->left;
            if (current->isLeaf()) {
                outFile.put(static_cast<char>(current->data));
                bytesDecoded++;
                current = root;
            }
        } else {
            std::cout << "[Decoder Fatal] Reached EOF. Stream corrupted." << std::endl;
            return false;
        }
    }

    // Explicitly close the file to guarantee Emscripten MEMFS writes it before JS reads it
    outFile.close(); 
    std::cout << "[Decoder] Success! " << bytesDecoded << " bytes perfectly restored." << std::endl;
    return true;
}