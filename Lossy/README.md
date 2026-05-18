# Lossy Audio Compression

This module focuses on the compression of continuous analog phenomena (represented as audio waveforms) using the **Opus codec**. It includes both the encoding mechanisms and the objective signal analysis tools.

## 📁 Directory Structure

* `/AudioLossy/` & `AudioLossy.sln` - The C++ implementation utilizing the Opus codec to encode `.wav` reference files at various target bitrates (1 to 128 kbps).
* `Lossy Audio.m` - MATLAB script used for objective analysis (Time-domain waveforms, Spectrograms, and FFT Magnitude Spectrums).
* `audio_analysis.mlx` - MATLAB Live Script containing interactive signal analysis and visual results.

## 🔬 Methodology

1. **Encoding:** The C++ solution encodes reference `.wav` files into `.opus` format at predefined bitrates.
2. **Subjective Testing:** Informal listening tests to establish perceptual degradation thresholds.
3. **Objective Analysis:** Processing the compressed files through MATLAB to visualize high-frequency attenuation and spectral behavior.

## 🛠️ Usage / How to Run

### C++ Encoder
1. Open `AudioLossy.sln` in Visual Studio.
2. Build and run the project to generate the compressed `.opus` files in your designated output directory.

### MATLAB Analysis
1. Open `Lossy Audio.m` or `audio_analysis.mlx` in MATLAB.
2. Verify that the `file_wav` and `file_opus` paths correctly point to your generated audio files.
3. Run the script to generate the comparative signal plots.