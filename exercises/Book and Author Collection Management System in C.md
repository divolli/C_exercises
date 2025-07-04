## **Project Goal**

Create a system in C to manage a **collection of books and their associated authors**.  
The program will allow adding, deleting, modifying, and searching for both books and authors.  
This project is a great opportunity to work with:

- **Complex data relationships (many-to-many)**
    
- **Dynamic allocation of arrays**
    
- **Various sorting and searching algorithms**
    
- **Reading and writing binary files**
    

The program will be interactive with a console menu interface.

---

## **File Structure**

The project should be organized into the following files:

- **`errors.h`**: Definitions of error codes (you can reuse from previous projects).
    
- **`utils.h`**: Declarations of helper functions (e.g., for safe input reading, ID generation, managing dynamic pointer arrays).
    
- **`utils.c`**: Implementations of helper functions.
    
- **`authors.h`**: Definition of the `Author` structure and prototypes for managing the author list (doubly linked list).
    
- **`authors.c`**: Implementations of functions managing the `Author` list.
    
- **`books.h`**: Definition of the `Book` structure and prototypes for managing the book list (singly linked list).
    
- **`books.c`**: Implementations of functions managing the `Book` list.
    
- **`main.c`**: Main program logic, interactive menu, and command-line argument handling.

---

## 1. Error enumeration & helping functions

### File: `errors.h` 
```c
#ifndef ERRORS_H
#define ERRORS_H

// Definition error codes
typedef enum {
    SUCCESS = 0,
    ERROR_INVALID_ARGUMENT = 1,
    ERROR_MEMORY_ALLOCATION_FAILED = 2,
    ERROR_NOT_FOUND = 3,
    ERROR_DUPLICATE_ENTRY = 4,
    ERROR_FILE_OPERATION_FAILED = 5,
    ERROR_EMPTY_LIST = 6,
    ERROR_OTHER = 255
} ErrorCode;

#endif // ERRORS_H
```

### File: `utils.h`
```c
#ifndef UTILS_H
#define UTILS_H

#include "errors.h"
#include <stddef.h> // For size_t

// --- Comparison function types (for sorting/searching) ---

/**
 * @brief Pointer to a function comparing two strings (e.g. IDs, titles, names).
 * @return <0 if s1 < s2, 0 if s1 == s2, >0 if s1 > s2.
 */
typedef int (*StringCompareFunc)(const char *s1, const char *s2);

// --- Functions for handling dynamic pointer arrays ---

/**
 * @brief Adds an element to a dynamic pointer array.
 * Reallocates the array and copies the pointer.
 * @param array_ptr Pointer to the pointer to the start of the array.
 * @param count Pointer to the number of elements in the array.
 * @param element_ptr Pointer to the element to add.
 * @return SUCCESS or ERROR_MEMORY_ALLOCATION_FAILED.
 */
ErrorCode add_element_to_ptr_array(void ***array_ptr, size_t *count, void *element_ptr);

/**
 * @brief Removes an element from a dynamic pointer array.
 * Frees the memory pointed to by the element and reallocates the array.
 * @param array_ptr Pointer to the pointer to the start of the array.
 * @param count Pointer to the number of elements in the array.
 * @param element_ptr Pointer to the element to remove.
 * @param compare_func Function to compare elements (e.g. by their ID).
 * @return SUCCESS or ERROR_NOT_FOUND.
 */
ErrorCode remove_element_from_ptr_array(void ***array_ptr, size_t *count, void *element_ptr, StringCompareFunc compare_func);

// --- Other helper functions ---

/**
 * @brief Safely reads a string from stdin. Dynamically allocates memory.
 * Caller is responsible for freeing the returned string.
 * @return Pointer to the allocated string, or NULL on error.
 */
char* get_input_string();

/**
 * @brief Generates a unique ID (e.g. numeric as a string).
 * @return Allocated string with the ID, or NULL on error.
 */
char* generate_unique_id();

#endif // UTILS_H
```

