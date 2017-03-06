/*********************************************************
Author: Benjamin R. Olson
Date: 2-25-17
Filename: fileManagerTests.c

Description: OSU Networking CS 372, Project 2
  fileManager.c unit tests
***********************************************************/

#include "fileManager.h"


int main() { 

	char* dirListing = listDir(".");
	printf( "%s\n", dirListing );
	free( dirListing );
	
	return 0;

}