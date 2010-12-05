#include "HuffmanProcessor.h"

void HuffmanProcessor::WriteBit(bool bit)
{
	bitBuffer |= (bit ?  1 : 0) << numBits;
    ++numBits;

	if(numBits == 8)
	{
        output.put(bitBuffer);	
	    bitBuffer = 0;
	    numBits = 0;
	}
}

void HuffmanProcessor::FlushBits()
{
	if(numBits)
	{
		output.put(bitBuffer);
	}
	bitBuffer = 0;
	numBits = 0;
}

void HuffmanProcessor::WriteHeader()
{
}

void HuffmanProcessor::AccumulateBytes(Node* root, std::vector<bool>& accumulator)
{
    if(root->leftChild == NULL && root->rightChild == NULL)
    {
        encoding[root->data] = accumulator;
    } else {
        std::vector<bool> left = accumulator;
        std::vector<bool> right = accumulator;

        if(root->leftChild) 
        {
            left.push_back(0);
            AccumulateBytes(root->leftChild, left);
        }

        if(root->rightChild)
        {
            right.push_back(1);
	        AccumulateBytes(root->rightChild, right);
        }
    }
}

void HuffmanProcessor::PrintTreeInOrder(Node* root)
{
    // Base case
    if(root->leftChild == NULL && root->rightChild == NULL)
    {
        output << "Byte: " << root->data << " | " << "Frequency: " << root->frequency << "\n";
    } else {
        PrintTreeInOrder(root->leftChild);
        PrintTreeInOrder(root->rightChild);
    }
}

void HuffmanProcessor::Compress(char* output_filename)
{
    output_file = output_filename;
    // Start all frequencies out at zero.
    for(int i = 0; i < 255; ++i)
    {
        frequencies.insert(pair<unsigned char, int>((unsigned char)i, 0));
    }

    // Build histogram
    for(int curr_byte = 0; curr_byte < size; ++curr_byte)
    {
        ++frequencies[buffer[curr_byte]];
    }

    
    // Print the frequency table to a file for debugging
    ofstream debug_output = ofstream("frequency_table.txt");
    for(std::map<unsigned char, int>::iterator itr = frequencies.begin(); itr != frequencies.end(); ++itr)
    {
        debug_output << "Byte: " << (*itr).first << " | " << "Frequency: " << (*itr).second << "\n";
    }

    // For each nonzero frequency byte, create a new leaf node in the tree.
    for(std::map<unsigned char, int>::iterator itr = frequencies.begin(); itr != frequencies.end(); ++itr)
    {
        if((*itr).second)
        {
            nodes.push(new Node((*itr).first, (*itr).second));
        }
    }

    while(nodes.size() > 1)
    {
        Node* lowest = new Node();
        Node* secondLowest = new Node();
        
        // Find the two least frequent nodes.
        lowest = nodes.top();
        nodes.pop();

        secondLowest = nodes.top();
        nodes.pop();

        // Make a new node the parent of the two we just removed where its
        // frequency is the sum of the bottom two.
        Node* newParent = new Node();
        newParent->data = NULL;
        newParent->SetChildren(lowest, secondLowest);
        newParent->frequency = lowest->frequency + secondLowest->frequency;
        nodes.push(newParent);
    }

    root = nodes.top();

    output = ofstream(output_filename);

    //PrintTreeInOrder(root);

    std::vector<bool> none;
    AccumulateBytes(root, none);

    output = ofstream((output_file ? output_file : filename), ios::binary);


    short begin = 0x4D4D;

    output.write((char*)&begin, sizeof(short));
    WriteTree(root);

    short end = 0x0FFF;
    //output.write((char*)&end, sizeof(short));
    
    //WriteToFile();

    output.close();
}

void HuffmanProcessor::WriteToFile()
{
    
    int counter = 0;
    std::vector<bool> code;
    for(int curr_byte = 0; curr_byte < size; ++curr_byte)
    {
        counter += 1;
        code = encoding[buffer[curr_byte]];
        if(curr_byte == size - 1)
        {
            // Write extra bits to fill out a byte if we're on the last byte of the file
            for(vector<bool>::iterator itr = code.begin(); itr != code.end(); ++itr)
            {
                WriteBit((*itr));
            }

            int remaining_space = 8 - numBits;
            for(int i = 0; i < remaining_space; ++i)
            {
                WriteBit(false);
            }
        } else {

            for(vector<bool>::iterator itr = code.begin(); itr != code.end(); ++itr)
            {
                WriteBit((*itr));
            }
        }
    }

}

/**
 * The Tree begins with a single short: 0x4D4D
 *
 * Nodes are laid out in the following format:
 * Node Header (short) 2 bytes
 *    Non-Null Node: 0x0BAB
 *    Null Node: 0x1ABAB
 * Node Data (unsigned char) 1 byte
 * Node Frequency (int) 4 bytes
 *
 * The tree ends with a single short: 0x0FFF
 */
void HuffmanProcessor::WriteTree(Node* root)
{
    short non_null_header = 0x0BAB;
    if(root->leftChild == NULL && root->rightChild == NULL)
    {
        // Node header
        output.write((char*)&non_null_header, sizeof(non_null_header));

        // Data
        output.write((char*)&root->data, sizeof(root->data));
        output.write((char*)&root->frequency, sizeof(root->frequency));

        // Sentinel children
        short null_header = 0x1ABAB;
        output.write((char*)&null_header, sizeof(short));
        output.write((char*)&null_header, sizeof(short));
        
        return;
    }

    // Node header and data
    output.write((char*)&non_null_header, sizeof(non_null_header));
    output.write((char*)&root->data, sizeof(root->data));
    output.write((char*)&root->frequency, sizeof(root->frequency));

    WriteTree(root->leftChild);
    WriteTree(root->rightChild);
}

void HuffmanProcessor::Decompress(char* input_filename)
{
    input = ifstream(input_filename, ios::binary);
    root = new Node();

    ReadTree(root);
}

void HuffmanProcessor::ReadTree(Node*& root)
{
    streampos len;
    input.seekg(0, ios::end);
    len = input.tellg();
    input.seekg(0, ios::beg);

    short start_id;
    input.read((char*)&start_id, 2);

    RecursiveReadTree(root, len);
}

void HuffmanProcessor::RecursiveReadTree(Node*& root, streampos end_of_tree)
{
    if(input.tellg() == end_of_tree)
    {
        return;
    }


    unsigned char data_token;
    int frequency_token;
    short header;

    input.read((char*)&header, 2);

    // Null node
    if(header == 0x1ABAB)
    {
        return;
    }

    input.read((char*)&data_token, 1);
    input.read((char*)&frequency_token, 4);

    root = new Node();
    root->data = data_token;
    root->frequency = frequency_token;

    RecursiveReadTree(root->leftChild, end_of_tree);
    RecursiveReadTree(root->rightChild, end_of_tree);
}