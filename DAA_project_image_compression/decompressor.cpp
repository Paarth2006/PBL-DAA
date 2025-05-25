#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <bitset>
#include <sstream>
using namespace std;

struct Node {
    int value;
    Node* left;
    Node* right;
    Node(int v) : value(v), left(nullptr), right(nullptr) {}
    Node() : value(-1), left(nullptr), right(nullptr) {}
};

void insertCode(Node* root, const string& code, int value) {
    Node* curr = root;
    for (char c : code) {
        if (c == '0') {
            if (!curr->left) curr->left = new Node();
            curr = curr->left;
        } else {
            if (!curr->right) curr->right = new Node();
            curr = curr->right;
        }
    }
    curr->value = value;
}

void deleteTree(Node* root) {
    if (!root) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: decompressor.exe <input.huff> <output_pixels.bin>\n";
        return 1;
    }

    string input_path = argv[1];
    string output_path = argv[2];

    ifstream fin(input_path, ios::binary);
    if (!fin) {
        cerr << "Error opening input file.\n";
        return 1;
    }

    // Read pixel count
    std::string firstLine;
    std::getline(fin, firstLine);
    int pixel_count = std::stoi(firstLine);

    // Read code table
    unordered_map<string, int> codeToValue;
    string line;
    while (getline(fin, line)) {
        if (line == "END") break;
        istringstream iss(line);
        int value;
        string code;
        if (!(iss >> value >> code)) {
            cerr << "Error reading code table.\n";
            return 1;
        }
        codeToValue[code] = value;
    }

    // Build Huffman tree
    Node* root = new Node();
    for (const auto& p : codeToValue) {
        insertCode(root, p.first, p.second);
    }

    // Read compressed bitstream
    vector<unsigned char> compressed_bytes((istreambuf_iterator<char>(fin)), {});
    fin.close();

    // Decode bitstream
    vector<unsigned char> pixel_stream;
    string bits;
    for (unsigned char byte : compressed_bytes) {
        for (int i = 7; i >= 0; --i) {
            bits += ((byte >> i) & 1) ? '1' : '0';
        }
    }
    size_t idx = 0;
    while (idx < bits.size() && pixel_stream.size() < pixel_count) {
        Node* node = root;
        while (node->left || node->right) {
            if (idx >= bits.size()) break;
            if (bits[idx++] == '0') node = node->left;
            else node = node->right;
        }
        if (node->value != -1)
            pixel_stream.push_back(static_cast<unsigned char>(node->value));
        // Stop after pixel_count pixels
    }

    ofstream fout(output_path, ios::binary);
    fout.write(reinterpret_cast<const char*>(pixel_stream.data()), pixel_stream.size());
    fout.close();

    deleteTree(root);
    cout << "Decompression complete. Output written to " << output_path << endl;
    return 0;
}