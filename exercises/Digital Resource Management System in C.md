### **Project Goal**

The goal of this project is to design and implement a simple system in C for managing hypothetical **digital resources** (files) and **user records**.  
The system will incorporate advanced C programming concepts, including:

- **Singly and doubly linked lists**
    
- **Bitwise operations**
    
- **Function pointers**
    
- **Preprocessor macros**
    
- **Precise memory management**
    

The program will be **interactive** and will support **loading and saving data to files**.

---

### **File Structure**

The project should be organized into the following files:

- **`errors.h`** – Definitions of error codes.
    
- **`utils.h`** – Macros for bitwise operations and prototypes of helper functions.
    
- **`utils.c`** – Implementations of helper functions and macros.
    
- **`assets.h`** – Definition of the `DigitalAsset` structure and function prototypes for managing the resource list (singly linked).
    
- **`assets.c`** – Implementations of functions for managing the `DigitalAsset` list.
    
- **`users.h`** – Definitions of `UserRecord` and `UserAssetRef` structures, and prototypes for managing the user list (doubly linked).
    
- **`users.c`** – Implementations of functions for managing the `UserRecord` list.
    
- **`main.c`** – Main program logic, interactive menu, and command-line argument handling.
    

---

## 1. Error Enumeration

**File: `errors.h`**
``` C
#ifndef ERRORS_H
#define ERRORS_H

// Definition of error codes using an enum.
// Use these codes as return values for functions.
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

## 2. Structures & Functions

### 2.1. Singly Linked List (Digital Assets)

This list will represent digital files.

**File: `assets.h`**
``` C
#ifndef ASSETS_H
#define ASSETS_H

#include <stdint.h> // For uint32_t, uint8_t
#include "errors.h" // For ErrorCode

// Bit flags definitions for an asset.
// Use bitwise operators to manipulate these flags.
#define ASSET_FLAG_ENCRYPTED   (1U << 0) // Asset is encrypted
#define ASSET_FLAG_READ_ONLY   (1U << 1) // Asset is read-only
#define ASSET_FLAG_ARCHIVED    (1U << 2) // Asset is archived
#define ASSET_FLAG_CORRUPTED   (1U << 3) // Asset is corrupted
// You can add more flags as needed, e.g., ASSET_FLAG_SHARED, ASSET_FLAG_PUBLIC

/**
 * @brief Structure representing a single digital asset (file).
 */
typedef struct DigitalAsset {
    char *hash;             // Unique identifier for the asset (e.g., SHA256 as a hex string). The node owns this memory.
    uint32_t size_bytes;    // Size of the file in bytes.
    uint8_t flags;          // Bit flags indicating the asset's state.
    struct DigitalAsset *next; // Pointer to the next asset in the singly linked list.
} DigitalAsset;

/**
 * @brief Function pointer for comparing two hashes (strings).
 * Returns <0 if hash1 < hash2, 0 if equal, >0 if hash1 > hash2.
 */
typedef int (*AssetHashCompareFunc)(const char *hash1, const char *hash2);

/**
 * @brief Function pointer for counting '01' bit pairs in a string.
 */
typedef uint8_t (*StringBitPairCounter)(const char *str);


// --- Function Prototypes for DigitalAsset List (Implement in assets.c) ---

/**
 * @brief Creates a new DigitalAsset node.
 * @param hash Unique asset hash (string).
 * @param size Size of the asset in bytes.
 * @param flags Bit flags for the asset.
 * @return Pointer to the newly created node, or NULL if memory allocation fails or data is invalid.
 */
DigitalAsset *create_asset_node(const char *hash, uint32_t size, uint8_t flags);

/**
 * @brief Inserts a new asset into the list, maintaining alphabetical order by hash.
 * @param head Pointer to the pointer to the head of the DigitalAsset list.
 * @param hash Hash of the asset to insert.
 * @param size Size of the asset.
 * @param flags Flags of the asset.
 * @param compare_func Function pointer for comparing hashes.
 * @return ErrorCode.
 */