### File: `utils.c`
``` c
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // For tolower() if needed for string comparison
#include <time.h>  // For random ID generation

// --- Implementations of helper functions ---

// Static counter for simple ID generation (you can use more complex logic)
static long long next_id = 1;

ErrorCode add_element_to_ptr_array(void ***array_ptr, size_t *count, void *element_ptr) {
    // Your implementation: Reallocation of the array and adding the pointer
    return ERROR_OTHER; // Placeholder
}

ErrorCode remove_element_from_ptr_array(void ***array_ptr, size_t *count, void *element_ptr, StringCompareFunc compare_func) {
    // Your implementation: Removing the pointer and reallocating the array
    // Remember that compare_func will compare *IDs* of elements,
    // so you will need to cast void* to the appropriate type (e.g., Book*, Author*)
    // and refer to their ID.
    return ERROR_OTHER; // Placeholder
}

char* get_input_string() {
    // Your implementation: Safe reading of a string using fgets and strdup
    return NULL; // Placeholder
}

char* generate_unique_id() {
    // Your implementation: Generating a unique ID (e.g. "B001", "A001")
    // You can use a simple counter or a more complex algorithm.
    char id_buffer[20];
    sprintf(id_buffer, "%lld", next_id++); // Example ID
    return strdup(id_buffer);
}

```

## 2. Authors functions & structures

### File: `authors.h`
```c 
#ifndef AUTHORS_H
#define AUTHORS_H

#include "errors.h"
#include "utils.h" // For StringCompareFunc, add/remove_element_to_ptr_array

// Forward declaration for Book (to resolve cyclic dependency)
typedef struct Book Book;

/**
 * @brief Structure representing a single author.
 * Uses a doubly linked list.
 */
typedef struct Author {
    char *author_id;          // Unique identifier of the author. Owner of the memory.
    char *name;               // Author's full name. Owner of the memory.
    char *nationality;        // Nationality. Owner of the memory.
    int birth_year;           // Year of birth.
    Book **books_written;     // Dynamic array of POINTERS to books written by the author.
                              // NOT the owner of the memory pointed to by Book*.
    size_t num_books_written; // Number of books in the books_written array.
    struct Author *prev;      // Pointer to the previous author in the list.
    struct Author *next;      // Pointer to the next author in the list.
} Author;

// --- Function prototypes for Author list (to be implemented in authors.c) ---

/**
 * @brief Creates a new Author node.
 * @return Pointer to the new node, or NULL on error.
 */
Author *create_author_node(const char *author_id, const char *name, const char *nationality, int birth_year);

/**
 * @brief Inserts a new author into the doubly linked list, maintaining alphabetical order by name.
 * @param head Pointer to the pointer to the head of the Author list.
 * @return ErrorCode.
 */
ErrorCode insert_author(Author **head, const char *author_id, const char *name, const char *nationality, int birth_year, StringCompareFunc name_compare_func);

/**
 * @brief Searches for an author in the list by their ID.
 * @param head Start of the Author list.
 * @param author_id ID of the author to find.
 * @param found_author Pointer to pointer where found author will be stored.
 * @return ErrorCode.
 */
ErrorCode find_author(Author *head, const char *author_id, Author **found_author, StringCompareFunc id_compare_func);

/**
 * @brief Deletes an author from the list by their ID.
 * Important: This function must also remove references to this author from all books
 * they wrote (updating their Authors arrays).
 * @param head Pointer to the pointer to the head of the Author list.
 * @param author_id ID of the author to delete.
 * @param main_book_list_head Pointer to the head of the main book list (for updating).
 * @return ErrorCode.
 */
ErrorCode delete_author(Author **head, const char *author_id, StringCompareFunc id_compare_func, Book *main_book_list_head);

/**
 * @brief Frees all memory occupied by the Author list.
 * @param head Pointer to the pointer to the head of the Author list.
 */
void clear_authors(Author **head);

/**
 * @brief Prints all authors in the list along with their written books.
 * @param head Start of the Author list.
 */
void print_authors(Author *head);

/**
 * @brief Adds a pointer to a book to the books_written list of the given author.
 * @param author_node Pointer to the author.
 * @param book_ptr Pointer to the book to add.
 * @return ErrorCode.
 */
ErrorCode add_book_to_author_written_list(Author *author_node, Book *book_ptr);

/**
 * @brief Removes a pointer to a book from the books_written list of the given author.
 * @param author_node Pointer to the author.
 * @param book_id ID of the book to remove.
 * @return ErrorCode.
 */
ErrorCode remove_book_from_author_written_list(Author *author_node, const char *book_id, StringCompareFunc id_compare_func);

/**
 * @brief Loads author data from a BINARY file.
 * Important: This function will need to restore links with books,
 * which means the book list must already be loaded.
 * @param head Pointer to the pointer to the head of the Author list.
 * @param filepath Path to the file.
 * @param main_book_list_head Pointer to the head of the main book list (to restore links).
 * @return ErrorCode.
 */
ErrorCode load_authors_from_binary_file(Author **head, const char *filepath, Book *main_book_list_head, StringCompareFunc book_id_compare_func);

/**
 * @brief Saves author data to a BINARY file.
 * @param head Start of the Author list.
 * @param filepath Path to the file.
 * @return ErrorCode.
 */
ErrorCode save_authors_to_binary_file(Author *head, const char *filepath);

#endif // AUTHORS_H
```

