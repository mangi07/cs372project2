"""
Author: Ben R. Olson
Date: 2-11-17
Adapted from https://docs.python.org/2/library/socket.html#example

Description: Simple ftp client that either
(1) recursively lists files on server or
(2) attempts to download a file from the server
    The file download attempt will fail if the file name
(just file name without path) is found in the client's
working directory.
    The file download attempt will still be made if no
file of the same name is found in the working directory,
even if found in a subdirectory.
"""

from socket import *
import sys
import os.path

"""
characters needed to hold string representation
of number max 9223372036854775807
"""
HEADER_LEN = 19

"""
Simple protocol used (length followed by data):
**length: The first HEADER_LEN (19) bytes received
will be a string representation of
the number of following data bytes being sent.
This is because sys.maxint is estimated to be
9223372036854775807 bytes
**data: The following data should be length bytes.
"""

# client has at least initiateContact , makeRequest ,
# receiveFile functions


"""
Arguments:
host: the hostname of the server
port: the port number where server is receiving new connections

Returns:
clientSocket: the socket for this client's connection
"""
def makeConnection(host, port):
    clientSocket = socket(AF_INET, SOCK_STREAM)
    #clientSocket.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
    clientSocket.connect((host, port))
    return clientSocket

"""
Description:
Receives a message from the server

Arguments:
sock: a connected socket ready to send and recv

Returns:
string of characters received
or the error message received
"""
def receiveMessage(sock):
    string = ""
    bytes_received = 0
    remainder_str = ""
    bytes_expected = 0

    # receive first 19 bytes to determine length of message
    while bytes_received < HEADER_LEN:
        temp_str = sock.recv(HEADER_LEN-bytes_received)
        string += temp_str
        bytes_received += len(temp_str)
    if len(string) > HEADER_LEN:
        remainder_str = string[HEADER_LEN:]
    bytes_expected = int(string[:HEADER_LEN])

    bytes_received = len(remainder_str)

    # receive the data
    string = ""
    while bytes_received < bytes_expected:
        try:
            temp = sock.recv(bytes_expected-bytes_received)
            if temp == "":
                break
            bytes_received += len(temp)
            string += temp
        except:
            sock.close()
            print "ERROR RECEIVING DATA"
            sys.exit(1)
    return string


"""
Description:
Send a message to the server

Arguments:
sock: a connected socket ready to send and recv
"""
def sendCommand(command, sock):
    byte_len = len(command)
    message = "" + str(byte_len).rjust(HEADER_LEN) + command
    message_len = len(message)
    bytes_sent = 0

    while bytes_sent < message_len:
        try:
            bytes_sent += sock.send(message)
            message = message[bytes_sent:]
        except:
            print("ERROR SENDING MESSAGE")
            sock.close()


"""
Pre-Conditions: sys.argv length must be 5 or 6

Returns: a tuple representing the connection to be made:
(control_port, data_port, host)
"""
def getConnInfo():
    control_port = -1
    data_port = -1

    # get port info
    # adapted from https://edumaven.com/python-programming/is-number
    try:
        control_port = int(sys.argv[2])
        if len(sys.argv) == 5:
            data_port = int(sys.argv[4])
        elif len(sys.argv) == 6:
            data_port = int(sys.argv[5])
        else:
            printUsage()
    except ValueError:
        printUsage()

    """
    Validate char* input as valid port number:
    requirements: (1) control port and data port must be different
    (2) port numbers 30021, 30020 not accepted
    (3) port number > 1024 and < 65535
    """
    if control_port == data_port: # (1)
        print "INPUT ERROR: control port and data port must be different"
        sys.exit(1)
    if control_port == 30021 or control_port == 30020: # (2)
        print "INPUT ERROR: control port cannot be 30021 or 30020"
        sys.exit(1)
    if data_port == 30021 or data_port == 30020: # (2)
        print "INPUT ERROR: data port cannot be 30021 or 30020"
        sys.exit(1)
    if control_port < 1024 or control_port > 65535: # (3)
        print "INPUT ERROR: control port must be within range 1024 to 65535"
        sys.exit(1)
    if data_port < 1024 or data_port > 65535: # (3)
        print "INPUT ERROR: data port must be within range 1024 to 65535"
        sys.exit(1)

    host = sys.argv[1]

    return host, control_port, data_port