ErrorCode insert_asset(DigitalAsset **head, const char *hash, uint32_t size, uint8_t flags, AssetHashCompareFunc compare_func);

/**
 * @brief Finds an asset in the list by its hash.
 * @param head Head of the DigitalAsset list.
 * @param hash Hash of the asset to find.
 * @param found_asset Pointer to a pointer where the found asset will be stored.
 * @param compare_func Function pointer for comparing hashes.
 * @return ErrorCode.
 */
ErrorCode find_asset(DigitalAsset *head, const char *hash, DigitalAsset **found_asset, AssetHashCompareFunc compare_func);

/**
 * @brief Deletes an asset from the list by its hash.
 * Important: This function must also handle the removal of all *references* to this asset
 * from users' `owned_assets` lists. You will need a mechanism to access the user list for this.
 * @param head Pointer to the pointer to the head of the DigitalAsset list.
 * @param hash Hash of the asset to delete.
 * @param compare_func Function pointer for comparing hashes.
 * @return ErrorCode.
 */
ErrorCode delete_asset(DigitalAsset **head, const char *hash, AssetHashCompareFunc compare_func);

/**
 * @brief Frees all memory occupied by the DigitalAsset list.
 * @param head Pointer to the pointer to the head of the DigitalAsset list.
 */
void clear_assets(DigitalAsset **head);

/**
 * @brief Prints all assets in the list.
 * @param head Head of the DigitalAsset list.
 */
void print_assets(DigitalAsset *head);

/**
 * @brief Loads assets from a file into the list.
 * File format (example): hash size_bytes flags
 * @param head Pointer to the pointer to the head of the DigitalAsset list.
 * @param filepath Path to the file.
 * @param compare_func Function pointer for comparing hashes (for insertion).
 * @return ErrorCode.
 */
ErrorCode load_assets_from_file(DigitalAsset **head, const char *filepath, AssetHashCompareFunc compare_func);

/**
 * @brief Saves assets from the list to a file.
 * @param head Head of the DigitalAsset list.
 * @param filepath Path to the file.
 * @return ErrorCode.
 */
ErrorCode save_assets_to_file(DigitalAsset *head, const char *filepath);

#endif // ASSETS_H
```

### 2.2. Doubly Linked List (User Records)

This list will represent users and their assigned assets.

**File: `users.h`**
``` C
#ifndef USERS_H
#define USERS_H

#include <stdint.h>
#include "errors.h"
#include "assets.h" // Needed because UserRecord will link to DigitalAsset

/**
 * @brief Helper structure to create a list of assets assigned to a user.
 * It contains a POINTER to a DigitalAsset from the MAIN DigitalAsset list.
 * It does not own the memory pointed to by 'asset_ptr'.
 */
typedef struct UserAssetRef {
    DigitalAsset *asset_ptr; // Pointer to an asset from the main list.
    struct UserAssetRef *next; // Pointer to the next asset reference in the user's list.
} UserAssetRef;

/**
 * @brief Structure representing a user record in a doubly linked list.
 */
typedef struct UserRecord {
    char *username;         // Username. The node owns this memory.
    uint32_t user_id;       // Unique user identifier.
    UserAssetRef *owned_assets; // Head of a singly linked list of pointers to DigitalAssets owned by this user.
                                // THIS list (UserAssetRef nodes) is allocated and freed BY the UserRecord,
                                // but the *DigitalAsset pointed to* is NOT.
    struct UserRecord *prev; // Pointer to the previous record in the doubly linked list.
    struct UserRecord *next; // Pointer to the next record in the doubly linked list.
} UserRecord;

/**
 * @brief Function pointer for comparing two usernames (strings).
 * Returns <0 if name1 < name2, 0 if equal, >0 if name1 > name2.
 */
typedef int (*UserNameCompareFunc)(const char *name1, const char *name2);