### File: `authors.c`
```c
#include "authors.h"
#include "books.h" // Needed for the full definition of the Book structure
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Implementations of functions for the Author list ---

Author *create_author_node(const char *author_id, const char *name, const char *nationality, int birth_year) {
    // Your implementation
    return NULL; // Placeholder
}

ErrorCode insert_author(Author **head, const char *author_id, const char *name, const char *nationality, int birth_year, StringCompareFunc name_compare_func) {
    // Your implementation (alphabetical sorting by name)
    return ERROR_OTHER; // Placeholder
}

ErrorCode find_author(Author *head, const char *author_id, Author **found_author, StringCompareFunc id_compare_func) {
    // Your implementation
    return ERROR_OTHER; // Placeholder
}

ErrorCode delete_author(Author **head, const char *author_id, StringCompareFunc id_compare_func, Book *main_book_list_head) {
    // Your implementation. Remember to update the Authors arrays in the books!
    return ERROR_OTHER; // Placeholder
}

void clear_authors(Author **head) {
    // Your implementation. Remember to free the books_written array.
}

void print_authors(Author *head) {
    // Your implementation
}

ErrorCode add_book_to_author_written_list(Author *author_node, Book *book_ptr) {
    // Your implementation (use add_element_to_ptr_array from utils.h)
    return ERROR_OTHER; // Placeholder
}

ErrorCode remove_book_from_author_written_list(Author *author_node, const char *book_id, StringCompareFunc id_compare_func) {
    // Your implementation (use remove_element_from_ptr_array from utils.h)
    // You will need to find the Book* matching book_id in author_node->books_written
    // to pass it to remove_element_from_ptr_array.
    return ERROR_OTHER; // Placeholder
}

ErrorCode load_authors_from_binary_file(Author **head, const char *filepath, Book *main_book_list_head, StringCompareFunc book_id_compare_func) {
    // Your implementation. THIS WILL BE A CHALLENGE!
    // Remember that you need to save/load book IDs and then find them in main_book_list_head
    // and set the appropriate pointers.
    return ERROR_OTHER; // Placeholder
}

ErrorCode save_authors_to_binary_file(Author *head, const char *filepath) {
    // Your implementation. THIS WILL BE A CHALLENGE!
    // You must save all data, including IDs of books the author wrote.
    return ERROR_OTHER; // Placeholder
}

```

## 3. Books functions & structures

