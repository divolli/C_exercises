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
