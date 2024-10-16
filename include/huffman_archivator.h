#ifndef HUFFMAN_ARCHIVATOR_H
#define HUFFMAN_ARCHIVATOR_H

#include <fstream>
#include <queue>
#include <tuple>
#include <unordered_map>

#include "archivator.h"
#include "file_streams.h"

class HuffmanArchivator : public Archivator {
   public:
    HuffmanArchivator() = default;

    void compress(const std::filesystem::path& input_file_path,
                  const std::filesystem::path& output_file_path) override;
    void decompress(const std::filesystem::path& input_file_path,
                    const std::filesystem::path& output_file_path) override;

   private:
    struct HuffmanNode {
        char data = '\0';
        int freq = 0;
        std::shared_ptr<HuffmanNode> left = nullptr;
        std::shared_ptr<HuffmanNode> right = nullptr;

        HuffmanNode() = default;

        HuffmanNode(char data, int freq)
            : data(data), freq(freq), left(nullptr), right(nullptr) {}
    };

    struct HuffmanNodeCompare {
        bool operator()(std::shared_ptr<HuffmanNode> l,
                        std::shared_ptr<HuffmanNode> r) {
            return l->freq > r->freq;
        }
    };

    using huffman_priority_queue =
        std::priority_queue<std::shared_ptr<HuffmanNode>,
                            std::vector<std::shared_ptr<HuffmanNode>>,
                            HuffmanNodeCompare>;

    using huffman_code_map = std::unordered_map<char, std::string>;

    const char PADDING_KEY = '0';
    const char LEFT_BRANCH_KEY = '0';
    const char RIGHT_BRANCH_KEY = '1';

    FileStreams CheckFiles(const std::filesystem::path& input_file,
                           const std::filesystem::path& output_file);

    std::shared_ptr<HuffmanNode> FormHuffmanTree(
        const std::unordered_map<char, int>& char_frequency);
    void BuildCodeMap(const std::shared_ptr<HuffmanNode>& root,
                      const std::string& str, huffman_code_map& out_code_map);
    void WriteHeader(std::ofstream& output_file,
                     const huffman_code_map& code_map);
    void WriteEncodedText(std::ifstream& input_file, std::ofstream& output_file,
                          huffman_code_map& code_map);

    void ReadHeader(std::ifstream& input_file, huffman_code_map& out_code_map);
    std::shared_ptr<HuffmanNode> FormHuffmanTree(
        const huffman_code_map& code_map);
    void ReadEncodedString(std::ifstream& input_file,
                           std::string& out_encoded_string);
};

#endif
