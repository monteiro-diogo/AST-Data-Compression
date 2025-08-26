#include <iostream>
#include <vector>
#include <sndfile.h>
#include <opus/opusenc.h>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <ficheiro.wav>\n";
        return 1;
    }

    fs::create_directories("resultados");

    const char* input_path = argv[1];
    std::string base_name = std::string(input_path);
    size_t dot = base_name.find_last_of('/');
    if (dot != std::string::npos) base_name = base_name.substr(dot + 1);
    dot = base_name.find_last_of('.');
    if (dot != std::string::npos) base_name = base_name.substr(0, dot);

    // abrir WAV
    SF_INFO sfinfo{};
    SNDFILE* infile = sf_open(input_path, SFM_READ, &sfinfo);
    if (!infile) {
        std::cerr << "Erro a abrir WAV: " << sf_strerror(NULL) << "\n";
        return 1;
    }

    // taxas de bits que queremos testars
    std::vector<int> bitrates = {8000, 32000, 128000};

    const int frame_size = sfinfo.samplerate / 50; // 20 ms
    std::vector<opus_int16> pcm(frame_size * sfinfo.channels);

    for (int br : bitrates) {
        // construir nome do ficheiro
        std::string output_path = "resultados/" + base_name + "_" + std::to_string(br/1000) + "kbps.opus";

        // resetar posição do ficheiro para o início
        sf_seek(infile, 0, SEEK_SET);

        // criar comentários e encoder Opus
        OggOpusComments* comments = ope_comments_create();
        ope_comments_add_string(comments, "ENCODER=MeuEncoder");

        int error;
        OggOpusEnc* enc = ope_encoder_create_file(
            output_path.c_str(),
            comments,
            sfinfo.samplerate,
            sfinfo.channels,
            0, // OPUS_APPLICATION_AUDIO
            &error
        );

        if (!enc || error != OPE_OK) {
            std::cerr << "Erro a criar encoder (" << br << "): " << ope_strerror(error) << "\n";
            continue;
        }

        ope_encoder_ctl(enc, OPUS_SET_BITRATE(br));
        ope_encoder_ctl(enc, OPUS_SET_VBR(1));
        ope_encoder_ctl(enc, OPUS_SET_VBR_CONSTRAINT(0));

        sf_count_t readcount;
        while ((readcount = sf_readf_short(infile, pcm.data(), frame_size)) > 0) {
            if (readcount < frame_size) {
                std::fill(pcm.begin() + readcount * sfinfo.channels, pcm.end(), 0);
            }
            if ((error = ope_encoder_write(enc, pcm.data(), frame_size)) != OPE_OK) {
                std::cerr << "Erro a codificar (" << br << "): " << ope_strerror(error) << "\n";
                break;
            }
        }

        ope_encoder_drain(enc);
        ope_encoder_destroy(enc);

        std::cout << "Compressão concluída: " << output_path << "\n";
    }

    sf_close(infile);
    return 0;
}
