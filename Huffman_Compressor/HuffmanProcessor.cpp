#include "HuffmanProcessor.h"

void HuffmanProcessor::WriteBit(bool bit)
{
	bitBuffer |= bit << numBits;
	if(++numBits < 8)
	{
		return;
	}

	output.put(bitBuffer);
	bitBuffer = 0;
	numBits = 0;
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

void HuffmanProcessor::AccumulateBytes(Node* root)
{
	if(!root->leftChild && !root->rightChild)
	{
		return;
	}

	if(root->leftChild)
	{
		WriteBit(0);
		AccumulateBytes(root->leftChild);
	}

	if(root->rightChild)
	{
		WriteBit(1);
		AccumulateBytes(root->rightChild);
	}

}

void HuffmanProcessor::Compress(const char* output_filename)
{
	// Build the frequency table
	for(int i = 0; i < size; i++)
	{
		frequencyTable[buffer[i]]++;
	}

	for(int i = 0; i < 256; i++)
	{
		if(frequencyTable[i])
		{
			nodes.push_back(new Node((unsigned char)i, frequencyTable[i]));
		}
	}

	sort(nodes.begin(), nodes.end(), NodeComparator);

	while(nodes.size() > 1) {
		Node* lowest = new Node();
		Node* secondLowest = new Node();
		lowest = *(nodes.end() - 1);
		secondLowest = *(nodes.end() - 2);
		nodes.erase(nodes.end() - 2, nodes.end());
		Node* newParent = new Node();
		newParent->SetChildren(lowest, secondLowest);
		newParent->frequency = lowest->frequency + secondLowest->frequency;
		nodes.push_back(newParent);
	}

	output = ofstream((output_filename ? output_filename : filename), ofstream::binary);

	AccumulateBytes(nodes.front());
	FlushBits();
}