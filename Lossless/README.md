# Lossless Data Compression Benchmark

This module contains the automated benchmarking suite designed to evaluate and compare the performance of various lossless compression algorithms across different programming environments (**Python** and **C++**). 

The suite measures execution time, compression ratios, and system resource utilization (CPU and memory) using a diverse test corpus.

## 📁 Module Structure

* `lossless_metrics.py` - Python benchmarking script that evaluates algorithms, tracks metrics using tools like `psutil`, and exports results.
* `lossless_metrics.cpp` - Optimized C++ benchmarking source code providing low-level execution data and high-precision timing.
* `dataset/` - The test corpus directory containing structured and unstructured data organized by categories:
  * `Excel/` - Tabular data sheets.
  * `PDF2TXT/` - Plain text extracted from documentation.
  * `Photos/` - Image assets.
  * `Text/` - Log files and unstructured text documents.

## 📊 Supported Algorithms

The benchmarking suite performs comprehensive level-sweeps across the following codecs:
* **Gzip / DEFLATE**
* **Brotli**
* **Zstd (Zstandard)**
* **BZ2 (bzip2)**
* **LZMA**
* **LZ4**

## 🛠️ Environment Setup & Execution

### 1. Python Implementation

The Python script automates the sweep across algorithms and handles data typologies inside the `dataset/` directory.

#### Dependencies
Ensure you have the required codec bindings and system monitoring libraries installed:
```bash
pip install psutil zstandard brotli lz4
```
#### Running the Benchmark
Execute the script from the terminal:

```bash
python lossless_metrics.py
```
The script will process the dataset and automatically generate a compiled metrics file named `results_python.csv.`

### 2. C++ Implementation
The C++ solution delivers maximum execution speed and deterministic memory tracking to establish a high-performance baseline.

#### Compilation
Compile the source code using your system's C++ compiler, ensuring all native development libraries for the target codecs (Zstd, Brotli, LZ4, Zlib) are correctly linked. For example, using GCC/Clang:

```bash
g++ lossless_metrics.cpp -o lossless_metrics -lz -lzstd -lbrotlienc -lbrotlidec -llz4
```
#### Running the Benchmark
Execute the compiled binary:

```bash
./lossless_metrics
```
The execution will run the test matrix over the local dataset/ folder and output the structured data directly into results_cpp.csv.