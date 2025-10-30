# Lossless Testing
## Description
⚠️ The purpose of these scripts is to run benchmarking tests on each method's performance for comparison.
___

### Project Overview
- Methods: `gzip`, `Brotli`, `Zstd`, `BZ2`, `LZMA`, and `LZ4`
- Measures: `output size`, `reduction ratio`, `elapsed time`, `peak memory`, `average memory`, and `CPU usage` for **compression** and **decompression**.
- Implementations: C++ (`lossless_metrics.cpp`) and Python (`lossless_metrics.py`).

All tests used the same dataset, comprising a mix of file types (documents, datasheets and images).

___
### References
You can also preview the results online [here](https://onedrive.live.com/personal/497d77d174738fd9/_layouts/15/Doc.aspx?sourcedoc=%7B342d219b-7c36-40e3-8513-c7bc8f8b4363%7D&action=default&nav=MTVfezI4RTQ0NEQ0LTkyMEQtNDBFRS1CMTc1LUEyOEMzQTA0MUNBRH0&redeem=aHR0cHM6Ly8xZHJ2Lm1zL3gvYy80OTdkNzdkMTc0NzM4ZmQ5L0Vac2hMVFEyZk9OQWhSUEh2SS1MUTJNQnNMMW9xYmlvdmJ1eFRTT3pxaWJ0UGc_ZT1NUGhJWGQmbmF2PU1UVmZlekk0UlRRME5FUTBMVGt5TUVRdE5EQkZSUzFDTVRjMUxVRXlPRU16UVRBME1VTkJSSDA&slrid=dd7ad4a1-105d-e000-7c09-9d5ef9e8534e&originalPath=aHR0cHM6Ly8xZHJ2Lm1zL3gvYy80OTdkNzdkMTc0NzM4ZmQ5L0Vac2hMVFEyZk9OQWhSUEh2SS1MUTJNQnNMMW9xYmlvdmJ1eFRTT3pxaWJ0UGc_cnRpbWU9Y3ZPTTlyOFgza2cmbmF2PU1UVmZlekk0UlRRME5FUTBMVGt5TUVRdE5EQkZSUzFDTVRjMUxVRXlPRU16UVRBME1VTkJSSDA&CID=c9453693-5e28-4f1b-abcf-b2a1ebaf3060&_SRM=0:G:91).



