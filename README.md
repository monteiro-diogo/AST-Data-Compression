# AST Data Compression: Lossless & Lossy Evaluation

This repository contains the source code, datasets, and final report for my Summer Internship project at **Active Space Technologies**. 

The project focuses on evaluating and implementing both **lossless** and **lossy** data compression algorithms, targeting telemetry applications where reducing storage requirements and accelerating data transmission are critical.

## 🚀 Project Overview

The study evaluates the performance, compression ratios, and computational efficiency of various algorithms, comparing high-level (Python) and low-level (C++) implementations.

* **Lossless Compression:** Evaluation of Gzip, Brotli, Zstd, BZ2, LZMA, and LZ4. 
* **Lossy Compression:** Evaluation of the Opus codec for audio/analog signal data.
* **Implementation Comparison:** Benchmarking execution time and memory behavior between C++ and Python.

## 📁 Repository Structure

* [`/Lossless`](./Lossless/) - Contains the Python and C++ source code for evaluating lossless algorithms.
* [`/Lossy`](./Lossy/) - Contains the C++ Opus encoder implementation and MATLAB scripts for objective signal analysis.
* [`AST_Final_Report.pdf`](./AST_Final_Report.pdf) - The comprehensive final internship report detailing methodology, results, and conclusions.

## 📊 Key Findings

1. **Optimal Lossless Codec:** Zstd (at low to moderate levels) provides the best practical trade-off between compression ratio, runtime, and resource consumption.
2. **Optimal Lossy Codec:** Opus at 128 kbps delivers near-transparent perceptual quality for complex waveforms while yielding a ~91% file-size reduction.
3. **C++ vs. Python:** C++ implementations reduced compression time by approximately 18.2% and exhibited highly stable, predictable memory behavior, making it the superior choice for resource-constrained telemetry systems.
