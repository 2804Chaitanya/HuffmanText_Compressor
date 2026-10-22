#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <iostream>
#include <fstream>
#include <queue>
#include <map>
#include <string>
#include <ctime>
#include <vector>
using namespace std;

struct Node {
    char data;
    unsigned freq;
    Node *left, *right;
    Node(char data, unsigned freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(Node* left, Node* right) {
        return left->freq > right->freq;
    }
};

class Huffman {
public:
    static void compress(const string& inputFile, const string& outputFile);
    static void decompress(const string& inputFile, const string& outputFile);
private:
    static map<char, unsigned> countFrequencies(ifstream& in);
    static Node* buildTree(const map<char, unsigned>& freqMap);
    static void generateCodes(Node* root, const string& code, map<char, string>& codes);
    static void deleteTree(Node* root);
    static void writeHeader(ofstream& out, const map<char, unsigned>& freqMap);
    static map<char, unsigned> readHeader(ifstream& in, unsigned& padding);
};

#endif