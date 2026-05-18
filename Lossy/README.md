# Lossy Audio Compression

This module focuses on evaluating the **Opus codec** for audio data compression. It includes a C++ encoder and a MATLAB script for objective signal analysis.

## 📁 Directory Structure

* `encode_opus.cpp` - C++ source code that reads a reference `.wav` file and encodes it into multiple `.opus` files at different bitrates (128, 64, 32, 8, and 1 kbps).
* `audio_analysis.m` - MATLAB script that compares the original `.wav` file with the compressed `.opus` files, generating time-domain waveforms, spectrograms, and FFT magnitude spectrums.
* `Beethoven_5th_Symphony.wav` - The reference audio file used for the encoding tests (expected in the root of this folder).

## 🛠️ Usage / How to Run

### 1. C++ Encoder
Ensure you have the `libopus` and `libsndfile` libraries installed in your development environment.
Compile the source code linking the required libraries. For example, using GCC:
```bash
g++ encode_opus.cpp -o encode_opus -lopus -lsndfile
```
Run the executable. It will process Beethoven_5th_Symphony.wav and output the encoded files into the resultados folder:
```bash
./encode_opus
```

### 2. MATLAB Analysis
1. Open audio_analysis.m in MATLAB.
2. Ensure that Beethoven_5th_Symphony.wav and the /resultados/ directory are in the same working path as the script.
3. Run the script. It will generate a figure comparing the original WAV and the compressed OPUS file across three domains: time comparison, spectrograms, and frequency spectrum.