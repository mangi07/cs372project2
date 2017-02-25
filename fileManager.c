
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


/**********************************************************
Return: Directory listing of working directory as pointer
  to multidimensional char array.
Caller is responsible for deallocating this array
**********************************************************/
char** listDir () {
	// get file paths here
	char** listing = malloc ( sizeof char * 50 );
	return listing
}



/**********************************************************
Parameters: const char* filePath : the file path name
	relative to the current working directory
Return: Contents of file contained in working directory.
**********************************************************/


