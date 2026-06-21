#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include "Encoder.hpp"
#include "Decoder.hpp"

void printStats(const std::string& orig, const std::string& comp) {
    if (!std::filesystem::exists(orig) || !std::filesystem::exists(comp)) return;
    
    auto origSize = std::filesystem::file_size(orig);
    auto compSize = std::filesystem::file_size(comp);
    double ratio = origSize > 0 ? (static_cast<double>(compSize) / origSize) * 100.0 : 0.0;
    
    std::cout << "\n=== Compression Report ===\n"
              << "Original Size:   " << origSize << " bytes\n"
              << "Compressed Size: " << compSize << " bytes\n"
              << "Compression Ratio: " << ratio << "%\n" << std::endl;
}

int main(int argc, char* argv[]) {
    // CLI Fallback
    if (argc < 4) return 1;
    std::string mode = argv[1];
    std::string source = argv[2];
    std::string target = argv[3];

    if (mode == "-c") {
        Encoder enc;
        if (enc.compress(source, target)) printStats(source, target);
    } else if (mode == "-d") {
        Decoder dec;
        dec.decompress(source, target);
    }
    return 0;
}

// ---------------------------------------------------------
// WEBASSEMBLY API HOOKS (With Aggressive Console Logging)
// ---------------------------------------------------------
extern "C" {
    int compress(const char* inputPath, const char* outputPath) {
        std::cout << "-----------------------------------" << std::endl;
        std::cout << "[C++ Engine] Compression sequence initiated." << std::endl;
        std::cout << "[C++ Engine] Searching for target: " << inputPath << std::endl;

        if (!std::filesystem::exists(inputPath)) {
            std::cout << "[C++ Fatal] File does not exist in MEMFS sandbox." << std::endl;
            return -1;
        }

        std::cout << "[C++ Engine] File validated. Size: " << std::filesystem::file_size(inputPath) << " bytes." << std::endl;

        // Verify C++ standard library can open the sandboxed file
        std::ifstream testIn(inputPath, std::ios::binary);
        if (!testIn.is_open()) {
            std::cout << "[C++ Fatal] std::ifstream failed to open input file." << std::endl;
            return -1;
        }
        testIn.close();

        std::cout << "[C++ Engine] Memory checks passed. Handing off to Encoder::compress()..." << std::endl;
        
        Encoder enc;
        bool status = enc.compress(std::string(inputPath), std::string(outputPath));

        if (!status) {
            std::cout << "[C++ Fatal] Encoder::compress() encountered an internal algorithm failure and returned FALSE." << std::endl;
            return -1;
        }

        std::cout << "[C++ Engine] Compression algorithm completed successfully! Returning asset to JavaScript." << std::endl;
        return 0;
    }

    int decompress(const char* inputPath, const char* outputPath) {
        std::cout << "-----------------------------------" << std::endl;
        std::cout << "[C++ Engine] Decompression sequence initiated on: " << inputPath << std::endl;

        if (!std::filesystem::exists(inputPath)) {
            std::cout << "[C++ Fatal] Compressed file missing from sandbox." << std::endl;
            return -1;
        }

        Decoder dec;
        bool status = dec.decompress(std::string(inputPath), std::string(outputPath));

        if (!status) {
            std::cout << "[C++ Fatal] Decoder::decompress() failed. Corrupted file or algorithm error." << std::endl;
            return -1;
        }

        std::cout << "[C++ Engine] Decompression successful!" << std::endl;
        return 0;
    }
} 
