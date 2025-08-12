## Description

Tools to create an archive of a folder and run compress/decompress
across multiple codecs and compression levels.

- Measures and logs: output size, reduction ratio, elapsed time,
  peak memory, average memory, and CPU usage for compression and
  decompression.
- Produces organized outputs under `Results_Lossless_*` containing
  compressed artifacts and logs.
- Implementations: C++ (`lossless_metrics.cpp`) and Python
  (`lossless_metrics.py`).

## Dependencies

- Brotli, Zstd and LZ4 libraries (system packages)
- Python: `psutil`



