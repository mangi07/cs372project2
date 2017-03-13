/*********************************************************
Author: Benjamin R. Olson
Date: 2-25-17
Filename: connectionController.c

Description: OSU Networking CS 372, Project 2
  helper functions to manage networking connections
  
  Manages opening and closing TCP connections and
  sending and receiving data through TCP connections.

*****
Note: the following functions make use of server.c from
  http://beej.us/guide/bgnet/examples/server.c
int bindAndListen( const char* PORT )
int acceptCient( int sockfd )
void sigchld_handler(int s)
void *get_in_addr(struct sockaddr *sa)
*****
***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <limits.h>

#include "fileManager.h"

#define BACKLOG 10	 // how many pending connections queue will hold
#define HEADER_LEN 19

void handleRequest(int sockfd);
char* recvLen(int len, int sockfd);
char* receiveCommand( int sockfd );
void sendMessage(char message[], int len, int sockfd);
void parseCommand( char* command, int* get_file, char* file, char* port, int sockfd );
char* prepareMessage( char* payload );
int getPortNo(int sock);


/**********************************************************
Description: Convert char* num to int:
  requirements: (1) char* num must be convertable to int
Return: 0: invalid number OR if valid,
	conversion of num to it's numeric equivalent.
**********************************************************/
long long _charToLongLong ( const char* num_str ) {
	long long num = atoll ( num_str );
	return num;
}


// borrowed from: http://beej.us/guide/bgnet/examples/server.c
void sigchld_handler(int s)
{
	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}


// borrowed from: http://beej.us/guide/bgnet/examples/server.c
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


/*
Modified from: http://beej.us/guide/bgnet/examples/server.c
Description: Open a new socket with given host and port number
Return: socket file descriptor of listening socket
*/
int bindAndListen( const char* PORT ){
	int sockfd;  // listen on sock_fd
	struct addrinfo hints, *servinfo, *p;
	struct sigaction sa;
	int yes=1;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET; // IPv4
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("SERVER: Listening on port %d\n", getPortNo(sockfd));
	
	return sockfd;
}


/*
Modified from: http://beej.us/guide/bgnet/examples/server.c
Description: Make a connection with new client on given socket
Return: New file descriptor for incoming connection or -1 on error
*/
int acceptClient( int sockfd ){
	struct sockaddr_storage their_addr; // connector's address information
	int new_fd; // new connection
	socklen_t sin_size;
	char s[INET6_ADDRSTRLEN];
	
	sin_size = sizeof their_addr;
	new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

	if (new_fd == -1) {
		perror("accept");
		return -1;
	}

	inet_ntop(their_addr.ss_family,
		get_in_addr((struct sockaddr *)&their_addr),
		s, sizeof s);
		
	printf("SERVER: got connection from %s on port %d\n", s, getPortNo(new_fd));
	
	return new_fd;
}


/*
Modified from: http://beej.us/guide/bgnet/examples/server.c
THIS IS THE MAIN ENTRY POINT: CALLED FROM MAIN - or just move to main
calls handleRequest
*/
int startup( const char* PORT ){
	int sockfd = bindAndListen( PORT );
	
	while(1) {  // main accept() loop
		int new_fd = acceptClient( sockfd );
		
		if (!fork()) { // this is the child process
			close(sockfd); // child doesn't need the listener
			handleRequest(new_fd);
			exit(0);
		}
		close(new_fd);  // parent doesn't need this
	}

	return 0;
}

