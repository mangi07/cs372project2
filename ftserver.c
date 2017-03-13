
/*********************************************************
Author: Benjamin R. Olson
Date: 2-25-17
Filename: ftserver.c

Description: OSU Networking CS 372, Project 2
  simple FTP server

Usage: ftpserver <port number>
  Once running, receives and responds to requests:
  
  If a client requests "-l" (without the quotes),
  then respond with directory listing of working directory.
  
  If a client requests "-g <file name>",
  then respond with the contents of that file.
  
  A control connection remains running, and each request
  is serviced by an additional data connection.
***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "connectionController.h"
#include "fileManager.h"

int main (int argc, char **argv) {

	/*check command line syntax */
	if ( argc != 2 ) {
		printf( "Usage:\nftpserver <port number>\nNote: Port numbers 30021 and 30020 not allowed\n" );
		exit(1);
	}
	
	
	char* port = argv[1];
	
	
	/* check <port number> arg */
	
	/* adapted from http://stackoverflow.com/questions/16644906/how-to-check-if-a-string-is-a-number */
	/**********************************************************/
	int i = 0;
	while ( i < strlen( port ) ) {
		if( port[i] > '9' || port[i] < '0' ) {
			printf( "<port number> must be a number" );
		}
		i++;
	}
	/**********************************************************/
	
	if ( strcmp( port, "30021" ) == 0 || strcmp( port, "30020" ) == 0 ) {
		printf( "<port number> cannot be 30021 or 30020" );
		exit(1);
	}
	
	/* port number > 1024 and < 65535 */
    int x;
    sscanf( port, "%d", &x );
	if ( x < 1024 || x > 65535 ) {
		printf( "<port number> must be in range 1024 to 65535\n\n" );
		exit(0);
	}
	
	
	// start things up
	startup( port );

	
	return 0;
	
}