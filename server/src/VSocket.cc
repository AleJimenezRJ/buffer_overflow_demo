/**
 *  Establece la definición de la clase Socket para efectuar la comunicación
 *  de procesos que no comparten memoria, utilizando un esquema de memoria
 *  distribuida.  El desarrollo de esta clase se hará en varias etapas, primero
 *  los métodos necesarios para los clientes, en la otras etapas los métodos para el servidor,
 *  manejo de IP-v6, conexiones seguras y otros
 *
 *  Universidad de Costa Rica
 *  ECCI
 *  CI0123 Proyecto integrador de redes y sistemas operativos
 *  2024-i
 *  Grupos: 1, 2
 *
 **/

#include <cstddef>
#include <stdexcept>
#include <cstdio>
#include <cstring>			// memset
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>			// ntohs
#include <unistd.h>			// close
//#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>			// getaddrinfo, freeaddrinfo

#include "VSocket.h"


/**
  *  Class initializer
  *     use Unix socket system call
  *
  *  @param     char t: socket type to define
  *     's' for stream
  *     'd' for datagram
  *  @param     bool ipv6: if we need a IPv6 socket
  *
 **/
void VSocket::InitVSocket( char t, bool IPv6 ){
   this->IPv6 = IPv6;  // Store the IPv6 flag
   int domain;
   if (IPv6) {
      domain = AF_INET6;
   } else {
      domain = AF_INET;
   }

   int type;
   if (t == 's') {
      type = SOCK_STREAM;
   } else if (t == 'd') {
      type = SOCK_DGRAM;
   } else {
      std::cerr << "Invalid socket type." << std::endl;
      exit(EXIT_FAILURE);
   }

   idSocket = socket(domain, type, 0);
   if (idSocket == -1) {
      perror("Socket creation failed");
      exit(EXIT_FAILURE);
   }
}


/**
  *  Class initializer
  *     use Unix socket system call
  *
  *  @param     int id: descriptor for an already opened socket (accept)
  *
 **/
void VSocket::InitVSocket( int id ){

   this->idSocket = id;

}


/**
  * Class destructor
  *
 **/
VSocket::~VSocket() {

   this->Close();

}


/**
  * Close method
  *    use Unix close system call (once opened a socket is managed like a file in Unix)
  *
 **/
void VSocket::Close(){
   int st;
   // using the Unix close function to attempt to close the created socket
   st = close(idSocket);
   if ( -1 == st ) {
      throw std::runtime_error( "Socket::Close()" );
   }
   idSocket = -1;

}


/**
  * DoConnect method
  *   use "connect" Unix system call
  *
  * @param      char * host: host address in dot notation, example "10.1.104.187"
  * @param      int port: process address, example 80
  *
 **/
int VSocket::DoConnect( const char * hostip, int port ) {
   int st;
   // for IPV6
   if (IPv6) {
      struct sockaddr_in6  host6;
      struct sockaddr * ha;

      memset( &host6, 0, sizeof( host6 ) );
      host6.sin6_family = AF_INET6;
      st = inet_pton( AF_INET6, hostip, &host6.sin6_addr );
      if ( 0 <= st ) {	// 0 means invalid address, -1 means address error
         throw std::runtime_error( "Socket::Connect( const char *, int ) [inet_pton]" );
      }
      host6.sin6_port = htons( port );
      ha = (struct sockaddr *) &host6;
      st = connect( this->idSocket, ha, sizeof( host6 ) );
      if ( -1 == st ) {
         throw std::runtime_error( "Socket::Connect( const char *, int ) [connect]" );
      }
   } else {    // for IPV4
      struct sockaddr_in host4;
      memset((char *) &host4, 0, sizeof(host4));
      host4.sin_family = AF_INET;
      st = inet_pton(AF_INET, hostip, &host4.sin_addr);
      if (st <= 0) {
         throw std::runtime_error("VSocket::DoConnect, inet_pton error");
      }
      host4.sin_port = htons(port);
      st = connect(idSocket, (sockaddr *) &host4, sizeof(host4));
      if (st == -1) {
         throw std::runtime_error("VSocket::DoConnect, connection error here");
      }
   }
   return st;

}


/**
  * DoConnect method
  *   use "connect" Unix system call
  *
  * @param      char * host: host address in dns notation, example "os.ecci.ucr.ac.cr"
  * @param      char * service: process address, example "http"
  *
 **/
