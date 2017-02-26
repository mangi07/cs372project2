###########################################################
# Author: Benjamin R. Olson
# Date: 2-25-17
# File Name: makefile
# Description: OSU Networking CS 372, Project 2 Makefile
###########################################################



# main program
default: ftserver.c connectionController.o fileManager.o
	gcc -Wall -g -o ftserver ftserver.c
    
tests: connectionController.o connectionControllerTests.c fileManager.o
	gcc -Wall -g -o connectionControllerTests connectionControllerTests.c connectionController.o
	gcc -Wall -g -o fileManagerTests fileManagerTests.c fileManager.o

	
# build helper functions	
connectionController.o: connectionController.c connectionController.h
	gcc -Wall -g -c connectionController.c

fileManager.o: fileManager.c fileManager.h
	gcc -Wall -g -c fileManager.c


# clean and rebuild helper functions
rebuildHelpers: cleanHelpers connectionController.o fileManager.o

# clean and rebuild tests
rebuildTests: cleanHelpers tests

cleanHelpers:
	rm connectionController.o
	rm fileManager.o
	
	
# clean all object files and executables
clean:
	rm *.o
	rm ftserver
	rm connectionControllerTests
	rm fileManagerTests
	rm connectionControllerTests.exe
	rm fileManagerTests.exe

# run the tests
# comment out .exe on linux or the others on windows cygwin
runtests: tests
	./connectionControllerTests
	./fileManagerTests

