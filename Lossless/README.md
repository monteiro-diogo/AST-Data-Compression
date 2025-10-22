# Lossless Testing
## Description
⚠️ The purpose of these scripts is to run benchmarking tests on each method's performance for comparison.
___

### Project Overview
- Methods: `gzip`, `Brotli`, `Zstd`, `BZ2`, `LZMA`, and `LZ4`
- Measures: `output size`, `reduction ratio`, `elapsed time`, `peak memory`, `average memory`, and `CPU usage` for **compression** and **decompression**.
- Implementations: C++ (`lossless_metrics.cpp`) and Python (`lossless_metrics.py`).

All tests used the same dataset, comprising a mix of file types (documents, datasheets and images).



