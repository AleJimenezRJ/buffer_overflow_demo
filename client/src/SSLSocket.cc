
/**
  *  Socket class implementation
  *
 **/
 
// SSL includes
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <iostream>
#include <stdexcept>

#include "SSLSocket.h"
#include "Socket.h"

/**
  *  Class constructor
  *     use base class
  *
  *  @param     char t: socket type to define
  *     's' for stream
  *     'd' for datagram
  *  @param     bool ipv6: if we need a IPv6 socket
  *
 **/
SSLSocket::SSLSocket( bool IPv6 ) {

   this->CreateVSocket( 's', IPv6 );

   this->SSLContext = nullptr;
   this->SSLStruct = nullptr;

   this->Init();					// Initializes to client context

}


/**
  *  Class constructor
  *     use base class
  *
  *  @param     char t: socket type to define
  *     's' for stream
  *     'd' for datagram
  *  @param     bool IPv6: if we need a IPv6 socket
  *
 **/
SSLSocket::SSLSocket( char * certFileName, char * keyFileName, bool IPv6 ) {
   this->CreateVSocket('s', IPv6);

    this->SSLContext = nullptr;
    this->SSLStruct = nullptr;

    this->Init(); // Initializes to client context
    this->LoadCertificates(certFileName, keyFileName);
}


/**
  *  Class constructor
  *
  *  @param     int id: socket descriptor
  *
 **/
SSLSocket::SSLSocket( int id ) {

   this->CreateVSocket( id );
   this->SSLContext = nullptr;
   this->SSLStruct = nullptr;

   this->Init(); // Initializes to client context

}


/**
  * Class destructor
  *
 **/
SSLSocket::~SSLSocket() {

// SSL destroy
   if ( nullptr != this->SSLContext ) {
      SSL_CTX_free( reinterpret_cast<SSL_CTX *>( this->SSLContext ) );
   }
   if ( nullptr != this->SSLStruct ) {
      SSL_free( reinterpret_cast<SSL *>( this->SSLStruct ) );
   }

   this->Close();

}


/**
  *  SSLInit
  *     use SSL_new with a defined context
  *
  *  Create a SSL object
  *
 **/
void SSLSocket::Init( bool serverContext ) {
   this->InitContext(serverContext);

   SSL *ssl = SSL_new(reinterpret_cast<SSL_CTX *>(this->SSLContext));
   if (!ssl) {
      throw std::runtime_error("SSLSocket::Init() - Failed to create SSL structure.");
   }

   this->SSLStruct = reinterpret_cast<void *>(ssl);

}


/**
  *  InitContext
  *     use SSL_library_init, OpenSSL_add_all_algorithms, SSL_load_error_strings, TLS_server_method, SSL_CTX_new
  *
  *  Creates a new SSL server context to start encrypted comunications, this context is stored in class instance
  *
 **/
void SSLSocket::InitContext( bool serverContext ) {
   SSL_library_init();
   OpenSSL_add_all_algorithms();
   SSL_load_error_strings();

   const SSL_METHOD *method;
   if (serverContext) {
      method = TLS_server_method();
   } else {
      method = TLS_client_method();
   }

   if (!method) {
      throw std::runtime_error("SSLSocket::InitContext() - Unable to create SSL method.");
   }

   SSL_CTX *context = SSL_CTX_new(method);
   if (!context) {
      throw std::runtime_error("SSLSocket::InitContext() - Unable to create SSL context.");
   }

   this->SSLContext = reinterpret_cast<void *>(context);

}


/**
 *  Load certificates
 *    verify and load certificates
 *
 *  @param	const char * certFileName, file containing certificate
 *  @param	const char * keyFileName, file containing keys
 *
 **/
 void SSLSocket::LoadCertificates( const char * certFileName, const char * keyFileName ) {
   SSL_CTX *context = reinterpret_cast<SSL_CTX *>(this->SSLContext);

   if (SSL_CTX_use_certificate_file(context, certFileName, SSL_FILETYPE_PEM) <= 0) {
      ERR_print_errors_fp(stderr);
      throw std::runtime_error("SSLSocket::LoadCertificates() - Failed to load certificate.");
   }

   if (SSL_CTX_use_PrivateKey_file(context, keyFileName, SSL_FILETYPE_PEM) <= 0) {
      ERR_print_errors_fp(stderr);
      throw std::runtime_error("SSLSocket::LoadCertificates() - Failed to load private key.");
   }

   if (!SSL_CTX_check_private_key(context)) {
      throw std::runtime_error("SSLSocket::LoadCertificates() - Private key does not match the public certificate.");
   }
}
 

