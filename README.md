# 471ProjectFTP-ClientServer


#CPSC 471 Project 
Authors:
Matheo Maco matheomaco@csu.fullerton.edu

Programming language: C++
Run on Window/Linux

=========
Compiling
=========

make clean
make

=======
Running
=======

Open two terminals.

In one terminal type:

./server <PORT NUMBER>

In the other terminal type:

./client <SERVER IP> <PORT NUMBER>

Make sure that the port numbers for
client and server match.

The server will report the connection,
the data sent to the client, and any
status changes or errors.

=======
Example
=======

Note, the full output will be shown
after both server and client have been
deployed.

Server terminal:

./server 1234
Port  = 1234
Waiting for somebody to connect on port 1234
Connected!


Client terminal:

./client 127.0.0.1 1234

Enter get, put, ls or quit after ftp>
ftp> put
Enter the file name (.txt): small.txt
Sending file: small.txt (12 bytes)...
Uploaded file to server.
ftp>

======================
Commands 
======================
put <filename> (uploads file <file name> to the server)
get <filename> (downloads file <file name> from the server)
ls (lists files on the server)
quit (disconnects from the server and exits)

======================
Technical Information
======================

TCPLib.cpp implements functions that wrap standard read()/write()
system calls. Please see comments for details. 
