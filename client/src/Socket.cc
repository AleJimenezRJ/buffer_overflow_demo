/**
 *   CI0123 PIRO
 *   Clase para utilizar los "sockets" en Linux
 *
 **/
#include <iostream>
#include <stdexcept>
#include <unistd.h> 
#include <stdio.h>	// for perror
#include <stdlib.h>	// for exit
#include <string.h>	// for memset
#include <arpa/inet.h>	// for inet_pton
#include <sys/types.h>	// for connect 
#include <sys/socket.h>

#include "Socket.h"

/**
  *  Class constructor
  *     use Unix socket system call
  *
  *  @param	char type: socket type to define
  *     's' for stream
  *     'd' for datagram
  *  @param	bool ipv6: if we need a IPv6 socket
  *
 **/
Socket::Socket( char type, bool IPv6 ){

   this->CreateVSocket( type, IPv6 );

}


/**
  * Class destructor
  *
 **/
Socket::~Socket(){
    this->Close();
}


/**
  * Connect method
  *   use "connect" Unix system call
  *
  * @param	char * host: host address in dot notation, example "10.1.104.187"
  * @param	int port: process address, example 80
  *
 **/
int Socket::Connect( const char * host, int port ) {

  return this->MakeConnection( host, port );

}


/**
  * Read method
  *   use "read" Unix system call (man 3 read)
  *
  * @param	void * text: buffer to store data read from socket
  * @param	int size: buffer capacity, read will stop if buffer is full
  *
 **/
size_t Socket::Read( void * text, size_t size ) {
  size_t bytesRead = read(idSocket, text, size);
  if (bytesRead < 0) {
    throw std::runtime_error("Socket::Read() failed");
  }
  return bytesRead;
}


/**
  * Write method
  *   use "write" Unix system call (man 3 write)
  *
  * @param	void * buffer: buffer to store data write to socket
  * @param	size_t size: buffer capacity, number of bytes to write
  *
 **/
size_t Socket::Write( const void *text, size_t size ) {
  size_t bytesWritten = write(idSocket, text, size);
  if (bytesWritten < 0) {
    throw std::runtime_error("Socket::Write() failed");
  }
  return bytesWritten;

}


/**
  * Write method
  *
  * @param	char * text: string to store data write to socket
  *
  *  This method write a string to socket, use strlen to determine how many bytes
  *
 **/
size_t Socket::Write( const char *text ) {
  size_t length = strlen(text);
  size_t bytesWritten = write(idSocket, text, length);
  if (bytesWritten < 0) {
    throw std::runtime_error("Socket::Write(const char *) failed");
  }
  return bytesWritten;
}