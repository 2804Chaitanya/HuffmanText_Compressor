#include "huffman.h"
#include <iostream>
using namespace std;

int main() {
    string inputFile, compressedFile, decompressedFile;
    int choice;

    cout << "1. Compress\n2. Decompress\nEnter choice: ";
    cin >> choice;
    cin.ignore();

    if (choice == 1) {
        cout << "Enter input file name: ";
        getline(cin, inputFile);
        cout << "Enter compressed file name: ";
        getline(cin, compressedFile);
        Huffman::compress(inputFile, compressedFile);
    } else if (choice == 2) {
        cout << "Enter compressed file name: ";
        getline(cin, compressedFile);
        cout << "Enter decompressed file name: ";
        getline(cin, decompressedFile);
        Huffman::decompress(compressedFile, decompressedFile);
    } else {
        cerr << "Invalid choice." << endl;
    }

    return 0;
}