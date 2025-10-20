#include <cstring>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
#include <filesystem>
#include <unordered_map>

#include "Socket.h"
#include "Container.h"

#define PORT 8080
#define BUFSIZE 4096

Container bookContainer;

// Helper function with hidden buffer overflow vulnerability
void processUserInput(const char* username, VSocket* client) {
    char userBuffer[32];
    bool isPrivileged = false;
    const char* secret = "DEMO_SECRET: pa55w0rd!";

    // Unsafe copy, allows buffer overflow to overwrite isPrivileged
    strcpy(userBuffer, username); // Vulnerable!

    // Only leak secret if privilege is (incorrectly) granted
    if (isPrivileged) {
        std::string httpResponse = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(strlen(secret)) + "\r\nContent-Type: text/plain; charset=UTF-8\r\nConnection: close\r\n\r\n";
        client->Write(httpResponse.c_str());
        client->Write(secret);
        std::cout << "[!] Secret leaked due to buffer overflow!\n";
    }
}

/**
 * @brief Handles the client's request and sends an appropriate response.
 *
 * This function processes incoming HTTP requests from the client. It supports
 * two types of requests: listing available books (GET /menu) and serving the 
 * content of a specific book (GET /book/{title}). It will send an HTTP response 
 * with the requested information or an error if the request is invalid.
 *
 * @param client A pointer to the VSocket object representing the connected client.
 */
void task(VSocket *client) {
  std::cout << "Answering request from client\n";
  char buffer[BUFSIZE];
  memset(buffer, 0, sizeof(buffer));

  // Read the client request
  client->Read(buffer, BUFSIZE);
  std::cout << "Server received request:\n" << buffer << "\n";

  // Parse a custom header or username from the request
  std::string request(buffer);
  std::string method, path, username;
  std::istringstream iss(request);
  iss >> method >> path >> username;

  // Call the helper with the username (could be empty if not provided)
  processUserInput(username.c_str(), client);


  std::cout << "Method: " << method << ", Path: " << path << "\n";

  // Handle the menu request (GET /menu)
  if (method == "GET" && path == "/menu") {
    std::ostringstream menuStream;
    menuStream << "<html><body><h1>Book Menu</h1><ul>";

    // List available books from the "books" directory
    for (const auto& entry : std::filesystem::directory_iterator("books")) {
      if (entry.is_directory()) {
        std::string bookTitle = entry.path().filename().string();
        std::cout << "Found book: " << bookTitle << "\n";  // Log available books
        menuStream << "<li><a href=\"/book/" << bookTitle << "\">" << bookTitle << "</a></li>";
      }
    }

    menuStream << "</ul></body></html>";
    std::string menu = menuStream.str();

    // Prepare the HTTP response
    std::string httpResponse = "HTTP/1.1 200 OK\r\nContent-Length: " + 
                               std::to_string(menu.length()) + 
                               "\r\nContent-Type: text/html; charset=UTF-8\r\nConnection: close\r\n\r\n";

    // Send the response to the client
    client->Write(httpResponse.c_str());
    client->Write(menu.c_str());

    std::cout << "Sent book menu to client\n";

  } else if (method == "GET" && path.rfind("/book/", 0) == 0) {
    // Handle book content request (GET /book/{title})
    std::string bookTitle = path.substr(6);  // Extract book title from the path
    std::cout << "Client requested book: " << bookTitle << "\n";

    std::string bookContent = bookContainer.getBook(bookTitle);

    if (bookContent == "Book not found") {
      // If the book was not found, return 404
      std::string httpResponse = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
      client->Write(httpResponse.c_str());
      std::cout << "Book not found: " << bookTitle << "\n";
    } else {
      // If the book was found, return the book content
      std::string httpResponse = "HTTP/1.1 200 OK\r\nContent-Length: " + 
                                 std::to_string(bookContent.length()) + 
                                 "\r\nContent-Type: text/plain; charset=UTF-8\r\nConnection: close\r\n\r\n";
      client->Write(httpResponse.c_str());
      client->Write(bookContent.c_str());

      std::cout << "Sent book content to client for book: " << bookTitle << "\n";
    }

  } else {
    // Handle invalid requests
    std::string httpResponse = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
    client->Write(httpResponse.c_str());
    std::cout << "Invalid request received: " << path << "\n";
  }

  // After sending the response, close the connection
  client->Close();
  std::cout << "Request handled, connection closed\n";
}

/**
 * @brief Main server function that listens for incoming connections and handles them in separate threads.
 *
 * This function starts the server on a specified port (8080). It continuously accepts incoming client connections,
 * and for each connection, it spawns a new thread to handle the client's request using the `task()` function.
 *
 * @param argc Number of command line arguments.
 * @param argv Array of command line arguments.
 * @return int Return code (0 for success).
 */
int main(int argc, char **argv) {
  std::thread *worker;
  VSocket *s1, *client;
  Container bookContainer;

  // Ensure the books directory exists before starting the server
  std::string booksDir = "books";
  if (!std::filesystem::exists(booksDir)) {
    std::filesystem::create_directory(booksDir);  // Create directory if it doesn't exist
    std::cout << "Created books directory as it did not exist.\n";
  }

  s1 = new Socket('s');

  s1->Bind(PORT);  // Bind to port 8080
  s1->Listen(5);   // Set backlog queue to 5 connections
  std::cout << "Server started on port " << PORT << ". Waiting for connections...\n";

  // Main server loop, accepting incoming client connections
  for (;;) {
    client = s1->Accept();  // Wait for a client connection
    std::cout << "Accepted a new client connection.\n";
    worker = new std::thread(task, client);  // Spawn a new thread to handle the client
  }
}