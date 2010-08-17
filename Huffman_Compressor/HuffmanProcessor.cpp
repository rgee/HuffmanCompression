#include "HuffmanProcessor.h"

int HuffmanProcessor::MaxTreeDepth(Node* root) const
{
	int rightDepth, leftDepth;
	if(root->rightChild)
	{
		rightDepth = MaxTreeDepth(root->rightChild);
	}
	if(root->leftChild)
	{
		leftDepth = MaxTreeDepth(root->leftChild);
	}

	return std::max(rightDepth, leftDepth);
}

void HuffmanProcessor::WriteBytes(Node* root) const
{
	
}

void HuffmanProcessor::Compress(const char* output_filename)
{
	map<char, int>::iterator it;
	for(int i = 0; i < size; i++)
	{
		it = frequencies.find(buffer[i]);
		if(it == frequencies.end())
		{
			frequencies.insert(pair<char, int>(buffer[i], 1));
		} 
		else
		{
			it->second = it->second + 1;
		}
	}

	for(it = frequencies.begin(); it != frequencies.end(); it++)
	{
		if((*it).second > 0)
		{
			nodes.push_back(new Node((*it).first, (*it).second));
		}

		/* Sort on frequency */
		sort(nodes.begin(), nodes.end(), NodeComparator);
	}
	while(nodes.size() > 1)
	{
		Node* newParent = new Node();
		newParent->SetChildren((*(nodes.end() - 1)), (*(nodes.end() - 2)));
		nodes.push_back(newParent);

		nodes.erase(nodes.end() - 2, nodes.end());
	}

	Node* root = nodes.front();
	int maxDepth = MaxTreeDepth(root);
	bitBuffer = new char[maxDepth];

	

	output = ofstream((output_filename ? output_filename : filename), ofstream::binary);
	output.write(buffer, size);
}