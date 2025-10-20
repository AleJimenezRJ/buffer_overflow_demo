#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <string>
#include <iomanip> 
#include "Container.h"

#define FRAGMENT_SIZE 256  ///< The defined size of each book part

/**
 * @brief Constructs a new Container instance.
 *
 * Initializes a new Container for managing books. This constructor 
 * does not perform any specific actions.
 */
Container::Container() {
}

/**
 * @brief Creates a new book by splitting its content into parts and saving them as files.
 *
 * This function takes the title and content of a book, splits the content 
 * into fragments of a specified size (FRAGMENT_SIZE), and saves each fragment 
 * in a separate file within a directory named after the book's title.
 *
 * @param title The title of the book.
 * @param content The content of the book to be saved.
 */
void Container::createBook(const std::string& title, const std::string& content) {
  std::string bookDir = "books/" + title;
  std::filesystem::create_directory(bookDir);  // Create directory for the book
  std::ofstream partFile;

  std::vector<std::string> parts;
  for (size_t i = 0; i < content.size(); i += FRAGMENT_SIZE) {
    parts.push_back(content.substr(i, FRAGMENT_SIZE));  // Split into fragments
  }

  // Save each part with the format "Title.partXX.txt"
  for (size_t i = 0; i < parts.size(); i++) {
    std::ostringstream partFileName;
    partFileName << bookDir << "/" << title << ".part" << std::setw(2) << std::setfill('0') << i << ".txt";
    partFile.open(partFileName.str());
    partFile << parts[i];  // Write the part to file
    partFile.close();
  }
}

/**
 * @brief Retrieves the content of a book by reconstructing it from its parts.
 *
 * This function checks if a book exists in the directory and reconstructs 
 * the book content from its saved parts.
 *
 * @param title The title of the book to retrieve.
 * @return The content of the book as a string, or "Book not found" if it doesn't exist.
 */
std::string Container::getBook(const std::string& title) {
  std::string bookDir = "books/" + title;
  if (!std::filesystem::exists(bookDir)) {
    return "Book not found";  // Return message if book directory doesn't exist
  }
  return reconstructBook(title);  // Reconstruct the book content
}

/**
 * @brief Reads the content of a single part file.
 *
 * This function opens the specified file and reads its content into a string.
 *
 * @param filePath The path to the book part file.
 * @return The content of the file as a string, or an empty string if the file cannot be opened.
 */
std::string Container::getBookContent(const std::string& filePath) {
  std::ifstream file(filePath);
  if (!file.is_open()) {
    return "";  // Return empty string if file cannot be opened
  }
  std::stringstream content;
  content << file.rdbuf();  // Read the entire content of the file
  file.close();
  return content.str();
}

/**
 * @brief Reconstructs the full book from its parts.
 *
 * This function reads all the saved parts of a book in order, 
 * appending their contents together to form the full book content.
 *
 * @param title The title of the book to reconstruct.
 * @return The full content of the book as a string.
 */
std::string Container::reconstructBook(const std::string& title) {
  std::string bookDir = "books/" + title;
  std::stringstream content;
  size_t partNumber = 0;

  // Loop to read all parts in order
  while (true) {
    std::ostringstream partFileName;
    partFileName << bookDir << "/" << title << ".part" << std::setw(2) << std::setfill('0') << partNumber << ".txt";
    std::ifstream partFile(partFileName.str());

    if (!partFile.is_open()) {
      break;  // Stop if the part file does not exist
    }

    content << partFile.rdbuf();  // Append the content of the current part
    partFile.close();
    partNumber++;
  }

  return content.str();  // Return the full reconstructed book content
}

/**
 * @brief Prints the titles of available books in the library.
 *
 * This function lists all directories in the "books" directory,
 * which represent the titles of books in the library.
 */
void Container::printTable() {
  std::cout << "Books in Library:\n";
  for (const auto& entry : std::filesystem::directory_iterator("books")) {
    if (entry.is_directory()) {
      std::cout << entry.path().filename().string() << std::endl;  // Print book title
    }
  }
}

/**
 * @brief Displays information about the number of books and their fragments in the library.
 *
 * This function counts and displays the total number of books 
 * and the total number of fragments (parts) stored in the library.
 */
void Container::showInfo() {
  int numberOfBooks = 0;
  int numberOfFragments = 0;
  for (const auto& entry : std::filesystem::directory_iterator("books")) {
    if (entry.is_directory()) {
      numberOfBooks++;
      for (const auto& subentry : std::filesystem::directory_iterator(entry.path())) {
        numberOfFragments++;  // Count fragments for each book
      }
    }
  }
  std::cout << "Number of Books: " << numberOfBooks << "\n";
  std::cout << "Number of Fragments: " << numberOfFragments << "\n";
}
