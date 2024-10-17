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
                           std::string&& postfix, uintmax_t input_file_size) {
    std::unique_ptr<Archivator> archivator =
        std::make_unique<ArchivatorChild>();

    std::filesystem::path compressed_file =
        AddPostfix(output_compress_file, std::move(postfix));
    std::filesystem::path decompressed_file =
        AddPostfix(output_decompress_file, std::move(postfix));

    {
        LOG_DURATION("\tCompress time:");
        archivator->compress(input_file, compressed_file);
    }
    auto compressed_file_size =
        std::filesystem::file_size(compressed_file);
    std::cout << "\tCompressed file size: " << compressed_file_size
              << std::endl;
    double reduction = static_cast<double>(input_file_size) -
                       static_cast<double>(compressed_file_size);
    double compression_percentage =
        (reduction / static_cast<double>(input_file_size)) * 100.0;

    std::cout << "\tCompression percentage: " << compression_percentage << "%"
              << std::endl;

    if (auto huffman_archivator =
            dynamic_cast<HuffmanArchivator*>(archivator.get())) {
        std::cout << "\tRelative coding efficiency: "
                  << huffman_archivator->GetRelativeCodingEfficiency()
                  << std::endl;
    }

    {
        LOG_DURATION("\tDecompress time:");
        archivator->decompress(compressed_file,
                               decompressed_file);
    }
    std::cout << "\tDecompressed file size: "
              << std::filesystem::file_size(decompressed_file)
              << std::endl;
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

    auto input_file_size = std::filesystem::file_size(input_file);

    std::cout << "Input file size: " << input_file_size << std::endl;

    std::cout << "\nHuffman alghorithm: " << std::endl;
    CompressAndDecompress<HuffmanArchivator>(input_file, output_compress_file,
                                             output_decompress_file, "huffman",
                                             input_file_size);

    std::cout << "\nRLE alghorithm: " << std::endl;
    CompressAndDecompress<RleArchivator>(input_file, output_compress_file,
                                         output_decompress_file, "rle",
                                         input_file_size);

    std::cout << "\nProccess completed" << std::endl;

    return 0;
}
