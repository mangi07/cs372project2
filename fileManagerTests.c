/*********************************************************
Author: Benjamin R. Olson
Date: 2-25-17
Filename: fileManagerTests.c

Description: OSU Networking CS 372, Project 2
  fileManager.c unit tests
***********************************************************/

#include "fileManager.h"
#include <stdio.h>
#include <stdlib.h>


int main() { 

	char* dirListing = listDir(".");
	printf( "%s", dirListing );
	free( dirListing ); 
	return 0;

}