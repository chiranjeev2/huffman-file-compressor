#include <iostream>
#include <string>
#include <filesystem>
#include "Encoder.hpp"
#include "Decoder.hpp"

void printStats(const std::string& orig, const std::string& comp) {
    // Explicit check to ensure std::filesystem doesn't throw an error inside the WASM sandbox
    if (!std::filesystem::exists(orig) || !std::filesystem::exists(comp)) {
        return;
    }
    auto origSize = std::filesystem::file_size(orig);
    auto compSize = std::filesystem::file_size(comp);
    double ratio = 0.0;
    if (origSize > 0) {
        ratio = (static_cast<double>(compSize) / origSize) * 100.0;
    }
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
        if (!inputPath || !outputPath) return -1;
        
        std::string srcPath(inputPath);
        std::string dstPath(outputPath);

        // Force relative path mapping prefix if not present to ensure MEMFS hit consistency
        if (srcPath.find('/') == std::string::npos && srcPath.find("\\") == std::string::npos) {
            srcPath = "./" + srcPath;
        }
        if (dstPath.find('/') == std::string::npos && dstPath.find("\\") == std::string::npos) {
            dstPath = "./" + dstPath;
        }

        Encoder enc;
        bool status = enc.compress(srcPath, dstPath);
        return status ? 0 : -1;
    }

    int decompress(const char* inputPath, const char* outputPath) {
        if (!inputPath || !outputPath) return -1;

        std::string srcPath(inputPath);
        std::string dstPath(outputPath);

        if (srcPath.find('/') == std::string::npos && srcPath.find("\\") == std::string::npos) {
            srcPath = "./" + srcPath;
        }
        if (dstPath.find('/') == std::string::npos && dstPath.find("\\") == std::string::npos) {
            dstPath = "./" + dstPath;
        }

        Decoder dec;
        bool status = dec.decompress(srcPath, dstPath);
        return status ? 0 : -1;
    }
}