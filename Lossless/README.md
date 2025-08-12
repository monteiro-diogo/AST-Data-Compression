## Description

My goal was to find the "*best lossless compression method*". 

I evaluated `gzip`, `Brotli`, `Zstd`, `BZ2`, `LZMA`, and `LZ4` using a Python script that tracked time and compression rates at each codec's default level.
After the first research, i concluded that:
- `Brotli` had the highest compression rates, `LZ4` had the faster times and `Zstd` had the highest `compression rate`/`time` ratio which we will call `CTR` from now on.

With those 3 methods in mind i proceeded to study the following for each level of them:
- Measures: `output size`, `reduction ratio`, `elapsed time`, `peak memory`, `average memory`, and `CPU usage` for **compression** and **decompression**.
- Implementations: C++ (`lossless_metrics.cpp`) and Python (`lossless_metrics.py`).

## Observations

After this study i found that:
1. High `Brotli` levels take significantly longer while giving only marginal gains versus high `Zstd` levels. *(e.g On max level, Brotli (11) compressed 81.05% on 288s and Zstd (22) 80.31% on 146s)*  
2. High levels of `LZ4` (≥ 11) do not compress nearly as much as comparable levels of Brotli/Zstd, which lowers its CTR.
3. High levels of `Zstd` (≥ 19) can strain or fail on low-spec machines and may be impractical for some rigs. 
4. The `C++` implementation achieved lower elapsed times while producing comparable compression ratios to the `Python` scripts.

All tests used the same dataset, comprising a mix of file types (.txt, .jpg, etc.).

## Conclusion
Overall, lower to moderate compression levels of Zstd (levels 1 to 12) deliver the optimal trade-off between speed and compression ratio across the tested dataset and hardware. Based on my results, I can confidently conclude that **Zstd was the most effective lossless compression** method for this dataset.