// --- Function Prototypes for UserRecord List (Implement in users.c) ---

/**
 * @brief Creates a new UserRecord node.
 * @param username Username (string).
 * @param user_id User ID.
 * @return Pointer to the newly created node, or NULL if memory allocation fails or data is invalid.
 */
UserRecord *create_user_node(const char *username, uint32_t user_id);

/**
 * @brief Inserts a new user into the doubly linked list, maintaining alphabetical order by username.
 * @param head Pointer to the pointer to the head of the UserRecord list.
 * @param username Username to insert.
 * @param user_id User ID.
 * @param compare_func Function pointer for comparing usernames.
 * @return ErrorCode.
 */
ErrorCode insert_user(UserRecord **head, const char *username, uint32_t user_id, UserNameCompareFunc compare_func);

/**
 * @brief Finds a user in the list by username.
 * @param head Head of the UserRecord list.
 * @param username Username to find.
 * @param found_user Pointer to a pointer where the found user will be stored.
 * @param compare_func Function pointer for comparing usernames.
 * @return ErrorCode.
 */
ErrorCode find_user(UserRecord *head, const char *username, UserRecord **found_user, UserNameCompareFunc compare_func);

/**
 * @brief Deletes a user from the list by username.
 * Important: This function must free memory only for the UserRecord node and its internal UserAssetRef list,
 * but NOT for the DigitalAssets pointed to by UserAssetRef.
 * @param head Pointer to the pointer to the head of the UserRecord list.
 * @param username Username to delete.
 * @param compare_func Function pointer for comparing usernames.
 * @return ErrorCode.
 */
ErrorCode delete_user(UserRecord **head, const char *username, UserNameCompareFunc compare_func);

/**
 * @brief Frees all memory occupied by the UserRecord list.
 * Must correctly free UserRecord nodes and their internal UserAssetRef lists,
 * but NOT the DigitalAssets that these references point to.
 * @param head Pointer to the pointer to the head of the UserRecord list.
 */
void clear_users(UserRecord **head);

/**
 * @brief Prints all users and their assigned assets.
 * @param head Head of the UserRecord list.
 */
void print_users(UserRecord *head);

/**
 * @brief Assigns an existing asset to a user. Adds a reference to the user's owned_assets list.
 * @param user_head Head of the UserRecord list (to find the user).
 * @param asset_head Head of the main DigitalAsset list (to find the asset).
 * @param username Username.
 * @param asset_hash Hash of the asset to assign.
 * @param user_compare Function pointer for comparing usernames.
 * @param asset_compare Function pointer for comparing asset hashes.
 * @return ErrorCode.
 */
ErrorCode assign_asset_to_user(UserRecord *user_head, DigitalAsset *asset_head, const char *username, const char *asset_hash, UserNameCompareFunc user_compare, AssetHashCompareFunc asset_compare);

/**
 * @brief Removes an asset assignment from a user (removes the reference from the user's owned_assets list).
 * @param user_head Head of the UserRecord list.
 * @param username Username.
 * @param asset_hash Hash of the asset to remove.
 * @param user_compare Function pointer for comparing usernames.
 * @param asset_compare Function pointer for comparing asset hashes.
 * @return ErrorCode.
 */
ErrorCode remove_asset_from_user(UserRecord *user_head, const char *username, const char *asset_hash, UserNameCompareFunc user_compare, AssetHashCompareFunc asset_compare);

/**
 * @brief Loads users from a file into the list.
 * File format (example): username user_id asset_hash1 asset_hash2 ...
 * Important: When loading, you must find the corresponding DigitalAsset in the main list (main_asset_list_head)
 * and add pointers to these *existing* DigitalAssets to the user's owned_assets list.
 * @param head Pointer to the pointer to the head of the UserRecord list.
 * @param filepath Path to the file.
 * @param compare_func Function pointer for comparing usernames (for insertion).
 * @param main_asset_list_head Head of the main DigitalAsset list (needed to find assets for assignment).
 * @return ErrorCode.
 */
