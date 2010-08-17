#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <set>
#include <limits.h>
using namespace std;

/* Bit operator macros */

/* Generate a bit mask to access bit b */
#define BITMASK(b) (1 << ( (b) % CHAR_BIT))

#define BITSLOT(b) ( (b) / CHAR_BIT)

/* Set the bit at position b of a to 1 */
#define BITSET(a, b) ( (a)[BITSLOT(b)] |= BITMASK(b) )

/* Set the bit at position b of a to 0 */
#define BITCLEAR(a, b) ( (a)[BITSLOT(b)] &= ~BITMASK(b) )


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
		/* Read the entire file into memory */
		/* Maybe we should perform some kind of staggered loading
		   and processing if the file is gargantuan. Maybe 32mb chunks
		   or something */
		size = (int)file.tellg();

		buffer = new char[size];
		file.seekg(0);
		file.read(buffer, size);
		file.close();
	}

	~HuffmanProcessor() 
	{
		delete[] buffer;
		delete[] bitBuffer;
	}

	void Compress(const char* output_filename = NULL);
private:
	const char* filename;
	int size;
	ofstream output;
	char* buffer;

	/* A map that stores the frequency of each byte pattern
	   in the file */
	map<char, int> frequencies;


	/* Private Class for intneral tree data structure */

	class Node
	{
	public:
		char data;
		int frequency;
		Node* leftChild;
		Node* rightChild;

		Node(char data, int frequency)
			: data(data),
			  frequency(frequency),
			  leftChild(NULL),
			  rightChild(NULL)
		{}
		Node(char data, int frequency, Node* leftChild, Node* rightChild)
			: data(data),
			  frequency(frequency),
			  leftChild(leftChild),
			  rightChild(rightChild)
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

	/* Node sorting functor */
	struct functorClass {
		bool operator() (Node* i, Node* j) { return i->frequency < j->frequency; }
		bool operator() (Node i, Node j) { return i.frequency < j.frequency; }
	} NodeComparator;

	/* The intermediate representation of the nodes in the tree
	   before processing / arrangement / sorting */
	vector<Node*> nodes;

	/* Collection of visited nodes for depth-first traversal */
	set<Node*> visited;

	char* bitBuffer;

	/* Returns the max depth of the tree */
	int MaxTreeDepth(Node* root) const;

	/* Write individual bytes back to the file in the following pattern:
	   Traversing the frequency tree in depth-first order, we write paths from
	   the root as strings of bits where 0 represents a left branch and 1
	   represents a right branch. */
	void WriteBytes(Node* root) const;
};

