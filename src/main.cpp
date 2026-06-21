#include <iostream>
#include <string>
#include <filesystem>
#include "Encoder.hpp"
#include "Decoder.hpp"

void printStats(const std::string& orig, const std::string& comp) {
    auto origSize = std::filesystem::file_size(orig);
    auto compSize = std::filesystem::file_size(comp);
    double ratio = (static_cast<double>(compSize) / origSize) * 100.0;
    std::cout << "\n=== Compression Report ===\n"
              << "Original Size:   " << origSize << " bytes\n"
              << "Compressed Size: " << compSize << " bytes\n"
              << "Compression Ratio: " << ratio << "%\n";
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage:\n  " << argv[0] << " -c <input> <output.huff>\n  " 
                  << argv[0] << " -d <input.huff> <output>\n";
        return 1;
    }

    std::string mode = argv[1];
    std::string source = argv[2];
    std::string target = argv[3];

    if (mode == "-c") {
        Encoder enc;
        if (enc.compress(source, target)) {
            std::cout << "Compressed successfully.\n";
            printStats(source, target);
        } else {
            std::cerr << "Compression failed.\n";
        }
    } else if (mode == "-d") {
        Decoder dec;
        if (dec.decompress(source, target)) {
            std::cout << "Decompressed successfully into " << target << "\n";
        } else {
            std::cerr << "Decompression failed.\n";
        }
    }
    return 0;
}

// These are the core C API hooks for the web browser
extern "C" {
    int compress(const char* inputPath, const char* outputPath) {
        Encoder enc;
        return enc.compress(std::string(inputPath), std::string(outputPath)) ? 0 : -1;
    }

    int decompress(const char* inputPath, const char* outputPath) {
        Decoder dec;
        return dec.decompress(std::string(inputPath), std::string(outputPath)) ? 0 : -1;
    }
}