ErrorCode load_users_from_file(UserRecord **head, const char *filepath, UserNameCompareFunc compare_func, DigitalAsset *main_asset_list_head);

/**
 * @brief Saves users and their assigned assets to a file.
 * @param head Head of the UserRecord list.
 * @param filepath Path to the file.
 * @return ErrorCode.
 */
ErrorCode save_users_to_file(UserRecord *head, const char *filepath);

#endif // USERS_H
```

## 3. Macros and Utility Functions

**File: `utils.h`**
``` C
#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <string.h> // For strlen (if you use it in macro implementation)

// Macro generating a function to count '01' bit pairs in a given numeric type.
// E.g., CREATE_BIT_PAIR_COUNT_FUNC(char) will create a prototype: uint8_t count_01_pairs_char(char val);
#define CREATE_BIT_PAIR_COUNT_FUNC(type_name) \
uint8_t count_01_pairs_##type_name(type_name val)

// Macro generating a function to count '01' bit pairs in each byte of a string.
// This function will be used for hashes and names.
// In its implementation, you will need to call the count_01_pairs_char function.
#define CREATE_STRING_BIT_PAIR_COUNT_FUNC(func_name) \
uint8_t func_name(const char *str)

// --- Declarations of functions generated by macros (implement in utils.c) ---

// Declare a specific instance for 'char' so it can be used in the string counting function.
CREATE_BIT_PAIR_COUNT_FUNC(char);

// Declare the function to count '01' pairs in strings, which will use count_01_pairs_char.
CREATE_STRING_BIT_PAIR_COUNT_FUNC(count_01_pairs_in_string);

// --- Prototypes of comparison functions (implement in utils.c or main.c) ---

/**
 * @brief Function for comparing two hashes (strings).
 * @return <0 if hash1 < hash2, 0 if equal, >0 if hash1 > hash2.
 */
int compare_asset_hashes(const char *hash1, const char *hash2);

/**
 * @brief Function for comparing two usernames (strings).
 * Implement simple case-insensitivity (e.g., convert to lowercase for comparison).
 * @return <0 if name1 < name2, 0 if equal, >0 if name1 > name2.
 */
int compare_user_names(const char *name1, const char *name2);

#endif // UTILS_H
```

File: `utils.c`
``` C
#include "utils.h"
#include <stdlib.h> // For tolower, isalpha (if used in compare_user_names)
#include <string.h> // For strcmp, strlen

// --- Implementations of functions generated by macros ---

// Implement the function to count '01' bit pairs for the char type.
// Use bitwise operators to check the bits.
CREATE_BIT_PAIR_COUNT_FUNC(char) {
    // Your implementation here
    return 0; // Placeholder
}

// Implement the function to count '01' bit pairs in a string.
// Remember to call count_01_pairs_char for each character in the string.
CREATE_STRING_BIT_PAIR_COUNT_FUNC(count_01_pairs_in_string) {
    // Your implementation here
    return 0; // Placeholder
}

// --- Implementations of comparison functions ---

/**
 * @brief Implement the function for comparing two hashes (strings).
 * @return <0 if hash1 < hash2, 0 if equal, >0 if hash1 > hash2.
 */
int compare_asset_hashes(const char *hash1, const char *hash2) {
    // Your implementation here
    return 0; // Placeholder
}

/**
 * @brief Implement the function for comparing two usernames (strings).
 * Implement simple case-insensitivity (e.g., convert to lowercase for comparison).
 * @return <0 if name1 < name2, 0 if equal, >0 if name1 > name2.
 */
int compare_user_names(const char *name1, const char *name2) {
    // Your implementation here
    return 0; // Placeholder
}
```
## 4. Main Program Logic

**File: `main.c`**
``` c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "errors.h"
#include "assets.h"
#include "users.h"
#include "utils.h"

// Define an error code for missing arguments (if running from CLI)
#define NOT_ENOUGH_ARGS_ERROR 9

