### Programming Task: Implementation of Your Own Memory Manager (Declarations and Instructions Only)

**Project Goal:** Your task is to implement simplified versions of the functions `malloc`, `free`, `realloc`, `calloc`, and `memset`. They will operate on a single, pre-allocated **memory arena**, which will help you understand the mechanisms of allocation, freeing, resizing, and initializing memory at a low level.

---

### File Structure

The project will consist of the following files:

- **`mymalloc.h`**: Declarations of your memory manager functions and definitions of helper structures.
    
- **`mymalloc.c`**: **Your implementation** of all memory manager functions.
    
- **`main.c`**: The main program logic for testing your memory manager. You can create a simple test interface here.

---

### 1. Functions & structures

### File: `mymalloc.h`
```c
#ifndef MYMALLOC_H
#define MYMALLOC_H

#include <stddef.h> // For size_t type

// --- Error codes for your memory manager ---
typedef enum {
    MY_SUCCESS = 0,
    MY_ERROR_NO_MEMORY = 1,
    MY_ERROR_INVALID_POINTER = 2,
    MY_ERROR_UNINITIALIZED = 3,
    MY_ERROR_OTHER = 255
} MyErrorCode;

// --- Memory alignment macro ---
// Memory returned to the user must be aligned. This means
// that its starting address should be a multiple of ALIGNMENT.
// Block headers and data must be properly aligned.
// Recommended value is 8 (for 64-bit systems).
#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

// --- Memory block header structure ---
// This header will be placed at the beginning of each block in the arena.
// It will store metadata about the block (size, free/used status, etc.).
// Remember that the size of the header must also be aligned.
typedef struct BlockHeader {
    size_t size;                 // Total size of the block, including header
    int is_free;                 // Flag: 1 if block is free, 0 if occupied
    struct BlockHeader *next;    // Pointer to the next block in the memory arena
    struct BlockHeader *prev;    // Pointer to the previous block in the memory arena
    // Optional padding if sizeof(BlockHeader) is not multiple of ALIGNMENT
} BlockHeader;

// --- Prototypes of your memory manager functions ---

/**
 * @brief Initializes the memory manager by allocating the main arena.
 * @param total_size Total size of the arena to allocate.
 * @return MY_SUCCESS on success, error otherwise.
 */
MyErrorCode MyMemoryInit(size_t total_size);

/**
 * @brief Frees the main memory arena and resets the manager.
 */
void MyMemoryCleanup();

/**
 * @brief Allocates a block of memory of the given size.
 * Implements First Fit strategy.
 * @param size Number of bytes of usable memory to allocate.
 * @return Pointer to the allocated memory, or NULL on error.
 */
void *MyMalloc(size_t size);

/**
 * @brief Frees a previously allocated block of memory.
 * Implements coalescing (merging free blocks).
 * @param ptr Pointer to the memory block to free.
 */
void MyFree(void *ptr);

/**
 * @brief Allocates memory for an array of elements and zeros it.
 * @param num_elements Number of elements.
 * @param element_size Size of a single element.
 * @return Pointer to allocated and zeroed memory, or NULL.
 */
void *MyCalloc(size_t num_elements, size_t element_size);

/**
 * @brief Changes the size of a previously allocated memory block.
 * @param ptr Pointer to the original memory block.
 * @param new_size New requested size of the block.
 * @return Pointer to the (possibly new) memory block, or NULL on error.
 */
void *MyRealloc(void *ptr, size_t new_size);

/**
 * @brief Fills a memory block with a specified value.
 * @param s Pointer to the memory block.
 * @param c Value to set (as int, but treated as unsigned char).
 * @param n Number of bytes to set.
 * @return Pointer to memory block 's'.
 */
void *MyMemset(void *s, int c, size_t n);

/**
 * @brief Displays the current state of the memory arena (for diagnostics).
 */
void MyMemoryDump();

#endif // MYMALLOC_H
```


### 2. Implementation Instructions (`mymalloc.c`)

The file `mymalloc.c` will contain the implementations of all functions declared in `mymalloc.h`. You will need to write the code for each of them yourself, using the following guidelines.

#### Global / Static Variables:

- `static char *g_memory_arena;`: Main pointer to the start of the allocated memory arena. It will be set in `MyMemoryInit`.
    
- `static size_t g_arena_size;`: Total size of the arena.
    
- `static BlockHeader *g_first_block;`: Pointer to the header of the first block in the arena.
    

