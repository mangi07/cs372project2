
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
char* listDir (const char* path) {
	// get file paths here
	// adapted from: http://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program
	DIR *d;
	struct dirent *dir;
	d = opendir(path);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if ( strcmp(dir->d_name,".")!=0 && 
				  strcmp(dir->d_name,"..")!=0 ) {
				char d_path[255]; // here I am using sprintf which is safer than strcat
				sprintf(d_path, "%s/%s", path, dir->d_name);
				if ( dir -> d_type == DT_DIR ) { // if it is a directory
					listDir(d_path); // recall with the new path
				} else { // we've reached a file name, so print it's entire path
					// if listing starts with "./", remove it here...TODO:
					
					printf("%s\n", d_path);
				}
			}
		}
		closedir(d);
	}
	
	// TODO: modify directory listings above to be collected into char*
	char* listing = malloc ( 50 * sizeof(char) );
	return listing;
}



/**********************************************************
Parameters: const char* filePath : the file path name
	relative to the current working directory
Return: Contents of file contained in working directory.
**********************************************************/