// --- Prototypes of helper menu functions ---
// (Implement these functions below main())
void print_menu();
void print_asset_menu();
void print_user_menu();
char* get_input_string(); // Function for safely getting a string from stdin


// --- Main program function ---
int main(int argc, char *argv[]) {
    DigitalAsset *asset_list_head = NULL;
    UserRecord *user_list_head = NULL;
    int choice;
    ErrorCode err;

    // --- Command-line arguments processing (Implement this block) ---
    // Expected usage: ./main [path_to_asset_file] [path_to_user_file]
    // If argc == 3, attempt to load initial data.
    // Otherwise, print usage info if argc > 1.
    // This section will call load_assets_from_file and load_users_from_file.

    // --- Main interactive loop ---
    do {
        print_menu();
        // Get user choice (integer)
        // Handle invalid input (non-integer, clear buffer)

        switch (choice) {
            case 1: { // Manage Digital Assets
                int asset_choice;
                do {
                    print_asset_menu();
                    // Get asset menu choice (integer)
                    // Handle invalid input (non-integer, clear buffer)

                    switch (asset_choice) {
                        case 1: { // Add Asset
                            // Get asset hash, size, flags from user input (use get_input_string)
                            // Convert string inputs to appropriate types (e.g., atoi)
                            // Call insert_asset()
                            // Free allocated input strings
                            break;
                        }
                        case 2: { // Find Asset
                            // Get asset hash from user input
                            // Call find_asset()
                            // Print found asset details or "not found" message
                            // Free allocated input string
                            break;
                        }
                        case 3: { // Delete Asset
                            // Get asset hash from user input
                            // Call delete_asset()
                            // Free allocated input string
                            break;
                        }
                        case 4: { // Display All Assets
                            // Call print_assets()
                            break;
                        }
                        case 5: // Back
                            break;
                        default:
                            printf("Invalid option.\n");
                            break;
                    }
                } while (asset_choice != 5);
                break;
            }
            case 2: { // Manage Users
                int user_choice;
                do {
                    print_user_menu();
                    // Get user menu choice (integer)
                    // Handle invalid input (non-integer, clear buffer)

                    switch (user_choice) {
                        case 1: { // Add User
                            // Get username, user ID from user input
                            // Call insert_user()
                            // Free allocated input strings
                            break;
                        }
                        case 2: { // Find User
                            // Get username from user input
                            // Call find_user()
                            // Print found user details or "not found" message
                            // TODO: Add call to print user's owned assets
                            // Free allocated input string
                            break;
                        }
                        case 3: { // Delete User
                            // Get username from user input
                            // Call delete_user()
                            // Free allocated input string
                            break;
                        }
                        case 4: { // Display All Users
                            // Call print_users()
                            break;
                        }
                        case 5: { // Assign Asset to User
                            // Get username and asset hash from user input
                            // Call assign_asset_to_user()
                            // Free allocated input strings
                            break;
                        }
                        case 6: { // Remove Asset from User
                            // Get username and asset hash from user input
                            // Call remove_asset_from_user()
                            // Free allocated input strings
                            break;
                        }
                        case 7: // Back
                            break;
                        default:
                            printf("Invalid option.\n");
                            break;
                    }
                } while (user_choice != 7);
                break;
            }
            case 3: { // Load Data
                // Get file paths for assets and users from user input
                // Call load_assets_from_file()
                // Call load_users_from_file()
                // Free allocated input strings
                break;
            }
            case 4: { // Save Data
                // Get file paths for assets and users from user input
                // Call save_assets_to_file()
                // Call save_users_to_file()
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
    // IMPORTANT: Call clear_users() BEFORE clear_assets()!
    clear_users(&user_list_head);
    clear_assets(&asset_list_head);

    return 0;
}


// --- Implementations of helper menu functions ---

void print_menu() {
    printf("\n--- Digital Asset Management System ---\n");
    printf("1. Manage Digital Assets (Files)\n");
    printf("2. Manage Users\n");
    printf("3. Load Data from Files\n");
    printf("4. Save Data to Files\n");
    printf("5. Exit\n");
    printf("Choose option: ");
}

void print_asset_menu() {
    printf("\n--- Digital Asset Management ---\n");
    printf("1. Add new asset\n");
    printf("2. Find asset\n");
    printf("3. Delete asset\n");
    printf("4. Display all assets\n");
    printf("5. Back to main menu\n");
    printf("Choose option: ");
}

void print_user_menu() {
    printf("\n--- User Management ---\n");
    printf("1. Add new user\n");
    printf("2. Find user\n");
    printf("3. Delete user\n");
    printf("4. Display all users\n");
    printf("5. Assign asset to user\n");
    printf("6. Remove asset from user\n");
    printf("7. Back to main menu\n");
    printf("Choose option: ");
}

// Function for safely getting a string from input
char* get_input_string() {
    // Implement safe string input (e.g., using fgets and strdup)
    // Remember to handle potential NULL return from fgets/strdup.
    // The caller is responsible for freeing the returned string.
    return NULL; // Placeholder
}
```

## **Technical Requirements and Additional Guidelines**

- **No `[]` (Array Indexing)**:  
    You must strictly use **pointer arithmetic** for all list manipulations and string operations.
    

---

### **Memory Management**

- Every memory allocation (`malloc`, `calloc`, `strdup`) must be checked for `NULL`.
    
- All strings inside structures (e.g. `hash`, `username`) must be **deep copied** using `strdup()` or `malloc` + `strcpy()`.  
    The list nodes are considered **owners** of this memory and are responsible for freeing it.
    
- The functions `clear_assets` and `clear_users` (and also `delete_asset`, `delete_user`) must **free all allocated memory carefully** to avoid memory leaks.
    
- **Important for `owned_assets`**:  
    The `UserAssetRef` list inside each `UserRecord` contains **pointers to `DigitalAsset` nodes** from the main asset list.  
    This means that when freeing a `UserRecord` or its internal `UserAssetRef` list, you **must only free the `UserAssetRef` nodes themselves**, **not** the `DigitalAsset` nodes they point to.  
    The memory for each `DigitalAsset` must only be freed by `clear_assets` or `delete_asset`.
    

---

### **Function Pointers**

You must use function pointers for:

- `AssetHashCompareFunc`
    
- `UserNameCompareFunc`
    
- `StringBitPairCounter`
    

These will be used for dynamic function calls (comparisons, counting).

---

### **Macros**

You must use:

- `CREATE_BIT_PAIR_COUNT_FUNC`
    
- `CREATE_STRING_BIT_PAIR_COUNT_FUNC`
    

These macros generate bitwise functions in `utils.c`.

---

### **Bitwise Operations**

Use the following bitwise operators to **set, check, and modify flags** in `DigitalAsset` structures:

- `|` (OR)
    
- `&` (AND)
    
- `^` (XOR)
    
- `~` (NOT)
    
- `<<`, `>>` (bit shifting)
    

---

### **File Handling**

Implement the following:

- `load_assets_from_file`
    
- `save_assets_to_file`
    
- `load_users_from_file`
    
- `save_users_to_file`
    

The file format is up to you, but should be **simple** (e.g., space-separated text).

> `load_users_from_file` must be able to **link assets to users**, so it requires access to the global asset list and must **search for assets by hash**.

---

### **Multi-argument `main`**

The program should support optional **command-line arguments** (e.g., file paths for data) to preload system state.

---

### **Error Handling**

All functions returning `ErrorCode` must **correctly handle and return meaningful errors** based on `errors.h`.

---

### **Sorting**

Both lists must maintain **alphabetical order** upon insertion:

- `DigitalAsset` list – sorted by hash
    
- `UserRecord` list – sorted by username
    

---

### **Uniqueness Enforcement**

Duplicates must be prevented:

- No two assets with the same hash
    
- No two users with the same username
    

---

## **Suggested Implementation Process**

1. **Setup**: Create all `.h` and `.c` files as listed previously.
    
2. **`errors.h`**:  
    Define the `enum ErrorCode`.
    
3. **`utils.h` / `utils.c`**:
    
    - Implement `CREATE_BIT_PAIR_COUNT_FUNC` and `CREATE_STRING_BIT_PAIR_COUNT_FUNC`.
        
    - Implement `count_01_pairs_char` and `count_01_pairs_in_string` (they’re generated by the macros).
        
    - Implement comparison functions: `compare_asset_hashes`, `compare_user_names`.  
        Use simple case-insensitive comparison for usernames.
        
4. **`assets.h` / `assets.c`** (Singly Linked List):
    
    - Implement `create_asset_node`.
        
    - Start with `insert_asset`, `print_assets`, `clear_assets`.  
        Ensure sorting and uniqueness.
        
    - Add `find_asset` and `delete_asset`.  
        `delete_asset` must also remove references from all users (may require looping over the user list).
        
    - Finally: `load_assets_from_file` and `save_assets_to_file`.
        
5. **`users.h` / `users.c`** (Doubly Linked List):
    
    - Implement `create_user_node`.
        
    - Implement `insert_user`, `print_users`, `clear_users`.  
        Correctly handle `prev` and `next` for doubly-linked list.
        
    - Add `find_user` and `delete_user`.  
        Only the `UserRecord` and its `UserAssetRef` list should be freed.
        
    - Implement `assign_asset_to_user` and `remove_asset_from_user`.
        
    - Then: `load_users_from_file` and `save_users_to_file`.  
        Note: `load_users_from_file` will need access to the global asset list.
        
6. **`main.c`**:
    
    - Implement the **interactive menu**.
        
    - Connect all functions to menu options.
        
    - Handle **command-line arguments** for preloading data.
        
    - ⚠️ Ensure `clear_users` is called **before** `clear_assets` on exit to avoid dangling pointers.
        

---

## **Example Data for `assets.txt` (25 lines)**

This file contains an extended list of digital assets.  
Each line represents one asset and includes:

- **hash**
    
- **size in bytes**
    
- **sum of bit flags**
    

**`assets.txt`**
```
a1b2c3d4e5f6g7h8 1024 1   ; Asset_01: Encrypted
i9j0k1l2m3n4o5p6 2048 2   ; Asset_02: Read-only
q7r8s9t0u1v2w3x4 512  3   ; Asset_03: Encrypted and Read-only
y5z6a7b8c9d0e1f2 4096 0   ; Asset_04: No flags
g3h4i5j6k7l8m9n0 128  4   ; Asset_05: Archived
o1p2q3r4s5t6u7v8 768  5   ; Asset_06: Encrypted and Archived
z9x8c7v6b5n4m3l2 300  6   ; Asset_07: Read-only and Archived
k1j2h3g4f5d6s7a8 1500 7   ; Asset_08: Encrypted, Read-only, Archived
e9f0g1h2i3j4k5l6 256  8   ; Asset_09: Corrupted (new flag for testing)
m7n8o9p0q1r2s3t4 800  9   ; Asset_10: Encrypted and Corrupted
u5v6w7x8y9z0a1b2 1100 10  ; Asset_11: Read-only and Corrupted
c3d4e5f6g7h8i9j0 2200 12  ; Asset_12: Archived and Corrupted
p1q2r3s4t5u6v7w8 600  13  ; Asset_13: Encrypted, Archived, Corrupted
x9y0z1a2b3c4d5e6 1800 14  ; Asset_14: Read-only, Archived, Corrupted
f7g8h9i0j1k2l3m4 3500 15  ; Asset_15: All flags set (1+2+4+8)
n5o6p7q8r9s0t1u2 900  0   ; Asset_16: No flags
v3w4x5y6z7a8b9c0 1700 1   ; Asset_17: Encrypted
d1e2f3g4h5i6j7k8 2800 2   ; Asset_18: Read-only
l9m0n1o2p3q4r5s6 450  3   ; Asset_19: Encrypted and Read-only
t7u8v9w0x1y2z3a4 3200 4   ; Asset_20: Archived
b5c6d7e8f9g0h1i2 700  5   ; Asset_21: Encrypted and Archived
j3k4l5m6n7o8p9q0 1300 6   ; Asset_22: Read-only and Archived
r1s2t3u4v5w6x7y8 2100 7   ; Asset_23: Encrypted, Read-only, Archived
z9a0b1c2d3e4f5g6 500  8   ; Asset_24: Corrupted
h7i8j9k0l1m2n3o4 2600 0   ; Asset_25: No flags
```

---

## **Sample Data for `users.txt` (20 lines)**

This file contains an extended list of users.  
Each line represents a single user and consists of the following:

- **username**
    
- **user ID**
    
- An optional list of **asset hashes** that the user owns
    

⚠️ **Note**: The asset hashes listed must already exist in `assets.txt` (or in memory after loading the assets).

**`users.txt`**

```
john_doe 1001 a1b2c3d4e5f6g7h8 q7r8s9t0u1v2w3x4 z9x8c7v6b5n4m3l2
jane_smith 1002 i9j0k1l2m3n4o5p6 e9f0g1h2i3j4k5l6
bob_johnson 1003 y5z6a7b8c9d0e1f2 g3h4i5j6k7l8m9n0 o1p2q3r4s5t6u7v8 ; Includes Asset_06
alice_williams 1004 ; No assets assigned
charlie_brown 1005 k1j2h3g4f5d6s7a8 u5v6w7x8y9z0a1b2
diana_prince 1006 m7n8o9p0q1r2s3t4 c3d4e5f6g7h8i9j0
eve_adams 1007 p1q2r3s4t5u6v7w8 x9y0z1a2b3c4d5e6
frank_white 1008 f7g8h9i0j1k2l3m4
grace_kelly 1009 n5o6p7q8r9s0t1u2 v3w4x5y6z7a8b9c0
harry_potter 1010 d1e2f3g4h5i6j7k8 l9m0n1o2p3q4r5s6
sarah_connor 1011 t7u8v9w0x1y2z3a4 b5c6d7e8f9g0h1i2
mike_tyson 1012 j3k4l5m6n7o8p9q0 r1s2t3u4v5w6x7y8
lisa_simpson 1013 z9a0b1c2d3e4f5g6 h7i8j9k0l1m2n3o4 ; Includes Asset_24 and Asset_25
david_jones 1014 a1b2c3d4e5f6g7h8 i9j0k1l2m3n4o5p6 ; Duplicates assets for testing
emily_clark 1015 q7r8s9t0u1v2w3x4
chris_evans 1016 y5z6a7b8c9d0e1f2
olivia_kim 1017 g3h4i5j6k7l8m9n0
peter_parker 1018 o1p2q3r4s5t6u7v8
mary_jane 1019 z9x8c7v6b5n4m3l2
gwen_stacy 1020 k1j2h3g4f5d6s7a8 not_a_real_hash_here ; Invalid hash for testing
```

---

## **Important Notes for `users.txt`**

- In the line for `gwen_stacy`, I intentionally included the hash `not_a_real_hash_here`, which **does not exist** in the `assets.txt` file.  
    Your `load_users_from_file` function should handle this case properly — for example, by **skipping** such non-existent hashes and optionally printing a **warning** to the console.  
    This is an important test case for the **robustness** of your code.
    
- Some users, such as `david_jones`, have hashes assigned that are **also assigned to other users**, which is **allowed** in the reference system.
    
- Please note that the file format is only a suggestion.  
    You are free to adapt it as needed, but make sure that your loading and saving functions **correctly handle** the chosen format.