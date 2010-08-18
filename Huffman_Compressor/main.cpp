#include "HuffmanProcessor.h"

int main(int argc, char* argv[])
{
	HuffmanProcessor huffy("blinn_fail.jpg");
	huffy.Compress("new_blinn_fail.jpg");
	return 0;
}