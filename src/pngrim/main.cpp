
/* This code is released into the public domain. */

#include <string.h>
#include <stdio.h>
#include "ImagePNG.h"
#include "pngrim.h"

void processImage(Image& img, bool fast)
{
	if(fast)
		pngrimFast(img);
	else
		pngrimAccurate(img);
}

void processFile(const char *fn, bool fast)
{
	Image img;
	if(!img.readPNG(fn))
	{
		printf("File not processed: %s\n", fn);
		return;
	}

	printf("Processing %s ... ", fn);
	fflush(stdout);
	processImage(img, fast);
	printf("saving ... ");
	fflush(stdout);

	if(img.writePNG(fn))
		printf("OK\n");
	else
		printf("Failed to write!\n");
}


int main(int argc, char **argv)
{
	if(argc <= 1)
	{
		printf("Usage: ./pngrim [--fast] file1.png [fileX.png ...]\n");
		printf("Warning: Modifies files in place!\n");
		return 2;
	}

	int begin = 1;
	bool fast = false;
	if(!strcmp(argv[1], "--fast"))
	{
		begin = 2;
		fast = true;
	}

	for(int i = begin; i < argc; ++i)
		processFile(argv[i], fast);

	return 0;
}

