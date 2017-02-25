
/*********************************************************
Author: Benjamin R. Olson
Date: 2-25-17
Filename: connectionController.c

Description: OSU Networking CS 372, Project 2
  helper functions to manage networking connections
  
  Manages opening and closing TCP connections and
  sending and receiving data through TCP connections.
***********************************************************/

#ifndef CONNECTIONCONTROLLER_H
#define CONNECTIONCONTROLLER_H


/**********************************************************
Pre-Conditions: portno is declared
Description: Validate char* input as valid port number:
  requirements: (1) port numbers 30021, 30020 not accepted
  (2) port number > 1024 and < 65535
Return: 0: invalid port number OR if valid,
	conversion of portno to it's numeric equivalent.
**********************************************************/
int processPortNumber ( const char* portno );

/**********************************************************
Description: Convert char* num to int:
  requirements: (1) char* num must be convertable to int
Return: 0: invalid number OR if valid,
	conversion of num to it's numeric equivalent.
**********************************************************/
int _charToInt ( const char* portno );

/**********************************************************
Pre-Conditions: portno is a valid port number
Description: Validate char* as 
Return: 0: invalid port number OR if valid,
	conversion of portno to it's numeric equivalent.
**********************************************************/

#endif CONNECTIONCONTROLLER_H

