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

#define BACKLOG 10	 // how many pending connections queue will hold
#define HEADER_LEN 19

void handleRequest(int sockfd);
char* recvLen(int len, int sockfd);
char* receiveCommand( int sockfd );
void sendMessage(char message[], int len, int sockfd);

/**********************************************************
Pre-Conditions: portno is declared
Description: Validate char* input as valid port number:
  requirements: (1) port numbers 30021, 30020 not accepted
  (2) port number > 1024 and < 65535
Return: 0: invalid port number OR if valid,
	conversion of portno to it's numeric equivalent.
**********************************************************/
int isValidPortNumber ( const char* portno ) {
	
	return 0;
}


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

	return sockfd;
}


/*
Modified from: http://beej.us/guide/bgnet/examples/server.c
Description: Make a connection with new client on given socket
Return: New file descriptor for incoming connection or -1 on error
*/
int acceptCient( int sockfd ){
	struct sockaddr_storage their_addr; // connector's address information
	int new_fd; // new connection
	socklen_t sin_size;
	char s[INET6_ADDRSTRLEN];
	
	sin_size = sizeof their_addr;
	new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
	printf( "server: new_fd: %d\n", new_fd );
	if (new_fd == -1) {
		perror("accept");
		return -1;
	}

	inet_ntop(their_addr.ss_family,
		get_in_addr((struct sockaddr *)&their_addr),
		s, sizeof s);
	printf("server: got connection from %s\n", s);
	
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
		int new_fd = acceptCient( sockfd );
		// TODO: check for return value of -1 and close sockfd as well as new_fd
		
		if (!fork()) { // this is the child process
			close(sockfd); // child doesn't need the listener
			handleRequest(new_fd);
			exit(0);
		}
		close(new_fd);  // parent doesn't need this
	}

	return 0;
}

// TODO: define handleRequest function
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
	// TODO: receive port number and command for data connection
	
	char* command = receiveCommand( sockfd );
	printf( "DEBUG in handleRequest: command = '%s'\n\n", command );
	// TODO: parse string for command to get data port number and command
	
	// sockfd = bindAndListen( PORT from command ); // TODO: uncomment and continue here to 
	// open a data connection on data port number
	
	// if -l ..., else if -g <file> ...
	// sendMessage(char message[], int len, int sockfd)
	
	/* use some of this?
	if (send(new_fd, "9223372036854775807ABC", 22, 0) == -1)
				perror("send");
			close(new_fd);
	*/
}

/*
Description: Open new connection on port given by client
and set new_fd for data connection

TODO: calls recvLen twice: get command length and then command

Return: Command from client to be carried out on data connection
*/
char* receiveCommand( int sockfd ){
	char* string = NULL;
	long long bytes_expected = 0;

    // receive first 19 bytes to determine length of message
	string = recvLen(HEADER_LEN, sockfd);
	bytes_expected = _charToLongLong(string);
	printf( "DEBUG in receiveCommand: bytes_expeted = %lld\n\n", bytes_expected );
	//free(string); // TODO: uncomment after testing
	//string = NULL; // TODO: uncomment after testing
	
	
	
	// receive the data
    //string = recvLen(bytes_expected, sockfd); // TODO: uncomment after testing
	
	return string;
}

// TODO: define function to carry out command here, making use of sendMessage below

// TODO: adapt the following two functions

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
	//int chunk_size = 4096;
	//int recv_len = (len<chunk_size)?len:chunk_size;
	
	int total_bytes = 0;
	int bytes_recv = recv(sockfd, message, len, 0);
	int attempts = 0;
	while ( attempts < 20 && total_bytes < len ) {
		if ( bytes_recv == len ) {
			printf( "%s\n", message );
			break;
		} else if ( bytes_recv == -1 ) {
			perror("recv");
			attempts++;
		} else if ( bytes_recv == 0 ) {
			printf( "Client may have closed connection.\n" );
			close(sockfd);
			exit(0);
		} else if ( bytes_recv < len && bytes_recv > 0 ) {
			total_bytes += bytes_recv;
			//int remaining = len - total_bytes;
			//recv_len = (remaining < chunk_size) ? len : chunk_size;
			//bytes_recv = recv(sockfd, &message[total_bytes], len - total_bytes, 0);
			bytes_recv = recv(sockfd, &message[total_bytes], len - total_bytes, 0);
		}
	}
	/*
	if (attempts >= 20){
		close(sockfd);
		perror("recv");
		exit(0);
	}
	*/
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
}



