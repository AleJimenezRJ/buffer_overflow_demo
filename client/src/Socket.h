/**
 * @file Socket.h
 * @brief Declaration of the Socket class, a concrete implementation of the VSocket class for handling standard socket operations.
 */
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
 *  2024-ii
 *  Grupo: 3 y 5
 *
 *
 **/

#ifndef Socket_h
#define Socket_h

#include "VSocket.h"

/**
 * @class Socket
 * @brief A class that provides a concrete implementation of the VSocket interface for standard socket operations.
 *
 * The Socket class implements methods for connecting to a server, reading from, and writing to a socket.
 * It extends the VSocket class and provides specific behavior for these operations.
 */
class Socket : public VSocket {

   public:
      /**
     * @brief Constructs a Socket object with specified type and protocol.
     *
     * @param socketType A character representing the type of socket to create (e.g., 's' for stream, 'd' for datagram).
     * @param useIPv6 A boolean indicating whether to use IPv6. Defaults to false (IPv4).
     */
      Socket( char, bool = false );
      /**
     * @brief Destructor for the Socket class.
     *
     * Closes the socket and performs any necessary cleanup.
     */
      ~Socket();
      /**
     * @brief Connects the socket to a remote address using an IP address and port number.
     *
     * @param address A pointer to a string containing the remote IP address.
     * @param port An integer representing the port number to connect to.
     * @return An integer indicating success (0) or failure (non-zero).
     */
      int Connect( const char *, int );

    /**
     * @brief Reads data from the socket.
     *
     * @param buffer A pointer to a buffer where the read data will be stored.
     * @param size The maximum number of bytes to read.
     * @return The number of bytes actually read.
     */
      size_t Read( void *, size_t );

      /**
     * @brief Writes data to the socket.
     *
     * @param buffer A pointer to the data to be written.
     * @param size The number of bytes to write.
     * @return The number of bytes actually written.
     */
      size_t Write( const void *, size_t );
      
      /**
     * @brief Writes a null-terminated string to the socket.
     *
     * @param str A pointer to a null-terminated string to be written.
     * @return The number of bytes actually written.
     */
      size_t Write( const char * );

   protected:

};

#endif