### File: `books.h`
```c
#ifndef BOOKS_H
#define BOOKS_H

#include "errors.h"
#include "utils.h" // For StringCompareFunc, add/remove_element_to_ptr_array
// Forward declaration for Author
typedef struct Author Author;

// --- Enum for book genre ---
typedef enum {
    GENRE_FICTION = 0,
    GENRE_SCIENCE_FICTION,
    GENRE_FANTASY,
    GENRE_NON_FICTION,
    GENRE_OTHER
} BookGenre;

/**
 * @brief Structure representing a single book.
 * Uses a singly linked list.
 */
typedef struct Book {
    char *book_id;             // Unique identifier of the book. Owner of the memory.
    char *title;               // Title of the book. Owner of the memory.
    int publication_year;      // Year of publication.
    BookGenre genre;           // Genre.
    float rating;              // Rating (1.0-5.0).
    Author **authors;          // Dynamic array of POINTERS to authors of this book.
                               // NOT the owner of the memory pointed to by Author*.
    size_t num_authors;        // Number of authors in the authors array.
    struct Book *next;         // Pointer to the next book in the list.
} Book;

// --- Function prototypes for the Book list (to be implemented in books.c) ---

/**
 * @brief Creates a new Book node.
 * @return Pointer to the new node, or NULL on error.
 */
Book *create_book_node(const char *book_id, const char *title, int publication_year, BookGenre genre, float rating);

/**
 * @brief Inserts a new book into the singly linked list, maintaining alphabetical order by title.
 * @param head Pointer to pointer to the start of the Book list.
 * @return ErrorCode.
 */
ErrorCode insert_book(Book **head, const char *book_id, const char *title, int publication_year, BookGenre genre, float rating, StringCompareFunc title_compare_func);

/**
 * @brief Searches for a book in the list by its ID.
 * @param head Start of the Book list.
 * @param book_id ID of the book to find.
 * @param found_book Pointer to pointer where the found book will be stored.
 * @return ErrorCode.
 */
ErrorCode find_book(Book *head, const char *book_id, Book **found_book, StringCompareFunc id_compare_func);

/**
 * @brief Removes a book from the list by its ID.
 * Important: This function must also remove references to this book from all authors
 * who wrote it (updating their books_written arrays).
 * @param head Pointer to pointer to the start of the Book list.
 * @param book_id ID of the book to delete.
 * @param main_author_list_head Pointer to the start of the main author list (for updating).
 * @return ErrorCode.
 */
ErrorCode delete_book(Book **head, const char *book_id, StringCompareFunc id_compare_func, Author *main_author_list_head);

/**
 * @brief Frees all memory occupied by the Book list.
 * @param head Pointer to pointer to the start of the Book list.
 */
void clear_books(Book **head);

/**
 * @brief Prints all books in the list, along with their authors.
 * @param head Start of the Book list.
 */
void print_books(Book *head);

/**
 * @brief Adds a pointer to an author to the authors array of a given book.
 * @param book_node Pointer to the book.
 * @param author_ptr Pointer to the author to add.
 * @return ErrorCode.
 */
ErrorCode add_author_to_book_authors_array(Book *book_node, Author *author_ptr);

/**
 * @brief Removes a pointer to an author from the authors array of a given book.
 * @param book_node Pointer to the book.
 * @param author_id ID of the author to remove.
 * @return ErrorCode.
 */
ErrorCode remove_author_from_book_authors_array(Book *book_node, const char *author_id, StringCompareFunc id_compare_func);

/**
 * @brief Loads book data from a BINARY file.
 * Important: This function will have to restore links to authors,
 * meaning the author list must already be loaded.
 * @param head Pointer to pointer to the start of the Book list.
 * @param filepath Path to the file.
 * @param main_author_list_head Pointer to the start of the main author list (to restore links).
 * @return ErrorCode.
 */
ErrorCode load_books_from_binary_file(Book **head, const char *filepath, Author *main_author_list_head, StringCompareFunc author_id_compare_func);

/**
 * @brief Saves book data to a BINARY file.
 * @param head Start of the Book list.
 * @param filepath Path to the file.
 * @return ErrorCode.
 */
ErrorCode save_books_to_binary_file(Book *head, const char *filepath);

#endif // BOOKS_H
```

