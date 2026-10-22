#include "huffman.h"
#include <climits>
using namespace std;

// Counts how many times each character appears in the input file (frequency)
map<char, unsigned> Huffman::countFrequencies(ifstream& in) {
    map<char, unsigned> freqMap;
    char ch;
    while (in.get(ch)) {
        freqMap[ch]++;
    }
    in.clear();
    in.seekg(0);
    return freqMap;
}

// Constructing Huffman tree using frequency map
Node* Huffman::buildTree(const map<char, unsigned>& freqMap) {
    priority_queue<Node*, vector<Node*>, Compare> pq;
    for (const auto& pair : freqMap) {
        pq.push(new Node(pair.first, pair.second));
    }
    while (pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        Node* parent = new Node('\0', left->freq + right->freq);
        parent->left = left;
        parent->right = right;
        pq.push(parent);
    }
    return pq.empty() ? nullptr : pq.top();
}

// Node deletion
void Huffman::deleteTree(Node* root) {
    if (!root) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

// binary code generation for each character
void Huffman::generateCodes(Node* root, const string& code, map<char, string>& codes) {
    if (!root) return;
    if (!root->left && !root->right) {
        codes[root->data] = code;
        return;
    }
    generateCodes(root->left, code + "0", codes);
    generateCodes(root->right, code + "1", codes);
}
