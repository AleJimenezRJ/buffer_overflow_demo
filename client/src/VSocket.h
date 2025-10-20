/**
 * @file VSocket.h
 * @brief Declaration of the VSocket class, an abstract base class for handling socket operations.
 */

/**
 *  Universidad de Costa Rica
 *  ECCI
 *  CI0123 Proyecto integrador de redes y sistemas operativos
 *  2024-ii
 *  Grupos: 3 y 5
 *
 * (versión Fedora)
 *
 **/


#ifndef VSocket_h
#define VSocket_h

/**
 * @class VSocket
 * @brief An abstract base class for creating and managing socket connections.
 *
 * The VSocket class provides a common interface for creating, connecting, reading from, and writing to sockets.
 * It also includes functions for binding sockets and handling data transmission using various protocols.
 */
class VSocket {
   public:
    /**
     * @brief Creates a virtual socket with specified options.
     *
     * @param socketType A character representing the type of socket to create (e.g., 's' for stream, 'd' for datagram).
     * @param useIPv6 A boolean indicating whether to use IPv6. Defaults to false (IPv4).
     */
    void CreateVSocket(char socketType, bool useIPv6 = false);

    /**
     * @brief Creates a virtual socket with a specified socket identifier.
     *
     * @param socketID An integer representing an existing socket identifier.
     */
    void CreateVSocket(int socketID);

    /**
     * @brief Destructor for the VSocket class.
     *
     * Closes the socket if it is still open and performs any necessary cleanup.
     */
    ~VSocket();

    /**
     * @brief Closes the socket.
     */
    void Close();

    /**
     * @brief Establishes a connection to a remote host using an IP address and port number.
     *
     * @param ipAddress A pointer to a string containing the IP address of the remote host.
     * @param port An integer representing the port number on the remote host.
     * @return An integer indicating success (0) or failure (non-zero).
     */
    int MakeConnection(const char* ipAddress, int port);

    /**
     * @brief Establishes a connection to a remote host using a hostname and service name.
     *
     * @param hostname A pointer to a string containing the hostname of the remote host.
     * @param serviceName A pointer to a string containing the service name (e.g., "http", "ftp").
     * @return An integer indicating success (0) or failure (non-zero).
     */
    int MakeConnection(const char* hostname, const char* serviceName);

    /**
     * @brief Pure virtual function to connect the socket to a remote address.
     *
     * Must be implemented by derived classes.
     *
     * @param address A pointer to a string containing the remote address.
     * @param port An integer representing the port number to connect to.
     * @return An integer indicating success (0) or failure (non-zero).
     */
    virtual int Connect(const char* address, int port) = 0;

    /**
     * @brief Pure virtual function to read data from the socket.
     *
     * Must be implemented by derived classes.
     *
     * @param buffer A pointer to a buffer where the read data will be stored.
     * @param size The maximum number of bytes to read.
     * @return The number of bytes actually read.
     */
    virtual size_t Read(void* buffer, size_t size) = 0;

    /**
     * @brief Pure virtual function to write data to the socket.
     *
     * Must be implemented by derived classes.
     *
     * @param buffer A pointer to the data to be written.
     * @param size The number of bytes to write.
     * @return The number of bytes actually written.
     */
    virtual size_t Write(const void* buffer, size_t size) = 0;

    /**
     * @brief Writes a null-terminated string to the socket.
     *
     * @param str A pointer to a null-terminated string to be written.
     * @return The number of bytes actually written.
     */
    virtual size_t Write(const char* str) = 0;

    /**
     * @brief Binds the socket to a specific port.
     *
     * @param port An integer representing the port number to bind to.
     * @return An integer indicating success (0) or failure (non-zero).
     */
    int Bind(int port);

    /**
     * @brief Sends data to a specific destination.
     *
     * @param buffer A pointer to the data to be sent.
     * @param size The number of bytes to send.
     * @param dest A pointer to a structure containing the destination address.
     * @return The number of bytes actually sent.
     */
    size_t sendTo(const void* buffer, size_t size, void* dest);

    /**
     * @brief Receives data from a specific source.
     *
     * @param buffer A pointer to a buffer where the received data will be stored.
     * @param size The maximum number of bytes to receive.
     * @param src A pointer to a structure containing the source address.
     * @return The number of bytes actually received.
     */
    size_t recvFrom(void* buffer, size_t size, void* src);


   protected:
      int idSocket;	// Socket identifier
      bool IPv6;	// Is IPv6 socket?
      int port;		// Socket associated port
        
};

#endif // VSocket_h


