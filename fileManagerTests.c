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

	printf( "\n***TEST listDir\n\n" );
	char* dirListing = listDir(".");
	printf( "%s", dirListing );
	// free??

	printf( "\n***TEST loadFile\n\n" );
	char* fileContents = loadFile( "testdir/testfile" );
	printf( "%s", fileContents );
	
	printf( "\n***TEST loadFile 2\n\n" );
	char* fileContents2 = loadFile( "testdir/bible.txt" );
	printf( "%s", fileContents2 );
	
	return 0;

}