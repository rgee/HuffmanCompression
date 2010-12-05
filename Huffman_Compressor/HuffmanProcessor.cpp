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
    WriteToFile();

}

void HuffmanProcessor::WriteToFile()
{
    output = ofstream((output_file ? output_file : filename), ofstream::binary);
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