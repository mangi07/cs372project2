/*********************************************************
Author: Benjamin R. Olson
Date: 2-25-17
Filename: connectionControllerTests.c

Description: OSU Networking CS 372, Project 2
  connectionController.c unit tests
***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "connectionController.h"

int main() { 


	// long long _charToLongLong ( const char* num_str )
	/*
	char* num_str = "9223372036854775807";
	long long num = _charToLongLong ( num_str );
	printf( "num: %lld\n", num );
	*/
	
	// int startup( const char* PORT )
	char* port = "3490";
	startup(port);
	
	
	return 0;

}

