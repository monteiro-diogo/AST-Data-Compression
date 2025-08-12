#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <iomanip>
#include <sys/resource.h>
#include <unistd.h>
#include <brotli/encode.h>
#include <brotli/decode.h>
#include <zstd.h>
#include <lz4frame.h>

namespace fs = std::filesystem;
const unsigned CORES = std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : 1;
std::atomic<size_t> mem_sum(0);
std::atomic<size_t> mem_count(0);

size_t get_memory_usage() {
    std::ifstream status("/proc/self/status");
    std::string line;
    while (std::getline(status, line)) {
        if (line.rfind("VmRSS:", 0) == 0) {
            std::istringstream iss(line);
            std::string key, value, unit;
            iss >> key >> value >> unit;
            return std::stoul(value) * 1024;
        }
    }
    return 0;
}

std::atomic<bool> stop_sampling(false);
std::atomic<size_t> peak_net_mem(0);

void sample_memory_usage(size_t mem_before) {
    while (!stop_sampling.load()) {
        size_t current = get_memory_usage();
        size_t net = (current > mem_before) ? (current - mem_before) : 0;

        size_t prev = peak_net_mem.load();
        if (net > prev) peak_net_mem.store(net);

        mem_sum += net;
        mem_count += 1;

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

std::vector<char> read_file(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    return std::vector<char>(std::istreambuf_iterator<char>(in),
                             std::istreambuf_iterator<char>());
}

std::vector<char> brotli_compress_bytes(const std::vector<char>& input, int level) {
    size_t max_out = BrotliEncoderMaxCompressedSize(input.size());
    std::vector<char> output(max_out);
    size_t out_size = max_out;
    if (!BrotliEncoderCompress(level, BROTLI_DEFAULT_WINDOW, BROTLI_MODE_GENERIC,
                               input.size(), reinterpret_cast<const uint8_t*>(input.data()),
                               &out_size, reinterpret_cast<uint8_t*>(output.data()))) {
        throw std::runtime_error("Erro na compressão Brotli.");
    }
    output.resize(out_size);
    return output;
}

std::vector<char> zstd_compress_bytes(const std::vector<char>& input, int level) {
    ZSTD_CCtx* ctx = ZSTD_createCCtx();
    size_t max_out = ZSTD_compressBound(input.size());
    std::vector<char> output(max_out);
    size_t out_size = ZSTD_compressCCtx(ctx, output.data(), max_out,
                                        input.data(), input.size(), level);
    ZSTD_freeCCtx(ctx);
    if (ZSTD_isError(out_size)) {
        throw std::runtime_error("Erro na compressão Zstd: " + std::string(ZSTD_getErrorName(out_size)));
    }
    output.resize(out_size);
    return output;
}

std::vector<char> lz4_compress_bytes(const std::vector<char>& input, int level) {
    size_t max_out = LZ4F_compressFrameBound(input.size(), nullptr);
    std::vector<char> output(max_out);
    LZ4F_preferences_t prefs = {};
    prefs.compressionLevel = level;
    size_t out_size = LZ4F_compressFrame(output.data(), max_out,
                                         input.data(), input.size(), &prefs);
    if (LZ4F_isError(out_size)) {
        throw std::runtime_error("Erro na compressão LZ4: " + std::string(LZ4F_getErrorName(out_size)));
    }
    output.resize(out_size);
    return output;
}

void process_and_save(const std::string& method, const std::vector<char>& data, int level,
                      size_t orig_size, const fs::path& out_file) {
    size_t mem_before = get_memory_usage();
    peak_net_mem.store(0);
    stop_sampling.store(false);
    mem_sum.store(0);
    mem_count.store(0);
    std::thread mem_thread(sample_memory_usage, mem_before);

    struct rusage ru_before, ru_after;
    getrusage(RUSAGE_SELF, &ru_before);
    auto t0 = std::chrono::high_resolution_clock::now();

    std::vector<char> out;
    if (method == "Brotli") out = brotli_compress_bytes(data, level);
    else if (method == "Zstd") out = zstd_compress_bytes(data, level);
    else if (method == "LZ4") out = lz4_compress_bytes(data, level);

    auto t1 = std::chrono::high_resolution_clock::now();
    getrusage(RUSAGE_SELF, &ru_after);

    stop_sampling.store(true);
    mem_thread.join();
    size_t peak = peak_net_mem.load();
    double avg_mem = (mem_count > 0) ? static_cast<double>(mem_sum.load()) / mem_count : 0.0;


    double elapsed = std::chrono::duration<double>(t1 - t0).count();
    double user_diff = (ru_after.ru_utime.tv_sec - ru_before.ru_utime.tv_sec)
                     + (ru_after.ru_utime.tv_usec - ru_before.ru_utime.tv_usec)/1e6;
    double sys_diff  = (ru_after.ru_stime.tv_sec - ru_before.ru_stime.tv_sec)
                     + (ru_after.ru_stime.tv_usec - ru_before.ru_stime.tv_usec)/1e6;
    double cpu_pct = (elapsed > 0)
                   ? 100.0 * (user_diff + sys_diff) / (elapsed * CORES)
                   : 0.0;

    std::ofstream ofs(out_file, std::ios::binary);
    ofs.write(out.data(), out.size());
    ofs.close();

    size_t out_size = out.size();
    double reduction = 100.0 * (1.0 - double(out_size) / orig_size);

    std::ostringstream lvl_ss;
lvl_ss << std::setw(2) << std::setfill('0') << level;

std::ostringstream red_ss;
red_ss << std::fixed << std::setprecision(2) << reduction << "%";

std::ostringstream time_ss;
time_ss << std::fixed << std::setprecision(3) << elapsed << "s";

std::ostringstream cpu_ss;
cpu_ss << std::fixed << std::setprecision(2) << cpu_pct << "%";

std::cout << std::left
          << std::setw(8)  << method
          << std::setw(8)  << lvl_ss.str()
          << std::setw(12) << out_size
          << std::setw(10) << red_ss.str()
          << std::setw(10) << time_ss.str()
          << std::setw(12) << peak
          << std::setw(12) << static_cast<size_t>(avg_mem)
          << cpu_ss.str() << "\n";

}

std::vector<char> brotli_decompress(const std::vector<char>& input) {
    BrotliDecoderState* state = BrotliDecoderCreateInstance(nullptr, nullptr, nullptr);
    if (!state) throw std::runtime_error("Erro ao criar estado Brotli.");

    size_t available_in = input.size();
    const uint8_t* next_in = reinterpret_cast<const uint8_t*>(input.data());

    std::vector<char> output;
    std::vector<uint8_t> buffer(1024 * 1024);

    while (true) {
        size_t available_out = buffer.size();
        uint8_t* next_out = buffer.data();
        BrotliDecoderResult result = BrotliDecoderDecompressStream(
            state, &available_in, &next_in, &available_out, &next_out, nullptr
        );
        size_t written = next_out - buffer.data();
        output.insert(output.end(), buffer.begin(), buffer.begin() + written);
        if (result == BROTLI_DECODER_RESULT_SUCCESS) break;
        if (result == BROTLI_DECODER_RESULT_ERROR)
            throw std::runtime_error("Erro na descompressão Brotli.");
    }

    BrotliDecoderDestroyInstance(state);
    return output;
}

std::vector<char> zstd_decompress(const std::vector<char>& input) {
    unsigned long long rSize = ZSTD_getFrameContentSize(input.data(), input.size());
    if (rSize == ZSTD_CONTENTSIZE_ERROR || rSize == ZSTD_CONTENTSIZE_UNKNOWN)
        throw std::runtime_error("Tamanho de conteúdo Zstd desconhecido ou inválido.");
    std::vector<char> output(rSize);
    size_t dSize = ZSTD_decompress(output.data(), rSize, input.data(), input.size());
    if (ZSTD_isError(dSize))
        throw std::runtime_error("Erro na descompressão Zstd: " + std::string(ZSTD_getErrorName(dSize)));
    return output;
}

std::vector<char> lz4_decompress(const std::vector<char>& input) {
    LZ4F_dctx* dctx;
    if (LZ4F_isError(LZ4F_createDecompressionContext(&dctx, LZ4F_VERSION))) {
        throw std::runtime_error("Erro ao criar contexto LZ4.");
    }

    size_t src_size = input.size();
    const char* src = input.data();
    std::vector<char> output;
    std::vector<char> buffer(1024 * 1024);

    size_t src_offset = 0;
    while (src_offset < src_size) {
        size_t src_remaining = src_size - src_offset;
        size_t dst_size = buffer.size();
        size_t src_size_chunk = src_remaining;

        size_t ret = LZ4F_decompress(
            dctx,
            buffer.data(), &dst_size,
            src + src_offset, &src_size_chunk,
            nullptr
        );

        if (LZ4F_isError(ret))
            throw std::runtime_error("Erro na descompressão LZ4: " + std::string(LZ4F_getErrorName(ret)));

        output.insert(output.end(), buffer.begin(), buffer.begin() + dst_size);
        src_offset += src_size_chunk;
        if (ret == 0) break; // done
    }

    LZ4F_freeDecompressionContext(dctx);
    return output;
}

void decompress_with_metrics(const fs::path& in_path, const fs::path& out_path) {
    std::string ext = in_path.extension().string();
    std::vector<char> input = read_file(in_path.string());

    size_t mem_before = get_memory_usage();
    peak_net_mem.store(0);
    mem_sum.store(0);
    mem_count.store(0);
    stop_sampling.store(false);
    std::thread mem_thread(sample_memory_usage, mem_before);

    struct rusage ru_before, ru_after;
    getrusage(RUSAGE_SELF, &ru_before);
    auto t0 = std::chrono::high_resolution_clock::now();

    std::vector<char> output;
    if (ext == ".br") {
        output = brotli_decompress(input);
    } else if (ext == ".zst") {
        output = zstd_decompress(input);
    } else if (ext == ".lz4") {
        output = lz4_decompress(input);
    } else {
        std::cerr << "Extensão não suportada: " << ext << "\n";
        return;
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    getrusage(RUSAGE_SELF, &ru_after);

    stop_sampling.store(true);
    mem_thread.join();

    double elapsed = std::chrono::duration<double>(t1 - t0).count();
    double user_diff = (ru_after.ru_utime.tv_sec - ru_before.ru_utime.tv_sec)
                     + (ru_after.ru_utime.tv_usec - ru_before.ru_utime.tv_usec) / 1e6;
    double sys_diff  = (ru_after.ru_stime.tv_sec - ru_before.ru_stime.tv_sec)
                     + (ru_after.ru_stime.tv_usec - ru_before.ru_stime.tv_usec) / 1e6;
    double cpu_pct = (elapsed > 0) ? 100.0 * (user_diff + sys_diff) / (elapsed * CORES) : 0.0;
    size_t peak = peak_net_mem.load();
    double avg_mem = (mem_count > 0) ? static_cast<double>(mem_sum.load()) / mem_count : 0.0;

    std::ofstream out(out_path, std::ios::binary);
    out.write(output.data(), output.size());
    out.close();

    std::ostringstream time_ss;
    time_ss << std::fixed << std::setprecision(3) << elapsed << "s";

    std::ostringstream cpu_ss;
    cpu_ss << std::fixed << std::setprecision(2) << cpu_pct << "%";

    std::cout << std::left
          << std::setw(24) << in_path.filename().string()
          << std::setw(10) << time_ss.str()
          << std::setw(12) << peak
          << std::setw(12) << static_cast<size_t>(avg_mem)
          << cpu_ss.str() << "\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: ./compress_isolated <pasta>\n";
        return 1;
    }

    fs::path folder = argv[1];
    if (!fs::is_directory(folder)) {
        std::cerr << "Erro: não é uma pasta: " << folder << "\n";
        return 1;
    }

    fs::path out_dir = folder.parent_path() / "Results_Lossless_Cpp";
    fs::create_directories(out_dir);

    std::string base = folder.filename().string();
    fs::path tar_path = out_dir / (base + ".tar");
    std::string cmd = "tar -cf \"" + tar_path.string() + "\" -C \"" + folder.string() + "\" .";
    if (std::system(cmd.c_str()) != 0) {
        std::cerr << "Erro ao criar tar: " << folder << "\n";
        return 1;
    }

    size_t orig_size = 0;
    for (auto& p : fs::recursive_directory_iterator(folder)) {
        if (fs::is_regular_file(p)) orig_size += fs::file_size(p);
    }

    auto data = read_file(tar_path.string());
    fs::remove(tar_path);
    if (data.empty()) {
        std::cerr << "Erro: tar vazio ou não lido corretamente.\n";
        return 1;
    }

    std::cout << "\n=== COMPRESSÃO ===\n";
    std::cout << std::left
          << std::setw(9)  << "Método"
          << std::setw(9)  << "Nível"
          << std::setw(12) << "Tamanho"
          << std::setw(12) << "Redução"
          << std::setw(10) << "Tempo"
          << std::setw(12) << "PeakMem"
          << std::setw(12) << "AvgMem"
          << "CPU\n";

    for (int lvl = 1; lvl <= 11; ++lvl) {
        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0') << lvl;
        std::string lvl_str = oss.str();
        
        process_and_save("Brotli", data, lvl, orig_size,
                         out_dir / (base + "_brotli_" + lvl_str + ".br"));
    }
    for (int lvl = 1; lvl <= 22; ++lvl) {
        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0') << lvl;
        std::string lvl_str = oss.str();
        process_and_save("Zstd", data, lvl, orig_size,
                         out_dir / (base + "_zstd_" + lvl_str + ".zst"));
    }
    for (int lvl = 0; lvl <= 16; ++lvl) {
        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0') << lvl;
        std::string lvl_str = oss.str();
        process_and_save("LZ4", data, lvl, orig_size,
                         out_dir / (base + "_lz4_" + lvl_str + ".lz4"));
    }

    fs::path out_decompressed = out_dir.parent_path() / "Decompressed_Cpp";
    fs::create_directories(out_decompressed);

    std::cout << "\n=== DESCOMPRESSÃO ===\n";
    std::cout << std::left
          << std::setw(24) << "Arquivo"
          << std::setw(10) << "Tempo"
          << std::setw(12) << "PeakMem"
          << std::setw(12) << "AvgMem"
          << "CPU\n";

    for (const auto& entry : fs::directory_iterator(out_dir)) {
        if (!fs::is_regular_file(entry.path())) continue;

        std::string ext = entry.path().extension().string();
        if (ext != ".br" && ext != ".zst" && ext != ".lz4") continue;

        std::string base_name = entry.path().stem().string();
        fs::path out_path = out_decompressed / (base_name + ".tar");

        try {
            decompress_with_metrics(entry.path(), out_path);
        } catch (const std::exception& ex) {
            std::cerr << "Erro em " << entry.path().filename() << ": " << ex.what() << "\n";
        }
    }
    return 0;
}