### File: `books.c`
```c
#include "books.h"
#include "authors.h" // Necessary for full definition of Author struct
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Implementations of functions for the Book list ---

Book *create_book_node(const char *book_id, const char *title, int publication_year, BookGenre genre, float rating) {
    // Your implementation
    return NULL; // Placeholder
}

ErrorCode insert_book(Book **head, const char *book_id, const char *title, int publication_year, BookGenre genre, float rating, StringCompareFunc title_compare_func) {
    // Your implementation (alphabetical sorting by title)
    return ERROR_OTHER; // Placeholder
}

ErrorCode find_book(Book *head, const char *book_id, Book **found_book, StringCompareFunc id_compare_func) {
    // Your implementation
    return ERROR_OTHER; // Placeholder
}

ErrorCode delete_book(Book **head, const char *book_id, StringCompareFunc id_compare_func, Author *main_author_list_head) {
    // Your implementation. Remember to update the books_written arrays in authors!
    return ERROR_OTHER; // Placeholder
}

void clear_books(Book **head) {
    // Your implementation. Remember to free the authors array.
}

void print_books(Book *head) {
    // Your implementation
}

ErrorCode add_author_to_book_authors_array(Book *book_node, Author *author_ptr) {
    // Your implementation (use add_element_to_ptr_array from utils.h)
    return ERROR_OTHER; // Placeholder
}

ErrorCode remove_author_from_book_authors_array(Book *book_node, const char *author_id, StringCompareFunc id_compare_func) {
    // Your implementation (use remove_element_from_ptr_array from utils.h)
    // You will need to find the Author* matching author_id in book_node->authors array
    // to pass it to remove_element_from_ptr_array.
    return ERROR_OTHER; // Placeholder
}

ErrorCode load_books_from_binary_file(Book **head, const char *filepath, Author *main_author_list_head, StringCompareFunc author_id_compare_func) {
    // Your implementation. THIS WILL BE A CHALLENGE!
    // Remember you need to save/load author IDs and then find them in main_author_list_head
    // and set the appropriate pointers.
    return ERROR_OTHER; // Placeholder
}

ErrorCode save_books_to_binary_file(Book *head, const char *filepath) {
    // Your implementation. THIS WILL BE A CHALLENGE!
    // You must save all data, including the IDs of the book's authors.
    return ERROR_OTHER; // Placeholder
}
```

## 4. Main Program

### File: `main.c` 
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "errors.h"
#include "authors.h"
#include "books.h"
#include "utils.h"

// Define an error code for missing arguments (if running from CLI)
#define NOT_ENOUGH_ARGS_ERROR 9

// --- Prototypes of helper menu functions ---
void print_main_menu();
void print_book_menu();
void print_author_menu();

