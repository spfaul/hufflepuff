#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <queue>
#include <unordered_map>
#include <queue>

struct HuffTreeNode {
    char ascii_char;
    int freq;
    HuffTreeNode *left, *right;
    HuffTreeNode(char c, int f): ascii_char(c), freq(f) {
        left = right = NULL;
    }
};

struct HuffTreeNodeCompare {
    bool operator()(HuffTreeNode* left, HuffTreeNode* right) {
        return (left->freq > right->freq);
    }
};

struct BitWriter {
    std::ofstream& out_fhandler;
    unsigned char output_byte = 0;
    unsigned int bit_count = 0;

    BitWriter(std::ofstream& out_f) : out_fhandler(out_f) {}

    void write_bit(unsigned char bit) {
        std::cout << (int) bit << std::endl;
        output_byte <<= 1;
        if (bit) output_byte |= 1;
        bit_count++;
        if (bit_count == 8) {
            out_fhandler << output_byte;
            output_byte = bit_count = 0;
        }
    }

    void write_char(char byte) {
        for (int bit=7; bit>=0; bit--) {
            write_bit((byte >> bit) & 0x01);
        }
    }

    void flush() {
        while (bit_count > 0) write_bit(0);
    }
};


struct BitReader {
    std::ifstream& in_fhandler;
    char input_byte = 0;
    unsigned int bit_count = 0;

    BitReader(std::ifstream& in_f) : in_fhandler(in_f) {}

    unsigned int read_bit() {
        // def could be refactored more simply
        if (bit_count == 8) // last bit has been read, reset bit_count
            bit_count = 0;
        // get next char (8 bits)
        if (bit_count == 0) {
            input_byte = in_fhandler.get();
            if (input_byte == EOF)
                return 2;
        } 
        unsigned int b = (input_byte >> (7-bit_count)) & 0x01;
        bit_count++;
        return b;
    }
};

void write_hufftree(BitWriter *bw, HuffTreeNode* root) {
    if (root->ascii_char != '\0') {
        bw->write_bit(1); // leaf        
        bw->write_char(root->ascii_char);
        return;
    }
    bw->write_bit(0); // intermediate node
    write_hufftree(bw, root->left);
    write_hufftree(bw, root->right);
}

void populate_tree_path_map(std::unordered_map<char, std::string>* map, HuffTreeNode* root, std::string path) {
    if (root->ascii_char != '\0') {
        (*map)[root->ascii_char] = path;
        return;
    }
    populate_tree_path_map(map, root->left, path+'0');
    populate_tree_path_map(map, root->right, path+'1');
}

void write_textdata(BitWriter* bw, std::unordered_map<char, std::string>* tree_map, std::ifstream& in_stream) {
    in_stream.seekg(0);
    while (in_stream.peek() != EOF) {
        std::string path = (*tree_map)[in_stream.get()];
        for (char c: path)
            bw->write_bit(c - '0');
    }
}


void decompress(std::ifstream &in_fhandler, std::ofstream &out_fhandler) {
    // construct huff tree
    BitReader* br = new BitReader(in_fhandler);
    unsigned int bit;
    while (bit != 2) {
        bit = br->read_bit();
        std::cout << bit << std::endl;
    }
}

void compress(std::ifstream &in_fhandler, std::ofstream &out_fhandler) {
    // construct freq table
    std::unordered_map<char, int> freq_table; 
    in_fhandler.seekg(0);
    while (in_fhandler.peek() != EOF) {
        char c = in_fhandler.get();
        freq_table[c]++;
    }
    // populate min heap
    std::priority_queue<HuffTreeNode*, std::vector<HuffTreeNode*>, HuffTreeNodeCompare> q;    
    for (const auto &keyval: freq_table) {
        q.push(new HuffTreeNode(keyval.first, keyval.second));
    }
    // construct huffman tree
    while (q.size() > 1) {
        HuffTreeNode* least_freq_node = q.top();
        q.pop();
        HuffTreeNode* next_least_freq_node = q.top();
        q.pop();
        HuffTreeNode* new_node = new HuffTreeNode('\0', least_freq_node->freq + next_least_freq_node->freq);
        new_node->left = least_freq_node;
        new_node->right = next_least_freq_node;
        q.push(new_node);
    }
    // construct char to tree path map
    std::unordered_map<char, std::string>* tree_map = new std::unordered_map<char, std::string>();
    populate_tree_path_map(tree_map, q.top(), "");
    // write tree to file
    BitWriter *bw = new BitWriter(out_fhandler);
    write_hufftree(bw, q.top());
    write_textdata(bw, tree_map, in_fhandler);
    bw->flush();
}

int main(int argc, char** argv) {
    if (argc != 4)
        throw std::invalid_argument("Accepts 3 positional arguments - [cp OR dcp] [INPUT_FILE_PATH] [OUTPUT_FILE_PATH]");
    std::ifstream in_fhandler;
    std::ofstream out_fhandler;
    in_fhandler.open(argv[2]);
    out_fhandler.open(argv[3]);
    if (!in_fhandler.is_open() || !out_fhandler.is_open()) {
        throw std::runtime_error("Error opening input/output files: check file permissions and path");        
    }
    std::string procedure = argv[1];
    if (procedure == "cp")
        compress(in_fhandler, out_fhandler);
    else if (procedure == "dcp")
        decompress(in_fhandler, out_fhandler);
    else
        throw std::invalid_argument("Invalid 1st argument: accepts \"cp\" (compress) or \"dcp\" (decompress)");
        
    in_fhandler.close();
    out_fhandler.close();
    return 0;
}