# Lossless Data Compression

This module contains the implementations and benchmarks for various lossless compression algorithms (Gzip, Brotli, Zstd, BZ2, LZMA, and LZ4). The goal is to maximize data reduction for structured and unstructured telemetry data without data loss.

## 📁 Directory Structure

* `/Python_implementation/` - Python scripts utilized for rapid prototyping and baseline benchmarking.
* `/Lossless/` & `Lossless.sln` - The core C++ implementation and Visual Studio solution, optimized for speed and deterministic memory allocation.
* `/Data/` & `/dataset/` - The structured folders containing the test corpus (Excel, PDF2TXT, Photos, Text).

## 🛠️ Usage / How to Run

### C++ Implementation
1. Open `Lossless.sln` in Visual Studio.
2. Build the project in Release mode (x64) for accurate performance metrics.
3. Run the executable. Ensure the dataset paths in the source code point correctly to the `/dataset` directory.

### Python Implementation
1. Navigate to the `/Python_implementation/` directory.
2. Install the required dependencies: `pip install -r requirements.txt` *(Note: Update if you don't use a requirements file)*.
3. Execute the main benchmarking script: `python main.py` *(Note: Update with your actual script name)*.