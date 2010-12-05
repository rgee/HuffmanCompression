#include "HuffmanProcessor.h"

int main(int argc, char* argv[])
{
	HuffmanProcessor huffy("test.txt");
	huffy.Compress("new_test.txt");
    huffy.Decompress("new_test.txt");
	return 0;
}