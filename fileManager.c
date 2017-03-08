
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
Linked List
This represents a list of file paths
**********************************************************/
struct path_node {
  char* d;
  struct path_node* next;
};

void free_nodes( struct path_node* head ) {
	struct path_node* node = head;
	while ( node != NULL ) {
		struct path_node* temp = node;
		node = node->next;
		free( temp );
	}
	head = NULL;
}

/**********************************************************
Forward declarations
**********************************************************/
struct path_node* _addLink (struct path_node** n, char* d_path );
void _dirList (const char* path, struct path_node** n);
char* _listToChar( struct path_node* n );

/**********************************************************
Parameters: const char* path : the directory to list
		struct path_node* n : the linked list to contain
		directory listing
Return: Directory listing of working directory as pointer
  to multidimensional char array.
Caller is responsible for deallocating the char* array returned
Note: This function calls itself recursively.
Note: This will not include empty directories.
**********************************************************/
char* listDir (const char* path) {
	struct path_node* n = malloc( sizeof(struct path_node) );
	n->d = "";
	n->next = NULL;
	struct path_node* list_root = n;
	_dirList (path, &n);
	char* dir_listing = _listToChar( list_root );
	free_nodes( list_root );
	return dir_listing;
}

/**********************************************************
Helper wrapped by listDir
Pre-Conditions: struct path_node* n must be initialized here
**********************************************************/
void _dirList (const char* path, struct path_node** n) {
	// get file paths here
	// adapted from: http://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program
	DIR *d;
	struct dirent *dir;
	
	d = opendir(path);
	
	if (d) {
		while ( (dir = readdir(d)) != NULL ) {
			if ( strcmp(dir->d_name,".")!= 0 && 
				  strcmp(dir->d_name,"..")!= 0 ) {
				char d_path[4096]; // here I am using sprintf which is safer than strcat
				sprintf(d_path, "%s/%s", path, dir->d_name);
				if ( dir -> d_type == DT_DIR ) { // if it is a directory
					_dirList( d_path, n ); // recall with the new path
				} else { // we've reached a file name, so add entire path
					*n = _addLink( n, d_path );
					printf("%s\n", d_path);
				}
			}
		}
		closedir(d);
	}
}

/**********************************************************
Parameters: struct path_node* n : the tail of a linked list
		char* d_path : the full path to add
Return: the added link
**********************************************************/
struct path_node* _addLink (struct path_node** n, char* d_path ) {
	struct path_node *n_next = malloc( sizeof(struct path_node) );
	int len = (strlen(d_path)+2);
	char* d = malloc( len * sizeof(char) );
	memset( d, 0, len );
	stpcpy( d, d_path );
	strcat( d, "\n" );
	n_next->d = d;
	n_next->next = NULL;
	(*n)->next = n_next;
	return (*n)->next;
}

/**********************************************************
Parameters: struct path_node* n : the head of a linked list
Return: char* string representing directory structure
Note: caller is responsible for deallocating char* returned
**********************************************************/
char* _listToChar( struct path_node* n ) {
	int len = 0;
	struct path_node* i = n; // iterator
	// count length of all node values put together
	while( i != NULL ) {
		if ( i->next != NULL ) {
			len = len + strlen( i->next->d );
		}
		i = i->next;
	}
	printf( "len in _listToChar: %d", len );
	
	// make char* string of directory structure
	char* dir_str = malloc( (len+1) * sizeof(char) );
	memset(dir_str, 0, strlen(dir_str)+1 );
	i = n;
	while( i != NULL ) {
		strcat( dir_str, i->d );
		i = i->next;
	}
	
	return dir_str;
}

/**********************************************************
Parameters: const char* filePath : the file path name
	relative to the current working directory
Return: Contents of file contained in working directory.
**********************************************************/


