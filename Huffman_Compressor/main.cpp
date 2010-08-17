#include "HuffmanProcessor.h"

int main(int argc, char* argv[])
{
	HuffmanProcessor huffy("shakespeare.txt");
	huffy.Compress("new_shakespeare.txt");
	return 0;
}