#include "huffman.h"
#include <bitset>
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

// Node deletion
void Huffman::deleteTree(Node* root) {
    if (!root) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

//writing header
void Huffman::writeHeader(ofstream& out, const map<char, unsigned>& freqMap) {
    unsigned uniqueCount = freqMap.size();
    out.write(reinterpret_cast<const char*>(&uniqueCount), sizeof(uniqueCount));
    for (const auto& pair : freqMap) {
        out.put(pair.first);
        out.write(reinterpret_cast<const char*>(&pair.second), sizeof(pair.second));
    }
}

//reading header                --along with padding info
map<char, unsigned> Huffman::readHeader(ifstream& in, unsigned& padding) {
    unsigned uniqueCount;
    in.read(reinterpret_cast<char*>(&uniqueCount), sizeof(uniqueCount));
    map<char, unsigned> freqMap;
    for (unsigned i = 0; i < uniqueCount; ++i) {
        char ch;
        unsigned freq;
        in.get(ch);
        in.read(reinterpret_cast<char*>(&freq), sizeof(freq));
        freqMap[ch] = freq;
    }
    in.read(reinterpret_cast<char*>(&padding), sizeof(padding));
    return freqMap;
}

//compression
void Huffman::compress(const string& inputFile, const string& outputFile) {
    clock_t start = clock();
    ifstream in(inputFile, ios::binary);
    if (!in) {
        cerr << "Error opening input file for compression." << endl;
        return;
    }

    map<char, unsigned> freqMap = countFrequencies(in);
    if (freqMap.empty()) {
        ofstream out(outputFile, ios::binary);
        in.close();
        cout << "Compression ratio: 0 (empty input)" << endl;
        return;
    }

    Node* root = buildTree(freqMap);
    map<char, string> codes;
    generateCodes(root, "", codes);
    ofstream out(outputFile, ios::binary);
    writeHeader(out, freqMap);

    unsigned padding = 0;
    unsigned bitCount = 0;
    char buffer = 0;
    char ch;
    while (in.get(ch)) {
        const string& code = codes[ch];
        for (char bit : code) {
            buffer = (buffer << 1) | (bit == '1' ? 1 : 0);
            if (++bitCount == CHAR_BIT) {
                out.put(buffer);
                buffer = 0;
                bitCount = 0;
            }
        }
    }
    if (bitCount > 0) {
        padding = CHAR_BIT - bitCount;
        buffer <<= padding;
        out.put(buffer);
    }
    out.write(reinterpret_cast<const char*>(&padding), sizeof(padding));

    in.close();
    out.close();
    deleteTree(root);

    clock_t end = clock();
    double timeTaken = double(end - start) / CLOCKS_PER_SEC;
    ifstream orig(inputFile, ios::binary | ios::ate);
    ifstream comp(outputFile, ios::binary | ios::ate);
    double originalSize = orig.tellg();
    double compressedSize = comp.tellg();
    orig.close(); comp.close();

    cout << "Compression time: " << timeTaken << " seconds" << endl;
    cout << "Compression ratio: " << (compressedSize / originalSize) << endl;
}

//decompression
void Huffman::decompress(const string& inputFile, const string& outputFile) {
    clock_t start = clock();
    ifstream in(inputFile, ios::binary);
    if (!in) {
        cerr << "Error opening input file for decompression." << endl;
        return;
    }

    unsigned uniqueCount;
    in.read(reinterpret_cast<char*>(&uniqueCount), sizeof(uniqueCount));
    map<char, unsigned> freqMap;
    for (unsigned i = 0; i < uniqueCount; ++i) {
        char ch;
        unsigned freq;
        in.get(ch);
        in.read(reinterpret_cast<char*>(&freq), sizeof(freq));
        freqMap[ch] = freq;
    }

    if (freqMap.empty()) {
        ofstream out(outputFile, ios::binary);
        in.close();
        cout << "Decompression completed (empty input)." << endl;
        return;
    }

    Node* root = buildTree(freqMap);
    ofstream out(outputFile, ios::binary);

    unsigned dataStart = in.tellg();

    in.seekg(0, ios::end);
    unsigned fileEnd = in.tellg();
    unsigned padding = 0;
    in.seekg(fileEnd - sizeof(padding), ios::beg);
    in.read(reinterpret_cast<char*>(&padding), sizeof(padding));

    unsigned compressedDataSize = fileEnd - sizeof(padding) - dataStart;
    
    in.seekg(dataStart, ios::beg);

    Node* current = root;
    for (unsigned i = 0; i < compressedDataSize; ++i) {
        char ch;
        in.get(ch);
        int bitsToProcess = (i == compressedDataSize - 1) ? (CHAR_BIT - padding) : CHAR_BIT;
        for (int j = CHAR_BIT - 1; j >= CHAR_BIT - bitsToProcess; --j) {
            bool bit = (ch >> j) & 1;
            current = bit ? current->right : current->left;
            if (!current->left && !current->right) {
                out.put(current->data);
                current = root;
            }
        }
    }

    in.close();
    out.close();
    deleteTree(root);

    clock_t end = clock();
    double timeTaken = double(end - start) / CLOCKS_PER_SEC;
    cout << "Decompression time: " << timeTaken << " seconds" << endl;
}

