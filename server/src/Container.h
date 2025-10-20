#ifndef CONTAINER_H
#define CONTAINER_H

#include <string>
#include <vector>
#include <filesystem>

/**
 * @brief Manages a collection of books.
 *
 * The Container class provides functionality to create, retrieve, 
 * upload, and manage books. It handles the storage and organization 
 * of book content, allowing users to interact with their book collection.
 */
class Container {
public:
  /**
   * @brief Constructor for the Container class.
   *
   * Initializes a new instance of the Container class.
   */
  Container();  // Constructor

  /**
   * @brief Creates a new book with the given title and content.
   *
   * @param title The title of the book.
   * @param content The content of the book.
   */
  void createBook(const std::string& title, const std::string& content);

  /**
   * @brief Retrieves the content of a book by its title.
   *
   * @param title The title of the book to retrieve.
   * @return The content of the book as a string.
   */
  std::string getBook(const std::string& title);

  /**
   * @brief Gets the content of a book from a specified file path.
   *
   * @param filePath The path to the book file.
   * @return The content of the book as a string.
   */
  std::string getBookContent(const std::string& filePath);

  /**
   * @brief Uploads a book from the specified file path.
   *
   * This function reads the content of a book from the file path and 
   * stores it in the container.
   *
   * @param filePath The path to the book file to upload.
   */
  void uploadBook(std::string& filePath);

  /**
   * @brief Prints a table of available books.
   *
   * This function displays a list of all books in the container 
   * in a formatted table.
   */
  void printTable();

  /**
   * @brief Displays information about the books in the container.
   *
   * This function provides details about the books, such as 
   * the number of books, their titles, and other relevant information.
   */
  void showInfo();

private:
  /**
   * @brief Reconstructs the book content from its title.
   *
   * @param title The title of the book to reconstruct.
   * @return The reconstructed book content as a string.
   */
  std::string reconstructBook(const std::string& title);

  const size_t fragmentSize = 256;  ///< Size of each fragment for book storage.
};

#endif
