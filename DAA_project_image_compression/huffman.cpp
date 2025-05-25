#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <string>
#include <bitset>
using namespace std;

struct Node {
    int value, freq;
    Node *left, *right;
    Node(int v, int f) : value(v), freq(f), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(Node* a, Node* b) { return a->freq > b->freq; }
};

void buildCodeTable(Node* root, string code, unordered_map<int, string>& codeTable) {
    if (!root) return;
    if (!root->left && !root->right) { codeTable[root->value] = code; return; }
    buildCodeTable(root->left, code + "0", codeTable);
    buildCodeTable(root->right, code + "1", codeTable);
}

void deleteTree(Node* root) {
    if (!root) return;
    deleteTree(root->left); deleteTree(root->right); delete root;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: huffman.exe <input_pixels.bin> <output.huff>\n";
        return 1;
    }
    string input_bin = argv[1], output_huff = argv[2];

    // Read pixel stream
    ifstream fin(input_bin, ios::binary);
    vector<unsigned char> pixels((istreambuf_iterator<char>(fin)), {});
    fin.close();

    // Build frequency table
    unordered_map<int, int> freq;
    for (unsigned char pix : pixels) freq[pix]++;

    // Build Huffman tree
    priority_queue<Node*, vector<Node*>, Compare> pq;
    for (auto& p : freq) pq.push(new Node(p.first, p.second));
    while (pq.size() > 1) {
        Node* l = pq.top(); pq.pop();
        Node* r = pq.top(); pq.pop();
        Node* parent = new Node(-1, l->freq + r->freq);
        parent->left = l; parent->right = r;
        pq.push(parent);
    }
    Node* root = pq.top();

    // Build code table
    unordered_map<int, string> codeTable;
    buildCodeTable(root, "", codeTable);

    // Write code table
    ofstream fout(output_huff, ios::binary);
    fout << pixels.size() << "\n"; // Write pixel count as the first line
    for (auto& entry : codeTable)
        fout << entry.first << " " << entry.second << "\n";
    fout << "END\n";

    // Encode pixel stream
    string bitstring;
    for (unsigned char pix : pixels) bitstring += codeTable[pix];
    for (size_t i = 0; i < bitstring.size(); i += 8) {
        string byteStr = bitstring.substr(i, 8);
        while (byteStr.size() < 8) byteStr += '0';
        bitset<8> b(byteStr);
        unsigned char c = static_cast<unsigned char>(b.to_ulong());
        fout.write(reinterpret_cast<const char*>(&c), 1);
    }
    fout.close();
    deleteTree(root);
    cout << "Compression complete.\n";
    return 0;
}