#### Helper function: `static BlockHeader *get_header_from_ptr(void *ptr)`

- This function should, given a pointer returned to the user (`ptr`), locate and return a pointer to the **block header** corresponding to that `ptr`.
    
- Remember that the header is placed **directly before** the usable memory, and its size is `ALIGN(sizeof(BlockHeader))`.
    

#### Implementation of `MyMemoryInit(size_t total_size)`:

1. Allocate the main memory arena with size `total_size` (remembering alignment) using standard `malloc`.
    
2. Check whether the allocation succeeded.
    
3. Initialize the first `BlockHeader` at the start of the arena: set its size to `total_size` (aligned), mark it as free (`is_free = 1`), and set the `next_block_ptr` and `prev_block_ptr` pointers accordingly.

#### Implementation of `MyMemoryCleanup()`:

1. Free the main memory arena using standard `free`.
    
2. Reset the global variables (`g_memory_arena`, `g_arena_size`, `g_first_block`) to `NULL` or `0`.
    

#### Implementation of `MyMalloc(size_t size)`:

1. Compute the **actual required block size**: `ALIGN(sizeof(BlockHeader))` (for header) + `ALIGN(size)` (for user data).
    
2. Scan blocks in the arena (starting from `g_first_block` and following `next_block_ptr`) to find the first free block that is large enough.
    
3. **If a suitable block is found:**
    
    - **Block splitting:** If the free block is significantly larger than required (i.e., the leftover space is large enough to hold a new `BlockHeader` plus minimal aligned user data size), split it.
        
        - Update the size and status of the allocated block.
            
        - Create a new `BlockHeader` for the remaining free part, setting its size and status.
            
        - Update `next_block_ptr` and `prev_block_ptr` for all three involved blocks (original allocated block, new free remainder, and the block following the original).
            
    - Mark the chosen block as occupied (`is_free = 0`).
        
    - Return a pointer to the **start of usable memory** (`(char*)header_ptr + ALIGN(sizeof(BlockHeader))`).
        
4. **If no suitable block is found:** Return `NULL`.
    

#### Implementation of `MyFree(void *ptr)`:

1. Handle the case `ptr == NULL`.
    
2. Locate the block header using `get_header_from_ptr`.
    
3. **Pointer validation:** Check if `ptr` is valid (e.g., points inside your arena and is not already free). If invalid, print an error and return.
    
4. Mark the block as free (`is_free = 1`).
    
5. **Coalescence:**
    
    - Check if the **next** block (`header->next_block_ptr`) is free. If yes, merge it with the current block: increase the current block size and update `next_block_ptr` and `prev_block_ptr` to bypass the absorbed block.
        
    - Check if the **previous** block (`header->prev_block_ptr`) is free. If yes, merge it with the current block: increase the previous block's size and update pointers to bypass the absorbed block. Remember, in this case the previous block becomes the new, enlarged free block.


#### Implementation of `MyCalloc(size_t num_elements, size_t element_size)`:

1. Calculate total size: `total_size = num_elements * element_size`. Be careful with **overflow** when multiplying!
    
2. Call `MyMalloc(total_size)`.
    
3. If allocation succeeds, use **your own `MyMemset`** to fill the allocated memory with zeros.
    
4. Return the pointer.
    

#### Implementation of `MyRealloc(void *ptr, size_t new_size)`:

1. **Special cases:**
    
    - If `ptr` is `NULL`, behave like `MyMalloc(new_size)`.
        
    - If `new_size` is `0`, behave like `MyFree(ptr)` and return `NULL`.
        
2. Locate the original block header and get its current user data size.
    
3. **If `new_size` is less than or equal to current size:**
    
    - You may simply return the original `ptr`. (More advanced would be to split the block and free the leftover, but that is optional.)
        
4. **If `new_size` is greater than current size:**
    
    - Allocate a new block with size `new_size` using `MyMalloc`.
        
    - If the allocation succeeds:
        
        - Copy data from the old block to the new block (`memcpy`). Copy `min(current_user_size, new_size)` bytes.
            
        - Free the old block using `MyFree(ptr)`.
            
        - Return the pointer to the new block.
            
    - If allocation fails, return `NULL` and **do not free the original block**.
        

#### Implementation of `MyMemset(void *s, int c, size_t n)`:

1. Implement the standard `memset` function: fill `n` bytes at memory pointed by `s` with value `c` (cast to `unsigned char`).
    
2. Return `s`.
    

