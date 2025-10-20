/**
 * @file main.cc
 * @brief Entry point of the application to request and display a book's content from a server.
 */

#include "client.h"

/**
 * @brief Main function that initiates the client request to fetch and display a book from the server.
 *
 * This function creates an instance of the `client` class and calls its `clientRequest` method
 * to handle the process of requesting and displaying the content of a book.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return An integer representing the exit status of the program. Returns 0 on successful execution.
 */
int main(int argc, char* argv[]) {
  std::cout << "Starting Client Application...\n";
  client clientRequest;
  clientRequest.clientRequest(argc, argv);
  return 0;
}
