/**
 * @file client.h
 * @brief Declaration of the client class for handling book requests from a server.
 */

#include <regex.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <regex>
#include <cstddef>
#include <string>
#include "VSocket.h"
#include "Socket.h"
#include "SSLSocket.h"

#ifndef CLIENT_H
#define CLIENT_H

/**
 * @brief Removes HTML tags from the input string and inserts newlines for specific tags.
 * 
 * This function scans through the provided HTML string, removing all HTML tags
 * while preserving the text content. It also adds newlines for certain tags such
 * as <li>, <p>, and <br> to format the output text properly.
 * 
 * @param html The input HTML string to be cleaned.
 * @return A string containing the cleaned text without HTML tags.
 */
std::string removeHtmlTags(const std::string& html);

/**
 * @brief Cleans the HTTP headers from the server's response.
 * 
 * This function takes the raw server response and locates the end of the HTTP
 * headers. It then extracts and returns the content portion of the response,
 * effectively discarding the headers.
 * 
 * @param buffer The raw server response containing HTTP headers and content.
 * @return A string containing only the content part of the response.
 */
std::string cleanBuffer(const std::string& buffer);

/**
 * @class client
 * @brief Handles client requests to the server.
 * 
 * This class encapsulates the functionality for creating a client socket,
 * sending requests to a server, and processing the server's responses.
 */
class client {
 public:
  /**
   * @brief Sends a request to the server and processes the response.
   * 
   * This function initiates a connection to a server, requests the menu,
   * and processes the server's response. It also allows the user to select
   * an option from the menu and sends the selection back to the server,
   * displaying the final response.
   * 
   * @param argc The number of command-line arguments.
   * @param argv The command-line arguments.
   * @return An integer indicating the success or failure of the operation.
   */
  int clientRequest(int argc, char *argv[]);
};

#endif // CLIENT_H