#### Implementation of `MyMemoryDump()`:

1. Iterate through all blocks in the arena (starting from `g_first_block` using `next_block_ptr`).
    
2. For each block print: its offset from the start of the arena, total block size, user data size, status (free/occupied), and addresses of `next_block_ptr` and `prev_block_ptr`.
## 3 Main Program
### File: `main.c`
```c 
#include <stdio.h>    // For printf, fprintf
#include <stdlib.h>   // For random functions, if you want random tests
#include <string.h>   // For string functions, e.g. strcpy, strcmp (for test data)
#include <time.h>     // For initializing random number generator (optional)

#include "mymalloc.h" // Your own memory manager

// Define the size of the memory arena, e.g. 1MB
#define ARENA_SIZE (1024 * 1024)

int main() {
    // 1. Initial message
    //    Print a message informing about the start of tests.

    // 2. Initialize the memory manager
    //    Call MyMemoryInit with ARENA_SIZE.
    //    Check the error code returned by MyMemoryInit. If there is an error, print it and exit the program.
    //    Call MyMemoryDump() to see the initial state of the arena (it should be completely free).

    // --- MyMalloc Tests Section ---
    // 3. Simple allocations
    //    Declare several pointers (e.g. char*, int*, double*).
    //    Call MyMalloc for different sizes (e.g. 100 bytes, 50 ints, 10 doubles).
    //    For each allocation:
    //      - Check if the returned pointer is not NULL. If it is, that's an error.
    //      - Print the address of the allocated memory.
    //      - Use MyMemset (or simply assign values) to fill the allocated memory with data (e.g. 'A', numbers, etc.).
    //      - Print a snippet of the data to confirm writing.
    //    Call MyMemoryDump() after these allocations to see how the arena was divided.

    // 4. Test allocation that should fail
    //    Try to allocate more memory than the available arena (e.g. ARENA_SIZE * 2).
    //    Check if MyMalloc correctly returned NULL.

    // --- MyFree and Coalescing Tests Section ---
    // 5. Freeing blocks and basic coalescing
    //    Free one of the allocated blocks (e.g. the middle one if you have three).
    //    Call MyMemoryDump() to see that the block is free.
    //    Free another block that should be adjacent to the already freed block.
    //    Call MyMemoryDump() and check if the blocks have been merged (coalesced).
    //    Free remaining blocks and observe the arena state.

    // 6. Testing MyFree on invalid pointers
    //    Try calling MyFree on a pointer that:
    //      - Is NULL.
    //      - Was already freed (double free).
    //      - Was never allocated by MyMalloc (e.g. random address or stack address).
    //    Expect your MyFree to print an error but not crash the program.

    // --- MyCalloc Tests Section ---
    // 7. Testing MyCalloc
    //    Declare a pointer.
    //    Call MyCalloc to allocate an array of elements (e.g. 5 longs).
    //    Check if the returned pointer is not NULL.
    //    Print the first element's value to confirm it is zero.
    //    Iterate through all array elements and check if each one is zeroed.
    //    Call MyMemoryDump().

    // --- MyRealloc Tests Section ---
    // 8. Testing MyRealloc: increasing size
    //    Use one of the allocated blocks (from MyMalloc or MyCalloc).
    //    Call MyRealloc to increase its size (e.g. by 50% or 100 bytes).
    //    Check if the returned pointer is not NULL.
    //    Print the address of the new/modified block.
    //    Fill the newly allocated/enlarged part of the block with data.
    //    Verify if the original data was correctly copied.
    //    Call MyMemoryDump().

    // 9. Testing MyRealloc: decreasing size
    //    Use an allocated block.
    //    Call MyRealloc to shrink its size.
    //    Check if the returned pointer is valid.
    //    Call MyMemoryDump(). Notice if the block was split.

    // 10. Testing MyRealloc: special cases
    //     Call MyRealloc(NULL, size) and check if it works like MyMalloc.
    //     Call MyRealloc(ptr, 0) and check if it works like MyFree.
    //     Call MyRealloc on an invalid pointer.

    // --- Final Cleanup ---
    // 11. Free remaining memory
    //     Free all remaining active blocks you allocated in the tests.
    //     Call MyMemoryDump() to confirm that most (or all) of the arena is free.

    // 12. Cleanup the memory manager
    //     Call MyMemoryCleanup() to free the entire memory arena.
    //     Print a message about the end of tests.

    return 0;
}
```
