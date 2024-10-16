#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

#include "huffman_archivator.h"
#include "log_duration.h"
#include "rle_archivator.h"

std::filesystem::path AddPostfix(const std::filesystem::path& path,
                                 std::string&& postfix) {
    std::filesystem::path result = path;
    result.replace_extension(postfix + result.extension().string());
    return result;
}

template <typename ArchivatorChild>
void CompressAndDecompress(const std::filesystem::path& input_file,
                           const std::filesystem::path& output_compress_file,
                           const std::filesystem::path& output_decompress_file,
                           std::string&& postfix) {
    std::unique_ptr<Archivator> archivator =
        std::make_unique<ArchivatorChild>();

    std::filesystem::path compressed_huffman_file =
        AddPostfix(output_compress_file, std::move(postfix));
    std::filesystem::path decompressed_huffman_file =
        AddPostfix(output_decompress_file, std::move(postfix));

    {
        LOG_DURATION("Compress " + postfix + ":");
        archivator->compress(input_file, compressed_huffman_file);
    }

    {
        LOG_DURATION("Decompress " + postfix + ":");
        archivator->decompress(compressed_huffman_file,
                               decompressed_huffman_file);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout << "Need to use: <input_file> <output_compress_file> "
                     "<output_decompress_file>"
                  << std::endl;
        return 1;
    }

    std::filesystem::path input_file = argv[1];
    std::filesystem::path output_compress_file = argv[2];
    std::filesystem::path output_decompress_file = argv[3];

    CompressAndDecompress<HuffmanArchivator>(input_file, output_compress_file,
                                             output_decompress_file, "huffman");

    CompressAndDecompress<RleArchivator>(input_file, output_compress_file,
                                         output_decompress_file, "rle");

    std::cout << "Proccess completed" << std::endl;

    return 0;
}