# borrowed from: http://stackoverflow.com/questions/1265665/python-check-if-a-string-represents-an-int-without-using-try-except
def representsInt(s):
    try:
        int(s)
        return True
    except ValueError:
        return False

"""
Pre-Conditions: sys.argv length must be 5 or 6
Returns: a string representing the request to the server:
"-l" to list files in the working directory or
"-g <filename>" to attempt to get a file with 'filename'
"""
def getCommand():
    command = sys.argv[3]
    filename = ''
    if len(sys.argv) == 6:
        filename = sys.argv[4]
        portno = sys.argv[5]
    else:
        portno = sys.argv[4]

    # check that command is correct syntax
    if command != '-l' and command != '-g':
        printUsage()
    if not representsInt(portno):
        printUsage()

    """Validate file does not exist in working directory"""
    f = filename[filename.rfind("/", 0)+1:]
    print "DEBUG: {0}".format(f)
    if command == '-g' and os.path.exists(f):
        print "INPUT ERROR: filename exists in working directory."
        print "Stopping to prevent overwriting file \"{0}\"".format(f)
        sys.exit(1)

    flag = command
    command = command + " " + filename + " " + portno
    return command, flag, filename, portno


def printUsage():
    print "\nUsage: python ftclient.py <hostname> <control portnumber> <command> <data portnumber>\n"
    print "Arguments:\n"
    print "<hostname>: use localhost or flip1\n"
    print "<control portnumber>: must be a number in range (1024 to 65535) but not 30021 or 30020\n" \
          "\t(Note: <control portnumber> must be the same port that the server started on.)\n"
    print "<command>: The command can take either one of the following formats:\n" \
          "\t(1) \'-l\' (without quotes) to list all files within the working directory\n" \
          "\t(2) \'-g <filename>\' (without quotes) to request the given filename.\n"
    print "Example 1: python ftclient.py localhost 50000 -g testdir/testfile.txt 50004"
    print "Example 2: python ftclient.py localhost 50000 -l 50004"
    sys.exit(1)


if __name__ == "__main__":
    if len(sys.argv) != 5 and len(sys.argv) != 6:
        printUsage()

    command, flag, filename, portno = getCommand()

    # control connection
    host, control_port, data_port = getConnInfo()
    sock = makeConnection(host, control_port)
    sendCommand(command, sock)
    print receiveMessage(sock)
    sock.close()

    # data connection
    data_sock = makeConnection(host, data_port)
    # tell server we're ready to receive
    sendCommand("OK", data_sock)
    if flag == '-l':
        print receiveMessage(data_sock)
    elif flag == '-g':
        # check if file was successfully retreived on server
        success = receiveMessage(data_sock)
        if success == "OK":
            print "Receiving \"{0}\" from :{1}".format(filename, portno)
            print receiveMessage(data_sock)  # turn this into file write
        else:
            print receiveMessage(data_sock)
    # send confirmation to server, "OK" to close connection on server
    sendCommand("OK", data_sock)

    data_sock.close()

    # TODO: test with "python ftclient.py localhost 3490 -g testdir/testfile.txt 50004"
    # TODO: test with "python ftclient.py localhost 3490 -g testdir/bible.txt 50004"
    # TODO: test with "python ftclient.py localhost 3490 -l 50004"
    # TODO: test these on server
    # TODO: commit
    # TODO: replace debug statements with useful status updates
    # TODO: replace hard-coded server port with command line input to get port number for server startup
    print
    print "Connection closed."
