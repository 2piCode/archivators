#include "huffman_archivator.h"

#include <bitset>
#include <exception>
#include <iostream>

void HuffmanArchivator::compress(
    const std::filesystem::path& input_file_path,
    const std::filesystem::path& output_file_path) {
    auto [input_file, output_file] =
        CheckFiles(input_file_path, output_file_path);

    std::unordered_map<char, int> char_frequency;
    char ch;
    while (input_file.get(ch)) {
        char_frequency[ch]++;
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

FileStreams HuffmanArchivator::CheckFiles(
    const std::filesystem::path& input_file,
    const std::filesystem::path& output_file) {
    return FileStreams(input_file, output_file);
}

std::shared_ptr<HuffmanArchivator::HuffmanNode>
HuffmanArchivator::FormHuffmanTree(
    const std::unordered_map<char, int>& char_frequency) {
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
                                     huffman_code_map& out_code_map) {
    if (!root) return;

    if (!root->left && !root->right) {
        out_code_map[root->data] = str;
    }

    BuildCodeMap(root->left, str + LEFT_BRANCH_KEY, out_code_map);
    BuildCodeMap(root->right, str + RIGHT_BRANCH_KEY, out_code_map);
}

void HuffmanArchivator::WriteHeader(std::ofstream& output_file,
                                    const huffman_code_map& code_map) {
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
                                         huffman_code_map& code_map) {
    char ch;
    std::string encoded_string = "";
    while (input_file.get(ch)) {
        encoded_string += code_map[ch];
    }

    int padding = 8 - (encoded_string.size() % 8);
    for (int i = 0; i < padding; ++i) encoded_string += PADDING_KEY;

    output_file.write(reinterpret_cast<const char*>(&padding), sizeof(padding));

    for (size_t i = 0; i < encoded_string.size(); i += 8) {
        std::bitset<8> bits(encoded_string.substr(i, 8));
        char byte = static_cast<char>(bits.to_ulong());
        output_file.put(byte);
    }
}

void HuffmanArchivator::ReadHeader(std::ifstream& input_file,
                                   huffman_code_map& out_code_map) {
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
HuffmanArchivator::FormHuffmanTree(const huffman_code_map& code_map) {
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

void HuffmanArchivator::ReadEncodedString(std::ifstream& input_file,
                                          std::string& out_encoded_string) {
    int padding;
    input_file.read(reinterpret_cast<char*>(&padding), sizeof(padding));

    out_encoded_string = "";
    char byte;
    while (input_file.get(byte)) {
        std::bitset<8> bits(static_cast<unsigned char>(byte));
        out_encoded_string += bits.to_string();
    }

    if (padding > 0)
        out_encoded_string.erase(out_encoded_string.size() - padding, padding);
}
