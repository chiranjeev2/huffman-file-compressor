# WebAssembly Huffman File Compressor 🗜️

> A blazing-fast, edge-computed file compressor leveraging native C++17 and WebAssembly for secure, zero-latency data reduction directly inside the browser.

![Build Status](https://img.shields.io/badge/build-passing-brightgreen)
![Deployment](https://img.shields.io/badge/deployment-Vercel-black)
![C++](https://img.shields.io/badge/C++-17-blue)
![WebAssembly](https://img.shields.io/badge/WebAssembly-WASM-purple)

## 🚀 Overview

This project brings the raw computational speed of C++ to the web. It uses a custom-built Huffman Coding algorithm to losslessly compress and decompress dense binary files (like PDFs, images, and text) entirely on the client side. 

Because the compression happens locally via WebAssembly, **user files never leave their device.** There are no backend servers, no upload wait times, and absolute data privacy.

## ✨ Key Features

* **Near-Native Speeds:** C++17 algorithms compiled directly to WebAssembly (`.wasm`) for high-performance bit-level manipulation.
* **Deterministic Compression:** Utilizes alphabetically sorted frequency tables and priority queues to prevent "Hash Map Shuffle," guaranteeing perfect cross-platform tree synchronization.
* **Zero-Latency Processing:** Operates entirely in the browser using the JavaScript `FileReader` and `Blob` APIs. No server uploads required.
* **Automated CI/CD Pipeline:** Integrated GitHub Actions automatically spin up an Emscripten toolchain, compile the native C++ code to WASM, and deploy the new binaries directly to Vercel upon every push.
* **Binary Safe:** Implements strict byte-padding (`00000000` flushes) to ensure no hanging bits are lost during the memory buffer's write to the virtual disk.

## 🛠️ Technology Stack

* **Core Engine:** C++17
* **Compiler:** Emscripten Toolchain (`emcc`)
* **Web Integration:** WebAssembly (WASM), Virtual Memory File System (MEMFS), `ccall` string bridging
* **Frontend:** HTML5, CSS3, JavaScript (ES6)
* **DevOps:** GitHub Actions, Vercel

## 🧠 How It Works (Under the Hood)

1. **File Ingestion:** The JavaScript frontend reads a user's uploaded file as a raw `Uint8Array`.
2. **Virtual Sandboxing:** The JS thread mounts the byte array directly into Emscripten's Virtual Memory File System (MEMFS).
3. **Cross-Language Bridging:** JS triggers the C++ engine using the `ccall` memory bridge, safely passing virtual file paths without memory leaks.
4. **C++ Execution:** The native C++ algorithm reads the bytes, builds a deterministic Huffman tree, generates bit-codes, and writes the highly packed binary `.huff` file back into MEMFS.
5. **Extraction:** JS retrieves the new binary file from MEMFS, bundles it into a `Blob`, and forces a local browser download. 

## 📂 Project Structure

```text
├── .github/workflows/
│   └── compile.yml        # GitHub Actions CI/CD Emscripten pipeline
├── include/
│   ├── BitReaderWriter.hpp # Custom bit-level stream handlers
│   ├── Decoder.hpp         # Huffman decompression logic
│   └── Encoder.hpp         # Huffman compression logic
├── public/
│   ├── index.html          # Web application UI
│   ├── huffman_wasm.js     # Emscripten JS glue code (Auto-generated)
│   └── huffman_wasm.wasm   # Compiled WebAssembly binary (Auto-generated)
└── src/
    ├── BitReaderWriter.cpp
    ├── Decoder.cpp
    ├── Encoder.cpp
    └── main.cpp            # Engine entry point and WASM hooks
