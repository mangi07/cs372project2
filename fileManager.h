/*********************************************************
Author: Benjamin R. Olson
Date: 2-25-17
Filename: fileManager.h

Description: OSU Networking CS 372, Project 2
  helper functions to manage file reading and 
  directory listing
  
  If a client requests "-l" (without the quotes),
  then get directory listing of working directory.
  
  If a client requests "-g <file name>",
  then get the contents of that file.
 
Note: functions documented in source, fileManager.c
***********************************************************/

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

char** listDir ();




#endif FILEMANAGER_H
