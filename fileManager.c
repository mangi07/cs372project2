
/*********************************************************
Author: Benjamin R. Olson
Date: 2-25-17
Filename: fileManager.c

Description: OSU Networking CS 372, Project 2
  helper functions to manage file reading and 
  directory listing
  
  If a client requests "-l" (without the quotes),
  then get directory listing of working directory.
  
  If a client requests "-g <file name>",
  then get the contents of that file.
***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

/**********************************************************
Arguments: const char* path : the directory to list
Return: Directory listing of working directory as pointer
  to multidimensional char array.
Caller is responsible for deallocating this array
Note: This function calls itself recursively.
**********************************************************/
char* listDir (char* path) {
	// get file paths here
	// adapted from: http://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program
	DIR *d;
	struct dirent *dir;
	char* listing = NULL;
	
	d = opendir(path);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			
		}
		closedir(d);
	} // TODO: else error
	
	
	return listing;
}

char* _fileListing ( struct dirent *dir ) {
	// create space to store dir listing
	int size = strlen( dir->d_name ) * sizeof(char);
	file_listing = malloc ( size );
	
	if ( strcmp(dir->d_name,".")!= 0 && 
		  strcmp(dir->d_name,"..")!= 0 ) {
		sprintf( file_listing, "%s", dir->d_name );
	}
	
	while ( dir->d_type == DT_DIR ) {
		// get new dir
		DIR *d;
		struct dirent *new_dir;
		d = opendir(file_listing);
		if (!d) {
			perrror( "Fatal error opening directory" );
			exit( -1 );
		}
		new_dir = readdir(d);
		
		// add to the path
		char* new_dir_name = dir->d_name;
		prev_listing = file_listing;
		size = size + strlen(new_dir_name)*sizeof(char) + 1;
		file_listing = malloc( size );
		sprintf( file_listing, "%s/%s", file_listing, new_dir_name );
		free( prev_listing );
	}
	// we've reached a file name
	sprintf( "%s\n", listing );
	
	
	return file_listing;
}



/**********************************************************
Parameters: const char* filePath : the file path name
	relative to the current working directory
Return: Contents of file contained in working directory.
**********************************************************/


