#include <iostream>
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
    HuffTreeNode(char c, int f) {
        this->ascii_char = c;
        this->freq = f;
        left = right = NULL;
    }
};

struct HuffTreeNodeCompare {
    bool operator()(HuffTreeNode* left, HuffTreeNode* right) {
        return (left->freq > right->freq);
    }
};

void compress(std::ifstream &in_fhandler, std::ofstream &out_fhandler) {
    char c;
    std::unordered_map<char, int> freq_table; 
    while (in_fhandler.peek() != EOF) {
        c = in_fhandler.get();
        freq_table[c]++;
    }
    std::priority_queue<HuffTreeNode*, std::vector<HuffTreeNode*>, HuffTreeNodeCompare> q;    
    for (const auto &keyval: freq_table) {
        q.push(new HuffTreeNode(keyval.first, keyval.second));
    }
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
    
}

int main(int argc, char** argv) {
    if (argc != 3)
        throw std::invalid_argument("Accepts 2 positional arguments - [INPUT_FILE_PATH] [OUTPUT_FILE_PATH]");
    std::ifstream in_fhandler;
    std::ofstream out_fhandler;
    in_fhandler.open(argv[1]);
    out_fhandler.open(argv[2]);
    if (in_fhandler.is_open())
        compress(in_fhandler, out_fhandler);
    in_fhandler.close();
    out_fhandler.close();
    return 0;
}