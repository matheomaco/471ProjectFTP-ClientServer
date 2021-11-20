#include <stdio.h>      /* Contains common I/O functions */
#include <sys/types.h>  /* Contains definitions of data types used in system calls */
#include <sys/socket.h> /* Includes definitions of structures needed for sockets */
#include <netinet/in.h> /* Contains constants and structures needed for internet domain addresses. */
#include <unistd.h>     /* Contains standard unix functions */
#include <stdlib.h>     /* For atoi() and exit() */
#include <string.h> 	/* For memset() */
#include <string>
#include <arpa/inet.h>  /* For inet_pton() */
#include <time.h>
#include <limits.h>
#include <sys/stat.h>   /* For stat() */
#include "TCPLib.h"
#include <sys/sendfile.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>

using namespace std;

const int MAXSIZE = 1024;

FILE *popen(const char *command, const char *type);

/**
 * Returns the output of a command in C++ string format
 * @param cmd - the command to execute
 * @return - the output of the commaned in C++ format
 */
string getCmdOutput(const char* cmd)
{
	/* The buffer to store the results */
	string result = "";
	
	/* Open the output stream of the program for reading */
	FILE* fp = popen(cmd, "r");
	
	/* The output buffer */
	char outBuff[10];
	
	/* The number of bytes read */
	int numRead = 0;
	
	/* Make sure the stream was opened */
	if(!fp)
	{
		perror("popen");
		exit(-1);
	}
	
	/* Read the whole file */
	while(!feof(fp))
	{	
		/* Read information from the output stream of the program */
		if((numRead = fread(outBuff, sizeof(char), sizeof(outBuff) - 1, fp)) < 0)
		{
			perror("fread");
			exit(-1);
		}
			
		/* NULL terminate the string */
		outBuff[numRead] = '\0';	
	
		/* If anything was read, then save it! */
		if(numRead)
			result += outBuff;
		
	}
	
	/* Close the output stream of the program */
	if(pclose(fp) < 0)
	{
		perror("pclose");
		exit(-1);
	}
	
	return result;
	
}

int getFileSize(const char* fileName)
{
	/* The structure containing the file information */
	struct stat fileInfo;
	
	/* Get the file size */
	if(stat(fileName, &fileInfo) < 0)
		return -1;
	
	/* Return the file size */
	return fileInfo.st_size;
}

void sendFileInfo(const int& socket, const char* fileName)
{
	/* Get the size of the file name */
	int fileNameSize = strlen(fileName);
	
	/* Send the size of the file name */
	if(tcp_send_size(socket, fileNameSize) < 0)
	{
		perror("tcp_send_size");
		exit(-1);
	}
	
	/* Send the actual file name */	
	if(tcp_send(socket, fileName, fileNameSize) < 0)
	{
		perror("tcp_send");
		exit(-1);
	}
}

int main(int argc, char** argv)
{
  string cmd[MAXSIZE], recline[MAXSIZE];
  int port = -1;
  int connfd = -1;
  int numSent = 0;
  const char* fileName;

  string respondLine;
  off_t totalNumSent = 0;
  int fileNameSize = -1;
  sockaddr_in serverAddr;
  socklen_t servLen = sizeof(serverAddr);

  if(argc != 3)
 {
  /* Report an error */
  fprintf(stderr, "USAGE: %s <SERVER IP> <SERVER PORT #>", argv[0]);
  exit(-1);
 }

 //get the port number
 port = atoi(argv[2]);
 if(port < 0 || port > 65535)
 {
   fprintf(stderr, "Invalid port number\n");
		exit(-1);
 }

 //connect to the server
 if ((connfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(-1);
  }

  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);

  if(!inet_pton(AF_INET, argv[1], &serverAddr.sin_addr))
  {
    perror("inet_pton");
    exit(-1);
  }

  if(connect(connfd, (sockaddr*)&serverAddr, sizeof(sockaddr))<0)
  {
    perror("connect");
    exit(-1);
  }

  string input;
  cout << "Enter get, put, ls or quit after ftp" << endl;
  cout << "ftp> ";
  cin >> input;

  while (input != "quit") {
  
      send(connfd, cmd, MAXSIZE, 0);
      if(input == "get") {
        sendFileInfo(connfd, input.c_str());
        cin.clear();
        cout << "Enter the file name (.txt): ";
        cin.get();
        getline(cin,input);
        fileName = input.c_str();
            /* Send the file name */
	    sendFileInfo(connfd, fileName);
	
	    /* Get the file size */
	    int fileSize = getFileSize(fileName);	
	    
	    /* Did we successfully obtain the file size ? */
	    if(fileSize < 0) {
		    perror("stat");
		    exit(-1);
	    }
		    
	    /* Send the file size */
	    if(tcp_send_size(connfd, fileSize) < 0) {
		    perror("tcp_send_size");
		    exit(-1);
	    }
	    
	    /* Open t        fileName = input.c_str();he file */
	    int fd = open(fileName, O_RDONLY);
	    
	    /* Make sure the file was successfully opened */
	    if(fd < 0) {
		    perror("fopen");
	    }	
	    fprintf(stderr, "Receiving file: %s (%d bytes)...\n", fileName, fileSize);
	    /* Keep receiving until the whole file is sent */	
	    while(totalNumSent < fileSize) {	
		    /* Send the file to the server */
		    if((numSent = sendfile(connfd, fd, &totalNumSent, fileSize - totalNumSent)) < 0) {
			    perror("sendfile");
			    exit(-1);
		    }
		    /* Update the total number of bytes sent */
		    totalNumSent += numSent;		    
	    }
	    /* Close the file */
	    close(fd);
        cout << "Downloaded file from server.\n";
      }
      else if (input == "put"){
        
        sendFileInfo(connfd, input.c_str());
        cin.clear();
        cout << "Enter the file name (.txt): ";
        cin.get();
        getline(cin,input);
        fileName = input.c_str();
            /* Send the file name */
	    sendFileInfo(connfd, fileName);
	
	    /* Get the file size */
	    int fileSize = getFileSize(fileName);	
	    
	    /* Did we successfully obtain the file size ? */
	    if(fileSize < 0) {
		    perror("stat");
		    exit(-1);
	    }
		    
	    /* Send the file size */
	    if(tcp_send_size(connfd, fileSize) < 0) {
		    perror("tcp_send_size");
		    exit(-1);
	    }
	    
	    /* Open the file */
	    int fd = open(fileName, O_RDONLY);
	    
	    /* Make sure the file was successfully opened */
	    if(fd < 0) {
		    perror("fopen");
	    }	
	    fprintf(stderr, "Sending file: %s (%d bytes)...\n", fileName, fileSize);
	    /* Keep sending until the whole file is sent */	
	    while(totalNumSent < fileSize) {	
		    /* Send the file to the server */
		    if((numSent = sendfile(connfd, fd, &totalNumSent, fileSize - totalNumSent)) < 0) {
			    perror("sendfile");
			    exit(-1);
		    }
		    /* Update the total number of bytes sent */
		    totalNumSent += numSent;		    
	    }
	    /* Close the file */
	    close(fd);	
        cout << "Uploaded file to server.\n"; 
      }
      
      else if (input == "ls") {
       
	    cout << "Files to print from ls:\n";
	    
	    /* Get the output of the ls command */
	    string result =  getCmdOutput("ls");
	    
	    /* Print the output of the ls command */
	    fprintf(stderr, "%s\n", result.c_str());	
      
      }

      else
        cout << "Invalid Option. Please try again with a valid option.\n";

    cout << "ftp>";
    cin >> input;
   }
return 0;
}