/*
Simple protocol used (length followed by data):
**length: The first HEADER_LEN (19) bytes received
will be a string representation of
the number of following data bytes being sent.
This is because long long is estimated to be
9223372036854775807 bytes
**data: The following data should be length bytes.
*/
void handleRequest(int sockfd){
	int get_file = 0;
	char file_path[PATH_MAX];
	memset(file_path, 0, sizeof(file_path));
	char port[PATH_MAX];
	// PATH_MAX in case port number is contained in char[] file_path on command '-l'
	// in which case the port number in char[] file_path will be copied to char[] port 
	memset(port, 0, sizeof(port));
	
	char* command = receiveCommand( sockfd );
	//printf( "DEBUG in handleRequest: command = '%s'\n\n", command );
	
	parseCommand( command, &get_file, file_path, port, sockfd ); // command syntax error --> send error msg and close connection
	
	free(command);
	 
	
	// close current connection
	close(sockfd);
	
	// open a data connection on data port number
	int datafd = bindAndListen( port );
	int new_fd = acceptClient( datafd );
	
	// wait for client to say "OK"
	receiveCommand(new_fd); //  return value ignored
	printf( "CLIENT ready to receive on port: %s\n\n", port );
	
	// check action and respond accordingly
	if ( get_file == 1 ){ // try to get the file
		/*in this case the client is expecting either
		(1) two messages if file retrieval was successful or
		(2) one message if not.
		
		First message is always either "OK" or "NO" 
		to indicate successful file retrieval.
		
		function declaration from fileManager.h:
		char* loadFile( const char* file_path, int* error )*/
		
		// attempt to get file
		int error;
		char* payload = loadFile( file_path, &error ); // defined in fileManager.c
		
		
		// send "OK" or "NO", whether file retrieval was successful...
		if ( error == 0 ) {
			sendMessage("                  2OK", 21, new_fd);
		} else {
			sendMessage("                  2NO", 21, new_fd);
		}
		// ...and client will know what to do with it
		
		char* message = prepareMessage( payload ); 
		sendMessage(message, strlen(message), new_fd);
		
		free(payload);
		free(message);
	} else { // list directory
		// defined in fileManager.c
		char* payload = listDir(".");
		char* message = prepareMessage( payload );
		sendMessage(message, strlen(message), new_fd);
		free(payload);
		free(message);
	}
	
	// wait for client to say "OK" before closing connection
	receiveCommand(new_fd); // return value ignored
	
	int new_fd_port = getPortNo(new_fd);
	close(datafd);
	close(new_fd);
	
	printf( "SERVER closed connection on port %d\n\n.", new_fd_port );
}

/*
Description: Open new connection on port given by client
and set new_fd for data connection

Return: Command from client to be carried out on data connection
*/
char* receiveCommand( int sockfd ){
	char* string = NULL;
	long long bytes_expected = 0;

    // receive first 19 bytes to determine length of message
	string = recvLen(HEADER_LEN, sockfd);
	bytes_expected = _charToLongLong(string);
	free(string); 
	string = NULL;
	
	// receive the data
    string = recvLen(bytes_expected, sockfd);
	
	return string;
}

/*
borrowed from: 
http://stackoverflow.com/questions/4046616/sockets-how-to-find-out-what-port-and-address-im-assigned
*/
int getPortNo(int sock){
	struct sockaddr_in sin;
	socklen_t len = sizeof(sin);
	if (getsockname(sock, (struct sockaddr *)&sin, &len) == -1)
		perror("getsockname");
	else
		return ntohs(sin.sin_port);
	return -1;
}


