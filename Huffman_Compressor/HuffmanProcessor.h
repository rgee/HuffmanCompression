#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <set>
#include <queue>
#include <limits.h>
using namespace std;


class HuffmanProcessor
{
public:
	HuffmanProcessor(const char* filename)
		:filename(filename)
	{
		ifstream file(filename, ifstream::binary);
		if(!file.is_open())
		{
			throw std::runtime_error("Could not open file.");
		}


		file.seekg(0, ifstream::end);

		// Read the entire file into memory
		size = (int)file.tellg();

		buffer = new unsigned char[size];
		file.seekg(0);
		file.read((char*)buffer, size);
		file.close();

		frequencyTable = (int*)	calloc(256, sizeof(int));

		bitBuffer = 0;
		numBits = 0;
	}

	~HuffmanProcessor() 
	{
		delete[] buffer;
		output.close();
	}

	void Compress(char* output_filename = NULL);
private:
    std::map<unsigned char, int> frequencies;
    std::map<unsigned char, std::vector<bool>> encoding;

	const char* filename;
    const char* output_file;
	int size;
	ofstream output;
	unsigned char* buffer;

	// There are only 256 possible values for unsigned
	// chars, so we just index by that value and store the count
	// at its corresponding location.
	int* frequencyTable;


	// Private Class for intneral tree data structure
	// Leaves have null children
	class Node
	{
	public:
		unsigned char data;
		int frequency;
		Node* leftChild;
		Node* rightChild;

		Node(unsigned char data, int frequency)
			: data(data),
			  frequency(frequency),
			  leftChild(NULL),
			  rightChild(NULL)
		{}

		Node()
			: frequency(0),
			  leftChild(NULL),
			  rightChild(NULL)
		{}

		Node& operator=(const Node& rhs)
		{
			if(this != &rhs)
			{
				data = rhs.data;
				frequency = rhs.frequency;
				leftChild = rhs.leftChild;
				rightChild = rhs.rightChild;
			}
			return *this;
		}

		friend ostream& operator<<(ostream& stream, Node& node);

		void SetChildren(Node* left, Node* right)
		{
			leftChild = left;
			rightChild = right;
		}


	};

	// Node sorting functor
	struct functorClass {
		bool operator() (Node* i, Node* j) { return i->frequency > j->frequency; }
		bool operator() (Node i, Node j) { return i.frequency > j.frequency; }
	} NodeComparator;

	// The intermediate representation of the nodes in the tree
	// before processing / arrangement / sorting
	//vector<Node*> nodes;

    std::priority_queue<Node*, vector<Node*>, functorClass> nodes;

	// The buffer to hold bits until we have enough to 
	// make a full byte.
	unsigned char bitBuffer;

	// The number of unwritten bits in the buffer
	int numBits;

	// Accumulate individual bytes back to the file in the following pattern:
	// Traversing the frequency tree in depth-first order, we write paths from
	// the root as strings of bits where 0 represents a left branch and 1
	// represents a right branch.
	void AccumulateBytes(Node* root, std::vector<bool>& accumulator);

    unsigned char TraversalHelper(Node* root);

	// Write bytes in the appropriate size to the file
	void WriteBit(bool bit);

	// Flush any remaining bits after writing to the file
	void FlushBits();

	// Write the tree as a file header
	void WriteHeader();

    // The root of the binary tree of frequencies
    Node* root;

    void PrintTreeInOrder(Node* root);

    void WriteToFile();
};

