
/*********************************************************
Author: Benjamin R. Olson
Date: 2-25-17
Filename: connectionController.c

Description: OSU Networking CS 372, Project 2
  helper functions to manage networking connections
  
  Manages opening and closing TCP connections and
  sending and receiving data through TCP connections.
  
  **See connectionController.c for function comments.
***********************************************************/

#ifndef CONNECTIONCONTROLLER_H
#define CONNECTIONCONTROLLER_H


int isValidPortNumber ( const char* portno );

long long _charToLongLong ( const char* num_str );

int startup( const char* PORT );

#endif

