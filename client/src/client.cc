#include <iostream>
#include <cstring>
#include <regex>
#include "client.h"

#define MAXBUFFER 4096  // 256 * 16

// Function to remove HTML tags and insert newlines where necessary
std::string removeHtmlTags(const std::string& html) {
  std::string cleanText;
  bool insideTag = false;

  for (size_t i = 0; i < html.size(); ++i) {
    if (html[i] == '<') {
      insideTag = true;
      // Check if it's a tag that requires a new line or space
      if (html.substr(i, 4) == "<li>" || html.substr(i, 3) == "<p" || html.substr(i, 4) == "<br>") {
        cleanText += "\n";  // Add a newline for these tags
      }
    } else if (html[i] == '>') {
      insideTag = false;
    } else if (!insideTag) {
      cleanText += html[i];  // Only add characters that are not inside a tag
    }
  }

  return cleanText;
}

// Function to clean HTTP headers from the server's response and return the content
std::string cleanBuffer(const std::string& buffer) {
  const char* start = buffer.c_str();
  const char* headerEnd = strstr(start, "\r\n\r\n");  // Find the end of the headers

  std::string cleanedText;
  if (headerEnd != nullptr) {
    start = headerEnd + 4;  // Skip past the HTTP headers
    cleanedText = std::string(start);  // Extract the content part
  }

  return cleanedText;
}

int client::clientRequest(int argc, char *argv[]) {
  VSocket *client;
  int st, port = 8080;  // TCP Port 8080
  char buffer[MAXBUFFER];
  // Default to localhost; allow overriding via argv[1]
  std::string serverIPStr = "127.0.0.1";
  bool crashNow = false;
  size_t payloadLen = 1000;

  // Naive argument parsing:
  // - first non-flag arg is IP
  // - --crash triggers immediate overflow demo
  // - --len N or --len=N sets payload length
  for (int i = 1; i < argc; ++i) {
    std::string arg(argv[i]);
    if (arg.rfind("--", 0) == 0) {
      if (arg == "--crash") {
        crashNow = true;
      } else if (arg == "--len" && i + 1 < argc) {
        payloadLen = static_cast<size_t>(std::stoul(argv[++i]));
      } else if (arg.rfind("--len=", 0) == 0) {
        payloadLen = static_cast<size_t>(std::stoul(arg.substr(6)));
      }
    } else if (serverIPStr == "127.0.0.1") {
      serverIPStr = arg; // first positional sets IP
    }
  }
  const char* serverIP = serverIPStr.c_str();

  memset(buffer, 0, MAXBUFFER);

  // Create socket and connect to the server
  client = new Socket('s');
  client->Connect(serverIP, port);
  std::cout << "Connected to server at " << serverIP << ":" << port << "\n";
  // If asked, run the crash demo immediately and exit
  if (crashNow) {
    std::string payload(payloadLen, 'A');
    std::string crashRequest =
      std::string("GET /menu HTTP/1.1\r\n") +
      "Host: " + serverIP + "\r\n" +
      "X-User: " + payload + "\r\n" +
      "Connection: close\r\n\r\n";

    std::cout << "[Crash] Connecting to " << serverIP << ":" << port
              << ", sending payload length=" << payloadLen << "...\n";
    client = new Socket('s');
    client->Connect(serverIP, port);
    client->Write((char *)crashRequest.c_str(), crashRequest.length());

    std::string response;
    while (true) {
      memset(buffer, 0, MAXBUFFER);
      st = client->Read(buffer, MAXBUFFER);
      if (st <= 0) break;
      response.append(buffer, st);
    }

    if (response.empty()) {
      std::cout << "[Crash] No response (expected if server crashed)." << std::endl;
    } else {
      std::cout << "[Crash] Got response (server may not have crashed):\n" << response << std::endl;
    }
    return 0;
  }

  // Build HTTP request to get the menu
  std::string menuRequest = "GET /menu HTTP/1.1\r\nHost: " + std::string(serverIP)
   + "\r\nConnection: close\r\n\r\n";
  client->Write((char *)menuRequest.c_str(), menuRequest.length());

  // Read the server's response
  std::string response;
  while (true) {
    memset(buffer, 0, MAXBUFFER);
    st = client->Read(buffer, MAXBUFFER);
    if (st <= 0) break;
    response.append(buffer, st);
  }

  // Clean the response to remove HTTP headers and HTML tags
  std::string content = cleanBuffer(response);  // Remove HTTP headers
  std::string cleanMenu = removeHtmlTags(content);  // Remove HTML tags and add newlines

  // Display the cleaned menu
  std::cout << cleanMenu << std::endl;

  // Ask the user to select a book by name or crash demo
  std::cout << "Choose an option:\n1. View a book\n2. Crash Demo (strcpy overflow)\nEnter 1 or 2: ";
  int option = 0;
  std::cin >> option;

  if (option == 1) {
    std::string bookTitle;
    std::cout << "Enter the name of the book you want to view: ";
    std::cin >> bookTitle;

    // Send the user's selection to the server
    std::string selectionRequest = "GET /book/" + bookTitle + " HTTP/1.1\r\nHost: " + std::string(serverIP) + "\r\nConnection: close\r\n\r\n";

    // Create a new socket for the selection
    client = new Socket('s');
    client->Connect(serverIP, port);
    client->Write((char *)selectionRequest.c_str(), selectionRequest.length());

    // Read the server's response for the selected book
    response.clear();
    while (true) {
      memset(buffer, 0, MAXBUFFER);
      st = client->Read(buffer, MAXBUFFER);
      if (st <= 0) break;
      response.append(buffer, st);
    }

    // Check for HTTP status code in the response
    if (response.find("HTTP/1.1 404 Not Found") != std::string::npos) {
      std::cout << "Error: The book could not be found in the book server." << std::endl;
    } else if (response.find("HTTP/1.1 400 Bad Request") != std::string::npos) {
      std::cout << "Error: Bad request. Please check your input." << std::endl;
    } else {
      // Clean the response
      content = cleanBuffer(response);
      std::cout << "Content of the selected book: \n" << std::endl;
      std::cout << content << std::endl;
    }
  } else if (option == 2) {
  // Crash Demo: trigger stack buffer overflow via long X-User header
    std::string payload(payloadLen, 'A');
    std::string crashRequest =
      std::string("GET /menu HTTP/1.1\r\n") +
      "Host: " + serverIP + "\r\n" +
      "X-User: " + payload + "\r\n" +
      "Connection: close\r\n\r\n";

    std::cout << "Sending overflow payload of length " << payloadLen << " via X-User header...\n";
    client = new Socket('s');
    client->Connect(serverIP, port);
    client->Write((char *)crashRequest.c_str(), crashRequest.length());

    response.clear();
    while (true) {
      memset(buffer, 0, MAXBUFFER);
      st = client->Read(buffer, MAXBUFFER);
      if (st <= 0) break;
      response.append(buffer, st);
    }

    if (response.empty()) {
      std::cout << "No response received. The server likely crashed due to strcpy overflow." << std::endl;
    } else {
      std::cout << "Response received (server may have survived this run):\n" << response << std::endl;
    }
  }

  return 0;
}
