# Author: Ben R. Olson
# Date: 2-11-17

# Adapted from https://docs.python.org/2/library/socket.html#example

from socket import *
import sys

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
# TODO: look at discussion board for answer about how to handle file overwriting
"""
HOST = 'daring.cwi.nl'    # The remote host
PORT = 50007              # The same port as used by the server
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
s.sendall('Hello, world')
data = s.recv(1024)
s.close()
print 'Received', repr(data)
"""

"""
Arguments:
host: the hostname of the server
port: the port number where server is receiving new connections

Returns:
clientSocket: the socket for this client's connection
"""
def makeConnection(host, port):
    clientSocket = socket(AF_INET, SOCK_STREAM)
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
        temp_str = sock.recv(HEADER_LEN)
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
            temp = sock.recv(4092)
            if temp == "":
                break
            bytes_received += len(temp)
            string += temp
        except:
            sock.close()
            print "ERROR RECEIVING DATA"
            sys.exit(1)
    sock.close()
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
    # uncomment these after done testing on server
    # check that command is correct syntax
    #if command != '-l' and command != '-g':
    #    printUsage()
    #if not representsInt(portno):
    #    printUsage()
    command = command + " " + filename + " " + portno
    return command


def printUsage():
    print "\nUsage: python ftclient.py <hostname> <control portnumber> <command> <data portnumber>\n"
    print "Arguments:\n"
    print "<hostname>: use localhost or flip1\n"
    print "<control portnumber>: must be a number in range (1024 to 65535) but not 30021 or 30020\n" \
          "\t(Note: The port number must be the same port that the server started on.)\n"
    print "<command>: The command can take either one of the following formats:\n" \
          "\t(1) \'-l\' (without quotes) to list all files within the working directory\n" \
          "\t(2) \'-g <filename>\' (without quotes) to request the given filename.\n"
    print "Example: python ftclient.py localhost 50000"
    sys.exit(1)


if __name__ == "__main__":
    if len(sys.argv) != 5 and len(sys.argv) != 6:
        printUsage()

    # DEBUG
    print getConnInfo()
    print getCommand()

    host, control_port, data_port = getConnInfo()
    sock = makeConnection(host, control_port)
    sendCommand(getCommand(), sock)
    print receiveMessage(sock)


    print
    print "Connection closed."
