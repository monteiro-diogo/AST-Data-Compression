import sys
import argparse
import brotli, zstandard as zstd, lz4.frame
import time, threading, psutil, os, shutil, tempfile, tarfile

def get_folder_size(path):
    total = 0
    for dirpath, _, filenames in os.walk(path):
        for f in filenames:
            fp = os.path.join(dirpath, f)
            if os.path.isfile(fp):
                total += os.path.getsize(fp)
    return total

def make_temp_archive(folder_path, archive_base):
    tar_path = f"{archive_base}.tar"
    with tarfile.open(tar_path, "w") as tar:
        tar.add(folder_path, arcname=os.path.basename(folder_path))
    return tar_path

# VALUES ------------------------------------------------------------
def medir_recursos(func, *args, **kwargs):
    proc = psutil.Process(os.getpid()) 
    proc.cpu_percent(interval=None)
    cpu_samples, mem_samples = [], []
    stop = threading.Event()

    def sampler():
        while not stop.is_set():
            cpu_samples.append(proc.cpu_percent(interval=0.1) / psutil.cpu_count())
            mem_samples.append(proc.memory_info().rss)
    thread = threading.Thread(target=sampler)
    thread.start()
    
    t0 = time.time()
    result = func(*args, **kwargs)
    elapsed = time.time() - t0
    stop.set()
    thread.join()

    size = result[0] if isinstance(result, tuple) else result
    avg_cpu = sum(cpu_samples) / len(cpu_samples) if cpu_samples else 0.0
    peak_mem = max(mem_samples) if mem_samples else 0
    avg_mem = sum(mem_samples) / len(mem_samples) if mem_samples else 0

    return (size, elapsed, peak_mem, avg_mem, avg_cpu)

# COMPRESSION ----------------------------------------------------------
def compress_brotli(input_path, output_path, level):
    compressor = brotli.Compressor(quality=level)
    temp_out = output_path + ".part"
    try:
        with open(input_path, 'rb') as f_in, open(temp_out, 'wb') as f_out:
            while True:
                chunk = f_in.read(1024 * 1024)
                if not chunk:
                    break
                f_out.write(compressor.process(chunk))
            f_out.write(compressor.finish())
    except KeyboardInterrupt:
        if os.path.exists(temp_out): os.remove(temp_out)
        raise
    os.replace(temp_out, output_path)
    return (os.path.getsize(output_path),)

def compress_zstd(input_path, output_path, level):
    cctx = zstd.ZstdCompressor(level=level)
    temp_out = output_path + ".part"
    try:
        with open(input_path, 'rb') as f_in, open(temp_out, 'wb') as f_out:
            cctx.copy_stream(f_in, f_out)
    except KeyboardInterrupt:
        if os.path.exists(temp_out): os.remove(temp_out)
        raise
    os.replace(temp_out, output_path)
    return (os.path.getsize(output_path),)

def compress_lz4(input_path, output_path, level):
    temp_out = output_path + ".part"
    with lz4.frame.open(temp_out, 'wb', compression_level=level) as f_out, \
         open(input_path, 'rb') as f_in:
        shutil.copyfileobj(f_in, f_out, length=1024*1024)
    os.replace(temp_out, output_path)
    return (os.path.getsize(output_path),)

# DECOMPRESSION -------------------------------------------------------
def decompress_brotli(input_path, output_path, _level=None):
    decompressor = brotli.Decompressor()
    temp_out = output_path + ".part"
    with open(input_path, 'rb') as f_in, open(temp_out, 'wb') as f_out:
        while True:
            chunk = f_in.read(1024*1024)
            if not chunk:
                break
            f_out.write(decompressor.process(chunk))
    os.replace(temp_out, output_path)
    return (os.path.getsize(output_path),)

def decompress_zstd(input_path, output_path, _level=None):
    dctx = zstd.ZstdDecompressor()
    temp_out = output_path + ".part"
    with open(input_path, 'rb') as f_in, open(temp_out, 'wb') as f_out:
        dctx.copy_stream(f_in, f_out)
    os.replace(temp_out, output_path)
    return (os.path.getsize(output_path),)

def decompress_lz4(input_path, output_path, _level=None):
    temp_out = output_path + ".part"
    with lz4.frame.open(input_path, 'rb') as f_in, open(temp_out, 'wb') as f_out:
        shutil.copyfileobj(f_in, f_out, length=1024*1024)
    os.replace(temp_out, output_path)
    return (os.path.getsize(output_path),)

