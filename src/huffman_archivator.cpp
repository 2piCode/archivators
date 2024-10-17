#include "huffman_archivator.h"

#include <bitset>
#include <cmath>
#include <exception>
#include <iostream>

void HuffmanArchivator::compress(
    const std::filesystem::path& input_file_path,
    const std::filesystem::path& output_file_path) {
    auto [input_file, output_file] =
        CheckFiles(input_file_path, output_file_path);

    std::unordered_map<char, int> char_frequency;
    char ch;
    size_t total_chars = 0;
    while (input_file.get(ch)) {
        char_frequency[ch]++;
        total_chars++;
    }

    if (char_frequency.empty()) {
        throw std::invalid_argument("Input file is empty.");
    }

    input_file.clear();
    input_file.seekg(0, std::ios::beg);

    auto root = FormHuffmanTree(char_frequency);

    huffman_code_map code_map;
    BuildCodeMap(root, "", code_map);

    WriteHeader(output_file, code_map);
    WriteEncodedText(input_file, output_file, code_map);
    input_file.close();
    output_file.close();

    double entropy = CalculateEntropy(char_frequency, total_chars);
    double avg_code_length =
        CalculateAverageCodeLength(code_map, char_frequency, total_chars);

    codding_efficiency_ = entropy / avg_code_length;
}

void HuffmanArchivator::decompress(
    const std::filesystem::path& input_file_path,
    const std::filesystem::path& output_file_path) {
    auto [input_file, output_file] =
        CheckFiles(input_file_path, output_file_path);

    huffman_code_map code_map;
    ReadHeader(input_file, code_map);

    auto root = FormHuffmanTree(code_map);

    std::string encoded_string;
    ReadEncodedString(input_file, encoded_string);

    auto current = root;
    for (char bit : encoded_string) {
        if (bit == LEFT_BRANCH_KEY) {
            current = current->left;
        } else if (bit == RIGHT_BRANCH_KEY) {
            current = current->right;
        } else {
            throw std::runtime_error("Invalid encoded string.");
        }

        if (!current->left && !current->right) {
            output_file.put(current->data);
            current = root;
        }
    }

    input_file.close();
    output_file.close();
}

std::shared_ptr<HuffmanArchivator::HuffmanNode>
HuffmanArchivator::FormHuffmanTree(
    const std::unordered_map<char, int>& char_frequency) const {
    huffman_priority_queue pq;
    for (auto [symbol, frequency] : char_frequency) {
        pq.push(std::make_shared<HuffmanNode>(symbol, frequency));
    }

    while (pq.size() > 1) {
        auto left = pq.top();
        pq.pop();
        auto right = pq.top();
        pq.pop();

        auto merged =
            std::make_shared<HuffmanNode>('\0', left->freq + right->freq);
        merged->left = left;
        merged->right = right;
        pq.push(merged);
    }

    return pq.top();
}

void HuffmanArchivator::BuildCodeMap(const std::shared_ptr<HuffmanNode>& root,
                                     const std::string& str,
                                     huffman_code_map& out_code_map) const {
    if (!root) return;

    if (!root->left && !root->right) {
        out_code_map[root->data] = str;
    }

    BuildCodeMap(root->left, str + LEFT_BRANCH_KEY, out_code_map);
    BuildCodeMap(root->right, str + RIGHT_BRANCH_KEY, out_code_map);
}

void HuffmanArchivator::WriteHeader(std::ofstream& output_file,
                                    const huffman_code_map& code_map) const {
    size_t uniqiue_chars_count = code_map.size();
    output_file.write(reinterpret_cast<const char*>(&uniqiue_chars_count),
                      sizeof(uniqiue_chars_count));

    for (const auto& [symbol, code] : code_map) {
        output_file.put(symbol);
        size_t code_size = code.size();
        output_file.write(reinterpret_cast<const char*>(&code_size),
                          sizeof(code_size));
        output_file.write(code.c_str(), code_size);
    }
}

void HuffmanArchivator::WriteEncodedText(std::ifstream& input_file,
                                         std::ofstream& output_file,
                                         huffman_code_map& code_map) const {
    char ch;
    std::string encoded_string = "";
    while (input_file.get(ch)) {
        encoded_string += code_map[ch];
    }

    int padding = BIT_PER_SYMBOL - (encoded_string.size() % BIT_PER_SYMBOL);
    for (int i = 0; i < padding; ++i) encoded_string += PADDING_KEY;

    output_file.write(reinterpret_cast<const char*>(&padding), sizeof(padding));

    for (size_t i = 0; i < encoded_string.size(); i += BIT_PER_SYMBOL) {
        std::bitset<BIT_PER_SYMBOL> bits(
            encoded_string.substr(i, BIT_PER_SYMBOL));
        char byte = static_cast<char>(bits.to_ulong());
        output_file.put(byte);
    }
}

double HuffmanArchivator::CalculateEntropy(
    const std::unordered_map<char, int>& char_frequency,
    size_t total_chars) const {
    double entropy = 0.0;
    for (const auto& [symbol, freq] : char_frequency) {
        double probability = static_cast<double>(freq) / total_chars;
        entropy -= probability * std::log2(probability);
    }
    return entropy;
}

double HuffmanArchivator::CalculateAverageCodeLength(
    const huffman_code_map& code_map,
    const std::unordered_map<char, int>& char_frequency,
    size_t total_chars) const {
    double avg_length = 0.0;
    for (const auto& [symbol, code] : code_map) {
        double probability =
            static_cast<double>(char_frequency.at(symbol)) / total_chars;
        avg_length += probability * code.size();
    }
    return avg_length;
}

void HuffmanArchivator::ReadHeader(std::ifstream& input_file,
                                   huffman_code_map& out_code_map) const {
    size_t unique_chars_count;
    input_file.read(reinterpret_cast<char*>(&unique_chars_count),
                    sizeof(unique_chars_count));

    for (size_t i = 0; i < unique_chars_count; ++i) {
        char character;
        input_file.get(character);
        size_t code_size;
        input_file.read(reinterpret_cast<char*>(&code_size), sizeof(code_size));
        std::string code(code_size, '0');
        input_file.read(&code[0], code_size);
        out_code_map[character] = code;
    }
}

std::shared_ptr<HuffmanArchivator::HuffmanNode>
HuffmanArchivator::FormHuffmanTree(const huffman_code_map& code_map) const {
    auto root = std::make_shared<HuffmanNode>();
    for (const auto& [symbol, code] : code_map) {
        auto current = root;
        for (char bit : code) {
            if (bit == LEFT_BRANCH_KEY) {
                if (!current->left)
                    current->left = std::make_shared<HuffmanNode>();
                current = current->left;
            } else if (bit == RIGHT_BRANCH_KEY) {
                if (!current->right)
                    current->right = std::make_shared<HuffmanNode>();
                current = current->right;
            } else {
                throw std::runtime_error("Invalid code");
            }
        }
        current->data = symbol;
    }
    return root;
}

void HuffmanArchivator::ReadEncodedString(
    std::ifstream& input_file, std::string& out_encoded_string) const {
    int padding;
    input_file.read(reinterpret_cast<char*>(&padding), sizeof(padding));

    out_encoded_string = "";
    char byte;
    while (input_file.get(byte)) {
        std::bitset<BIT_PER_SYMBOL> bits(static_cast<unsigned char>(byte));
        out_encoded_string += bits.to_string();
    }

    if (padding > 0)
        out_encoded_string.erase(out_encoded_string.size() - padding, padding);
}