// --- Main program function ---
int main(int argc, char *argv[]) {
    Author *author_list_head = NULL;
    Book *book_list_head = NULL;
    int choice;
    ErrorCode err;

    // --- Command-line arguments processing (Implement this block) ---
    // Expected usage: ./main [path_to_authors_binary_file] [path_to_books_binary_file]
    // IMPORTANT: Authors must be loaded BEFORE books, as books reference authors.
    // So, if argc == 3:
    // 1. Call load_authors_from_binary_file().
    // 2. Then call load_books_from_binary_file().
    // Otherwise, print usage info if argc > 1.


    // --- Main interactive loop ---
    do {
        print_main_menu();
        // Get user choice (integer)
        // Handle invalid input (non-integer, clear buffer)

        switch (choice) {
            case 1: { // Manage Books
                int book_choice;
                do {
                    print_book_menu();
                    // Get book menu choice (integer)
                    // Handle invalid input (non-integer, clear buffer)

                    switch (book_choice) {
                        case 1: { // Add Book
                            // Get title, year, genre_str, rating_str from user
                            // Generate unique BookID (use generate_unique_id from utils.h)
                            // Convert genre string to BookGenre enum, rating string to float
                            // Call insert_book()
                            // Free allocated input strings
                            break;
                        }
                        case 2: { // Find Book
                            // Get book_id from user
                            // Call find_book()
                            // Print book details or "not found"
                            // Free allocated input string
                            break;
                        }
                        case 3: { // Modify Book
                            // Get book_id, then allow modification of title, year, genre, rating
                            // Free allocated input string
                            break;
                        }
                        case 4: { // Delete Book
                            // Get book_id from user
                            // Call delete_book()
                            // Free allocated input string
                            break;
                        }
                        case 5: { // Assign Author to Book
                            // Get book_id and author_id from user
                            // Call add_author_to_book_authors_array() AND add_book_to_author_written_list()
                            // Free allocated input strings
                            break;
                        }
                        case 6: { // Remove Author from Book
                            // Get book_id and author_id from user
                            // Call remove_author_from_book_authors_array() AND remove_book_from_author_written_list()
                            // Free allocated input strings
                            break;
                        }
                        case 7: { // Display All Books
                            // Call print_books()
                            break;
                        }
                        case 8: // Back
                            break;
                        default:
                            printf("Invalid option.\n");
                            break;
                    }
                } while (book_choice != 8);
                break;
            }
            case 2: { // Manage Authors
                int author_choice;
                do {
                    print_author_menu();
                    // Get author menu choice (integer)
                    // Handle invalid input (non-integer, clear buffer)

                    switch (author_choice) {
                        case 1: { // Add Author
                            // Get name, nationality, birth_year_str from user
                            // Generate unique AuthorID (use generate_unique_id from utils.h)
                            // Convert birth_year_str to int
                            // Call insert_author()
                            // Free allocated input strings
                            break;
                        }
                        case 2: { // Find Author
                            // Get author_id from user
                            // Call find_author()
                            // Print author details or "not found"
                            // Free allocated input string
                            break;
                        }
                        case 3: { // Modify Author
                            // Get author_id, then allow modification of name, nationality, birth_year
                            // Free allocated input string
                            break;
                        }
                        case 4: { // Delete Author
                            // Get author_id from user
                            // Call delete_author()
                            // Free allocated input string
                            break;
                        }
                        case 5: { // Display All Authors
                            // Call print_authors()
                            break;
                        }
                        case 6: // Back
                            break;
                        default:
                            printf("Invalid option.\n");
                            break;
                    }
                } while (author_choice != 6);
                break;
            }
            case 3: { // Load Data
                // Get file paths for authors and books from user input
                // IMPORTANT: Load authors first, then books.
                // Call load_authors_from_binary_file()
                // Call load_books_from_binary_file()
                // Free allocated input strings
                break;
            }
            case 4: { // Save Data
                // Get file paths for authors and books from user input
                // Call save_authors_to_binary_file()
                // Call save_books_to_binary_file()
                // Free allocated input strings
                break;
            }
            case 5:
                printf("Exiting program. Freeing memory...\n");
                break;
            default:
                printf("Invalid option. Please try again.\n");
                break;
        }
    } while (choice != 5);

    // --- Memory cleanup before exit ---
    // This part requires careful planning due to mutual references and dynamic arrays.
    // General strategy:
    // 1. Clear authors' books_written arrays (only the arrays, not the books themselves).
    // 2. Clear books' authors arrays (only the arrays, not the authors themselves).
    // 3. Clear authors list (frees Author nodes and their internal string data).
    // 4. Clear books list (frees Book nodes and their internal string data).
    // Alternatively, ensure delete/clear functions handle cross-references properly
    // so no dangling pointers or double frees occur.
    // The safest approach is usually to clear the "referencing" arrays, then the main lists.
    clear_books(&book_list_head);
    clear_authors(&author_list_head);

    return 0;
}


// --- Implementations of helper menu functions ---

void print_main_menu() {
    printf("\n--- Book and Author Management System ---\n");
    printf("1. Manage Books\n");
    printf("2. Manage Authors\n");
    printf("3. Load Data from Binary Files\n");
    printf("4. Save Data to Binary Files\n");
    printf("5. Exit\n");
    printf("Choose option: ");
}

void print_book_menu() {
    printf("\n--- Book Management ---\n");
    printf("1. Add new book\n");
    printf("2. Find book\n");
    printf("3. Modify book details\n");
    printf("4. Delete book\n");
    printf("5. Assign author to book\n");
    printf("6. Remove author from book\n");
    printf("7. Display all books\n");
    printf("8. Back to main menu\n");
    printf("Choose option: ");
}