# MAIN ----------------------------------------------------------------
def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("folder", help="Pasta a ser comprimida")
    args = parser.parse_args()

    folder_path = os.path.abspath(args.folder)
    if not os.path.isdir(folder_path):
        print(f"`{folder_path}` nao e uma pasta.")
        sys.exit(1)

    name = os.path.basename(folder_path)
    out_dir = os.path.join(os.path.dirname(folder_path), "Results_Lossless_Py")
    os.makedirs(out_dir, exist_ok=True)

    orig_size = get_folder_size(folder_path)
    print(f"Pasta: {name} (Original: {orig_size} bytes)")
    print(f"---------------------------------------------------------------------------------------")
    print("======================================= Compressao ====================================")
    with tempfile.TemporaryDirectory() as td:
        tar_path = make_temp_archive(folder_path, os.path.join(td, name))
        print(f"{'Metodo':<8}{'Nivel':<7}{'Tamanho(bytes)':<16}{'Reducao':<9}{'Tempo(s)':<10}{'PeakMem(bytes)':<16}{'AvgMem(bytes)':<15}{'CPU(%)':<8}")
        for level in range(1, 12):
            out = os.path.join(out_dir, f"{name}_brotli_{level:02}.br")
            size, tempo, peak_mem, avg_mem, avg_cpu = medir_recursos(compress_brotli, tar_path, out, level)
            if size is None:
                print(f"{'Brotli':<8}{f'{level:02}':<7} Interrompido. Operacao cancelada pelo utilizador.")
                break
            reduction = 100 * (1 - size / orig_size)
            print(f"{'Brotli':<8}{f'{level:02}':<7}"
                  f"{size:<16}"
                  f"{f'{reduction:.2f}%':<9}"
                  f"{tempo:<10.3f}"
                  f"{peak_mem:<16}"
                  f"{avg_mem:<15.0f}"
                  f"{f'{avg_cpu:.2f}%':<8}")
                  
        print(f"---------------------------------------------------------------------------------------")
        print(f"{'Metodo':<8}{'Nivel':<7}{'Tamanho(bytes)':<16}{'Reducao':<9}{'Tempo(s)':<10}{'PeakMem(bytes)':<16}{'AvgMem(bytes)':<15}{'CPU(%)':<8}")
        for level in range(1, 23):
            out = os.path.join(out_dir, f"{name}_zstd_{level:02}.zst")
            size, tempo, peak_mem, avg_mem, avg_cpu = medir_recursos(compress_zstd, tar_path, out, level)
            if size is None:
                print(f"{'Zstd':<8}{f'{level:02}':<7} Interrompido. Operacao cancelada pelo utilizador.")
                break
            reduction = 100 * (1 - size / orig_size)
            print(f"{'Zstd':<8}{f'{level:02}':<7}"
                  f"{size:<16}"
                  f"{f'{reduction:.2f}%':<9}"
                  f"{tempo:<10.3f}"
                  f"{peak_mem:<16}"
                  f"{avg_mem:<15.0f}"
                  f"{f'{avg_cpu:.2f}%':<8}")
                  
        print(f"---------------------------------------------------------------------------------------")
        print(f"{'Metodo':<8}{'Nivel':<7}{'Tamanho(bytes)':<16}{'Reducao':<9}{'Tempo(s)':<10}{'PeakMem(bytes)':<16}{'AvgMem(bytes)':<15}{'CPU(%)':<8}")
        for level in range(1, 17):
            out = os.path.join(out_dir, f"{name}_lz4_{level:02}.lz4")
            size, tempo, peak_mem, avg_mem, avg_cpu = medir_recursos(compress_lz4, tar_path, out, level)
            if size is None:
                print(f"{'LZ4':<8}{f'{level:02}':<7} Interrompido. Operacao cancelada pelo utilizador.")
                break
            reduction = 100 * (1 - size / orig_size)
            print(f"{'LZ4':<8}{f'{level:02}':<7}"
                  f"{size:<16}"
                  f"{f'{reduction:.2f}%':<9}"
                  f"{tempo:<10.3f}"
                  f"{peak_mem:<16}"
                  f"{avg_mem:<15.0f}"
                  f"{f'{avg_cpu:.2f}%':<8}")
                  
        print(f"---------------------------------------------------------------------------------------")
        print("==================================== Descompressao ====================================")
        print(f"{'OrigFile':<22}{'Tempo(s)':<10}{'PeakMem(bytes)':<16}{'AvgMem(bytes)':<15}{'CPU(%)':<8}")
        decom_dir = os.path.join(out_dir, "Decompressed_Py")
        os.makedirs(decom_dir, exist_ok=True)
        for fname in os.listdir(out_dir):
            if not fname.startswith(name + "_"): continue
            path = os.path.join(out_dir, fname)
            if fname.endswith('.br'):
                func = decompress_brotli
            elif fname.endswith('.zst'):
                func = decompress_zstd
            elif fname.endswith('.lz4'):
                func = decompress_lz4
            else:
                continue
            out_file = os.path.join(decom_dir, fname + ".tar")
            size, tempo, peak_mem, avg_mem, avg_cpu = medir_recursos(func, path, out_file, None)
            print(f"{fname:<22}{tempo:<10.3f}{peak_mem:<16}{avg_mem:<15.0f}{avg_cpu:<8.2f}")
        print(f"--------------------------------------------------------------------------------------")

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nOperação cancelada pelo utilizador.")
