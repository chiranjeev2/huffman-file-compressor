#include "Decoder.hpp"
#include "BitReaderWriter.hpp"
#include <queue>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

Decoder::~Decoder() { delete root; }

bool Decoder::decompress(const std::string& inputPath, const std::string& outputPath) {
    std::cout << "[Decoder] Initializing bit reader on: " << inputPath << std::endl;
    BitReader reader(inputPath);
    auto& inStream = reader.getStream();
    
    if (!inStream.is_open()) {
        std::cout << "[Decoder Fatal] Underlying std::ifstream failed to open." << std::endl;
        return false;
    }

    uint32_t magic;
    uint64_t totalBytes;
    uint16_t tableSize;

    inStream.read(reinterpret_cast<char*>(&magic), 4);
    if (magic != HUFF_MAGIC) {
        std::cout << "[Decoder Fatal] Magic number mismatch! Expected " << HUFF_MAGIC 
                  << " but extracted " << magic << std::endl;
        std::cout << "          (This usually means BitReader prefetched a byte and corrupted the header alignment)." << std::endl;
        return false;
    }

    inStream.read(reinterpret_cast<char*>(&totalBytes), 8);
    inStream.read(reinterpret_cast<char*>(&tableSize), 2);
    
    std::cout << "[Decoder] Header Verified: " << totalBytes << " bytes expected, " 
              << tableSize << " dictionary entries." << std::endl;

    std::ofstream outFile(outputPath, std::ios::binary);
    if (totalBytes == 0) {
        std::cout << "[Decoder] Empty file detected. Decompression complete." << std::endl;
        return true;
    }

    freqTable.clear();
    for (int i = 0; i < tableSize; ++i) {
        uint8_t b;
        uint64_t f;
        inStream.read(reinterpret_cast<char*>(&b), 1);
        inStream.read(reinterpret_cast<char*>(&f), 8);
        freqTable[b] = f;
    }

    std::cout << "[Decoder] Building Huffman Tree..." << std::endl;
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

    std::cout << "[Decoder] Tree built successfully. Commencing bitstream extraction..." << std::endl;
    HuffmanNode* current = root;
    uint64_t bytesDecoded = 0;
    bool bit;

    while (bytesDecoded < totalBytes) {
        // Safe fallback for single unique character files
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
            std::cout << "[Decoder Fatal] BitReader reached EOF unexpectedly at byte " << bytesDecoded << std::endl;
            return false;
        }
    }

    std::cout << "[Decoder] Extraction complete. " << bytesDecoded << " bytes restored." << std::endl;
    return true;
}