/**
 *  Connect
 *     use SSL_connect to establish a secure conection
 *
 *  Create a SSL connection
 *
 *  @param	char * hostName, host name
 *  @param	int port, service number
 *
 **/
int SSLSocket::Connect( const char * hostName, int port ) {
   int st = this->MakeConnection(hostName, port);  // Establish a non-SSL connection first
   if (st < 0) {
      std::cout << "Error in SSL connect" <<std::endl;
      return st;
   }

   SSL *ssl = reinterpret_cast<SSL *>(this->SSLStruct);
   SSL_set_fd(ssl, idSocket);  // Set the file descriptor for SSL

   if (SSL_connect(ssl) <= 0) {  // Establish the SSL connection
      ERR_print_errors_fp(stderr);
      return -1;
   }

   return 0;

}


/**
 *  Connect
 *     use SSL_connect to establish a secure conection
 *
 *  Create a SSL connection
 *
 *  @param	char * hostName, host name
 *  @param	char * service, service name
 *
 **/
int SSLSocket::Connect( const char * host, const char * service ) {
   int st;

   st = this->MakeConnection( host, service );

   return st;

}


/**
  *  Read
  *     use SSL_read to read data from an encrypted channel
  *
  *  @param	void * buffer to store data read
  *  @param	size_t size, buffer's capacity
  *
  *  @return	size_t byte quantity read
  *
  *  Reads data from secure channel
  *
 **/
size_t SSLSocket::Read( void * buffer, size_t size ) {
   SSL *ssl = reinterpret_cast<SSL *>(this->SSLStruct);
   size_t st = SSL_read(ssl, buffer, size);
   if (st <= 0) {
      throw std::runtime_error("SSLSocket::Read() - Error reading from SSL connection.");
   }

   return st;

}


/**
  *  Write
  *     use SSL_write to write data to an encrypted channel
  *
  *  @param	void * buffer to store data read
  *  @param	size_t size, buffer's capacity
  *
  *  @return	size_t byte quantity written
  *
  *  Writes data to a secure channel
  *
 **/
size_t SSLSocket::Write( const char * string ) {
   return this->Write(reinterpret_cast<const void *>(string), strlen(string));
}


/**
  *  Write
  *     use SSL_write to write data to an encrypted channel
  *
  *  @param	void * buffer to store data read
  *  @param	size_t size, buffer's capacity
  *
  *  @return	size_t byte quantity written
  *
  *  Reads data from secure channel
  *
 **/
size_t SSLSocket::Write( const void * buffer, size_t size ) {
   SSL *ssl = reinterpret_cast<SSL *>(this->SSLStruct);
   size_t st = SSL_write(ssl, buffer, size);
   if (st <= 0) {
      throw std::runtime_error("SSLSocket::Write() - Error writing to SSL connection.");
   }
   return st;
}


/**
 *   Show SSL certificates
 *
 **/
void SSLSocket::ShowCerts() {
   X509 *cert;
   char *line;

   cert = SSL_get_peer_certificate( (SSL *) this->SSLStruct );		 // Get certificates (if available)
   if ( nullptr != cert ) {
      printf("Server certificates:\n");
      line = X509_NAME_oneline( X509_get_subject_name( cert ), 0, 0 );
      printf( "Subject: %s\n", line );
      free( line );
      line = X509_NAME_oneline( X509_get_issuer_name( cert ), 0, 0 );
      printf( "Issuer: %s\n", line );
      free( line );
      X509_free( cert );
   } else {
      printf( "No certificates.\n" );
   }

}


/**
 *   Return the name of the currently used cipher
 *
 **/
const char * SSLSocket::GetCipher() {

   return SSL_get_cipher( reinterpret_cast<SSL *>( this->SSLStruct ) );

}

