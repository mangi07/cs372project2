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

	// no error expected
	printf( "\n***TEST loadFile with **NO** error\n\n" );
	int error1 = 0;
	char* fileContents1 = loadFile( "testdir/testfile", &error1 );
	printf( "***error: %d\n", error1 );
	printf( "%s", fileContents1 );
	
	// error expected
	printf( "\n***TEST loadFile **with error**\n\n" );
	int error2 = 0;
	char* fileContents2 = loadFile( "testdir/blahblah.txt", &error2 );
	printf( "***error: %d\n", error2 );
	printf( "%s", fileContents2 );
	
	/*
	printf( "\n***TEST loadFile 2\n\n" );
	char* fileContents2 = loadFile( "testdir/bible.txt", &error);
	printf( "%s", fileContents2 );
	*/
	
	return 0;

}