/*
Arguments:
command: the command to parse
get_file: whether we should retrieve a file (if not, then list directory)
  should be assigned either '-l' or '-g'
file: file path string
port: new port that we will connect on
sockfd: current, control connection in case we need to send an error message

command syntax error --> send error msg and close connection

Sends message 
*/
void parseCommand( char* command, int* get_file, char* file, char* port, int sockfd ){
	char action[3]; // should be either '-l' or '-g'
	memset(action, 0, sizeof(action) );
	*get_file = 0; // defaults to listing directory instead of getting a file
	
	sscanf( command, "%s %s %s", action, file, port );
	
	if (strcmp(action, "-l") == 0 || strcmp(port, "0") == 0 ){ // we may have only two arguments in command
		strncpy(port, file, strlen(file));
		printf("DEBUG in parseCommand: port = %s\n\n", port);
		file = NULL;
	}
	
	// check if port is already in use
	int curr_port;
	sscanf(port, "%d", &curr_port);
	
	if ( (getPortNo(sockfd)) == curr_port) {
		char* message = "                 40PORT ALREADY IN USE (CURRENT CONNECTION)";
		sendMessage(message, 59, sockfd);
		return;
	}
	
	// send the client a confirmation of the action to be taken based on its command
	char intent[4092];
	memset(intent, 0, 4092 );
	
	if ( strcmp(action, "-g") == 0 ) {
		*get_file = 1;
		printf( "File \"%s\" requested on port %s.\n\n",  file, port );
		sprintf( intent, "SENDING FILE \"%s\" on port %s...", file, port );
	} else if ( strcmp(action, "-l") == 0 ) {
		*get_file = 0;
		printf("SERVER: Client asked to list file directory with '-l' on port %s\n\n", port);
		sprintf( intent, "SENDING DIRECTORY LISTING ON PORT %s...", port );
	} else {
		char* message = "                 36INVALID FLAG (EXPECTED '-g' or '-l')";
		sendMessage(message, 55, sockfd);
		return;
	}
	
	char* message = prepareMessage( intent );
	int message_len = strlen(message);
	sendMessage(message, message_len, sockfd);
	free( message );
}

/*
Arguments:
payload: c-string of the data being sent

Return:
message ready to send to client,
containing length followed by payload
*/
char* prepareMessage( char* payload ){
	int payload_len = strlen(payload);
	int message_len = HEADER_LEN + payload_len;
	char* message = malloc( message_len + 1 );
	memset( message, 0, message_len + 1 );
	
	char len_str[HEADER_LEN+1];
	memset(len_str, 0, HEADER_LEN+1 );
	sprintf(len_str, "%d", payload_len);
	
	int spaces = HEADER_LEN - strlen(len_str);
	
	int i;
	for( i = 0; i < spaces; ++i ){
		strcat( message, " " );
	}
	
	strcat( message, len_str );
	strcat( message, payload );
	return message;
}

/*
Description: Receive len bytes into message c-string 
(appending '\0' right after bytes received).
Pre-Conditions: memory is allocated for message array
Return: dynamically allocated c-string containing bytes received
**Note: caller is responsible for freeing returned string 
*/
char* recvLen(int len, int sockfd)
{
	// prepare receiving string
	int message_size = len * sizeof(char) + 1;
	char* message = malloc(message_size); // one extra for null terminator
	memset(message, 0, message_size); // zero-fill message
	
	
	int total_bytes = 0;
	int bytes_recv = recv(sockfd, message, len, 0);
	
	int attempts = 0;
	while ( attempts < 20 && total_bytes < len ) {
		if ( bytes_recv == len ) {
			total_bytes += bytes_recv;
			break;
		} else if ( bytes_recv == -1 ) {
			perror("recv");
			attempts++;
		} else if ( bytes_recv == 0 ) {
			close(sockfd);
			exit(0);
		} else if ( bytes_recv < len && bytes_recv > 0 ) {
			total_bytes += bytes_recv;
			bytes_recv = recv(sockfd, &message[total_bytes], len - total_bytes, 0);
		}
	}
	
	message[total_bytes] = '\0';
	
	
	return message;
}

void sendMessage(char message[], int len, int sockfd)
{
	int total_bytes = 0;
	int bytes_sent = send(sockfd, message, len, 0);
	int attempts = 0;
	while ( attempts < 20 && total_bytes < len ) {
		if ( bytes_sent == len ) {
			break;
		} else if ( bytes_sent == -1 ) {
			perror("send");
			attempts++;
		} else if ( bytes_sent < len && bytes_sent > 0 ) {
			total_bytes += bytes_sent;
			bytes_sent = send(sockfd, &message[total_bytes], len - total_bytes, 0);
		}
	}
	if (attempts >= 20){
		close(sockfd);
		exit(0);
	}
	printf( "SERVER: sending message on port %d\n\n", getPortNo(sockfd) );
}