int VSocket::DoConnect( const char *host, const char *service ) {
   struct addrinfo hints, *result, *rp;
   int st;
   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
   hints.ai_socktype = SOCK_STREAM; /* Stream socket */
   hints.ai_flags = 0;
   hints.ai_protocol = 0;          /* Any protocol */

   st = getaddrinfo( host, service, &hints, &result );
   if (st != 0) {
      throw std::runtime_error("VSocket::MakeConnection: getaddrinfo failed");
   }
   for ( rp = result; rp; rp = rp->ai_next ) {
      st = connect( idSocket, rp->ai_addr, rp->ai_addrlen );
      if ( 0 == st ) {
         break;
      }
   }
   freeaddrinfo( result );
   if ( -1 == st ) {
      perror( "VSocket::connect" );
      throw std::runtime_error( "VSocket::MakeConnection" );
   }
   return st;
}


/**
  * Listen method
  *
  * @param      int queue: max pending connections to enqueue (server mode)
  *
  *  This method define how many elements can wait in queue by making use of the syscall listen(id, queue)
  *
 **/
int VSocket::Listen( int queue ) {
   int st = listen(idSocket, queue);
   if (st == -1) {
      perror("VSocket::Listen");
      throw std::runtime_error("VSocket::Listen() failed");
   }
   return st;

}

/**
  * Bind method
  *    use "bind" Unix system call (man 3 bind) (server mode)
  *
  * @param      int port: bind a unamed socket to a port defined in sockaddr structure
  *
  *  Links the calling process to a service at port
  *
 **/
int VSocket::Bind( int port ) {
   int st;
   struct sockaddr_in server_addr;
   server_addr.sin_family = AF_INET;	// Definimos la familia para IPv4
   server_addr.sin_addr.s_addr = htonl( INADDR_ANY );	// Establecemos cualquier dirección
   server_addr.sin_port = htons(port);	// El puerto asociado al servicio
   st = bind( idSocket, (const sockaddr *) & server_addr, sizeof( server_addr ));
   return 0;
}


/**
  * DoAccept method
  *    use "accept" Unix system call (man 3 accept) (server mode)
  *
  *  @returns   a new class instance
  *
  *  Waits for a new connection to service (TCP mode: stream)
  *
 **/
int VSocket::DoAccept() {
   struct sockaddr_in client_addr;
   socklen_t addr_len = sizeof(client_addr);
   
   // Using the accept system call to accept incoming connections
   int client_socket = accept(idSocket, (struct sockaddr *) &client_addr, &addr_len);
   
   if (client_socket == -1) {
      perror("VSocket::DoAccept accept failed");
      throw std::runtime_error("VSocket::DoAccept() failed");
   }
   
   return client_socket;  // Returning the new socket descriptor for the accepted connection
}


/**
  * Shutdown method
  *    use "shutdown" Unix system call (man 3 shutdown)
  *
  *  @param	int mode define how to cease socket operation
  *
  *  Partial close the connection (TCP mode)
  *
 **/
int VSocket::Shutdown( int mode ) {
   int st = shutdown(idSocket, mode);
   if (st == -1) {
   perror("VSocket::Shutdown");
   throw std::runtime_error("VSocket::Shutdown() failed");
   }
   return st;
}


/**
  *  sendTo method
  *
  *  @param	const void * buffer: data to send
  *  @param	size_t size data size to send
  *  @param	void * addr address to send data
  *
  *  Send data to another network point (addr) without connection (Datagram)
  *
 **/
size_t VSocket::sendTo( const void * buffer, size_t size, void * addr ) {
   struct sockaddr_in *dest_addr = (struct sockaddr_in *)addr;

   size_t sent_size = sendto(idSocket, buffer, size, 0, (struct sockaddr *)dest_addr, sizeof(*dest_addr));
   if (sent_size < 0) {
      perror("sendTo failed");
      return 0;
   }
   return sent_size;
}


/**
  *  recvFrom method
  *
  *  @param	const void * buffer: data to send
  *  @param	size_t size data size to send
  *  @param	void * addr address to receive from data
  *
  *  @return	size_t bytes received
  *
  *  Receive data from another network point (addr) without connection (Datagram)
  *
 **/
size_t VSocket::recvFrom( void * buffer, size_t size, void * addr ) {
   if(IPv6){
      socklen_t addrLen;
      struct sockaddr_in6* addr6 = (struct sockaddr_in6*)addr;
      addrLen = sizeof(*addr6);
      size_t received_size = recvfrom(idSocket, buffer, size, 0, (struct sockaddr*)addr6, &addrLen);
      if (received_size == -1) {
         perror("recvFrom failed");
         throw std::runtime_error("VSocket::recvFrom");
      }
      return received_size;
   } else {
      struct sockaddr_in *src_addr = (struct sockaddr_in *)addr;
      socklen_t addr_len = sizeof(*src_addr);
      size_t received_size = recvfrom(idSocket, buffer, size, 0, (struct sockaddr *)src_addr, &addr_len);
      if (received_size < 0) {
         perror("recvFrom failed");
         throw std::runtime_error("VSocket::recvFrom");
      }
      return received_size;
   }
}