void print_author_menu() {
    printf("\n--- Author Management ---\n");
    printf("1. Add new author\n");
    printf("2. Find author\n");
    printf("3. Modify author details\n");
    printf("4. Delete author\n");
    printf("5. Display all authors\n");
    printf("6. Back to main menu\n");
    printf("Choose option: ");
}
```

## Technical Requirements and Additional Guidelines

- **No `[]` (Array Indexing):** Consistently use **pointer arithmetic** for all list manipulations.
    
- **Memory Management:** This is the **most important and hardest part** of the task.
    
    - Every memory allocation (`malloc`, `calloc`, `strdup`, `realloc`) must be **checked against `NULL`**.
        
    - All strings inside structures (`book_id`, `title`, `author_id`, `name`, `nationality`) must be **deep copied** (`strdup` or `malloc` + `strcpy`). The list nodes are the "owners" of this memory and responsible for freeing it.
        
    - Functions like `clear_books`, `clear_authors`, `delete_book`, `delete_author` must meticulously **free all allocated memory**, including **dynamic pointer arrays (`authors` in `Book` and `books_written` in `Author`)**. Remember, you free _only the pointer arrays and the nodes themselves_, but **NOT the memory pointed to by these pointers** (i.e., the actual `Book` or `Author` objects in the main lists).
        
    - **Mutual references and updates:**
        
        - When **deleting a book**, you must find all its authors and remove the reference to this book from the `books_written` arrays of each author.
            
        - When **deleting an author**, you must find all books written by that author and remove the reference to this author from the `authors` arrays of each book. Consider what to do with books that end up with no assigned authors after deletion (e.g., assign a “unknown” placeholder or require deleting the book).
            
- **Binary Files (`fread`, `fwrite`):**
    
    - You will need to define your own **data format for writing to the binary file**.
        
    - **You cannot simply save pointers!** You must save unique IDs (`book_id`, `author_id`), and when loading, reconstruct links by searching for these objects in memory by their IDs.
        
    - Loading: Always load **authors first**, then **books**. Why? Because books may reference authors who must already be loaded in memory.
        
- **Sorting Algorithms:** You can implement your own sorting in `insert_book` and `insert_author` (e.g., insertion sort) or implement separate sorting functions (e.g., `qsort` with a custom compare function) before displaying or saving.
    
- **ID Uniqueness:** Make sure duplicates of `BookID` or `AuthorID` cannot be added.
    
- **Interactive Console Interface:** User menu similar to previous tasks.
    

---

### Suggested Implementation Process

1. **Setup:** Create all `.h` and `.c` files.
    
2. **`errors.h`:** Copy or create error code definitions.
    
3. **`utils.h` and `utils.c`:** Implement all helper functions, especially those for **dynamic pointer arrays** (`add_element_to_ptr_array`, `remove_element_from_ptr_array`). Test them separately. Also implement `generate_unique_id`.
    
4. **Structure Definitions:** Make sure `authors.h` and `books.h` contain correct enum, struct, and **forward declarations** (`typedef struct Book Book;` in `authors.h` and `typedef struct Author Author;` in `books.h`).
    
5. **Basic List Functions:**
    
    - In `authors.c`: `create_author_node`, `insert_author` (initially without sorting, just append at the end), `print_authors`, `clear_authors`.
        
    - In `books.c`: `create_book_node`, `insert_book` (without sorting), `print_books`, `clear_books`.
        
6. **`main.c`:** Implement the main menu and submenus. Connect basic functions (adding, displaying, clearing). Focus on the interactive loop.
    
7. **Search and Modify Functions:** Implement `find_book`, `find_author`, and functions to modify data.
    
8. **Relationship Management (Key Step):**
    
    - Implement `add_author_to_book_authors_array` and `remove_author_from_book_authors_array` in `books.c`.
        
    - Implement `add_book_to_author_written_list` and `remove_book_from_author_written_list` in `authors.c`.
        
    - Then modify `delete_book` and `delete_author` to properly update mutual references (remove references from the other list). This will require passing pointers to the main book/author lists.
        
9. **Sorting:** Now implement sorting in `insert_book` and `insert_author` or add separate sorting functions.
    
10. **Binary File Handling (`load`/`save`):** This is the biggest challenge.
    
    - Think about the save format: how to save strings, how to save pointer arrays (only their IDs).
        
    - Remember loading order (authors first, then books) and the need to reconstruct pointers by ID.
        
11. **Refactoring and Testing:**
    
    - Test the entire system carefully using `valgrind` to detect memory leaks and access errors.
        
    - Test edge cases: empty lists, adding/removing from empty lists, adding/removing the only element, deleting an object with many relations, etc.