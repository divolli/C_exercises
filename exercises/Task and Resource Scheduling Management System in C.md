## **Project Goal**

The goal of this project is to design and implement a system in C for managing **tasks** and **human resources** in the context of basic project scheduling.  
The system will track:

- **Tasks**, which have specific **dates**, **priorities**, and **statuses**
    
- **Human resources**, which are **assigned to tasks**
    

This project is designed to develop your skills in:

- Managing **complex data structures**
    
- Working with **linked lists** (both singly and doubly linked)
    
- **Date and time manipulation**
    
- **Precise dynamic memory management**
    

The program will be **interactive**, allowing data to be **loaded from and saved to files**.

---

## **File Structure**

The project should be organized into the following files:

- **`errors.h`** – Definitions of error codes (you can reuse this from a previous project if it fits).
    
- **`utils.h`** – Declarations of helper functions (e.g., for string manipulation, date handling, safe input reading).
    
- **`utils.c`** – Implementations of helper functions.
    
- **`tasks.h`** – Definition of the `Task` structure and prototypes for managing the task list (singly linked list).
    
- **`tasks.c`** – Implementations of functions that manage the `Task` list.
    
- **`resources.h`** – Definitions of the `Resource` and `TaskRef` structures, and prototypes for managing the resource list (doubly linked list).
    
- **`resources.c`** – Implementations of functions that manage the `Resource` list.
    
- **`main.c`** – Main program logic, including the interactive menu and command-line argument handling.

---

## 1. Helping function and error enumeration

### File: `errors.h` 
```c 
#ifndef ERRORS_H
#define ERRORS_H

// Definition of error codes using an enum.
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
``` c
#ifndef UTILS_H
#define UTILS_H

#include "errors.h"
#include <time.h> // For struct tm, time_t

// --- Date and Time Helper Structures and Functions ---

/**
 * @brief Simple date structure for consistency.
 * You could also directly use 'struct tm' from <time.h>.
 */
typedef struct Date {
    int year;
    int month;
    int day;
} Date;

/**
 * @brief Converts a Date structure to time_t.
 * @param date The Date structure.
 * @return time_t representation, or (time_t)-1 on error.
 */
time_t date_to_time_t(Date date);

/**
 * @brief Converts time_t to a Date structure.
 * @param t Time in time_t format.
 * @return Date structure.
 */
Date time_t_to_date(time_t t);

/**
 * @brief Compares two Date structures.
 * @return <0 if d1 is earlier, 0 if equal, >0 if d1 is later.
 */
int compare_dates(Date d1, Date d2);

/**
 * @brief Gets the current date as a Date structure.
 * @return Current Date.
 */
Date get_current_date();

/**
 * @brief Converts a Date structure to a formatted string (YYYY-MM-DD).
 * @param date The Date structure.
 * @param buffer Character buffer to store the formatted string.
 * @param buffer_size Size of the buffer.
 */
void format_date(Date date, char *buffer, size_t buffer_size);

/**
 * @brief Parses a date string (YYYY-MM-DD) into a Date structure.
 * @param date_str The date string.
 * @param date_out Pointer to the Date structure to fill.
 * @return SUCCESS on success, ERROR_INVALID_ARGUMENT on parsing failure.
 */
ErrorCode parse_date(const char *date_str, Date *date_out);


// --- General Utility Functions ---

/**
 * @brief Safely gets a string from stdin. Dynamically allocates memory.
 * The caller is responsible for freeing the returned string.
 * @return Pointer to the allocated string, or NULL on error.
 */
char* get_input_string();

/**
 * @brief Function pointer for comparing two string IDs (e.g., TaskID, ResourceID).
 * @return <0 if id1 < id2, 0 if equal, >0 if id1 > id2.
 */
typedef int (*StringIdCompareFunc)(const char *id1, const char *id2);

/**
 * @brief Function pointer for comparing two strings (e.g., Names).
 * Implement simple case-insensitivity.
 * @return <0 if name1 < name2, 0 if equal, >0 if name1 > name2.
 */
typedef int (*StringCompareFunc)(const char *name1, const char *name2);

// --- Default comparison functions (implement in utils.c) ---
int default_string_id_compare(const char *id1, const char *id2);
int default_string_compare(const char *name1, const char *name2);

#endif // UTILS_H
```

### File: `utils.c`
```c 
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // For tolower()

// --- Date and Time Helper Functions Implementations ---

time_t date_to_time_t(Date date) {
    // Your implementation: Convert Date to struct tm, then to time_t
    return (time_t)-1; // Placeholder
}

Date time_t_to_date(time_t t) {
    // Your implementation: Convert time_t to struct tm, then to Date
    Date d = {0, 0, 0};
    return d; // Placeholder
}

int compare_dates(Date d1, Date d2) {
    // Your implementation: Compare two Date structures
    return 0; // Placeholder
}

Date get_current_date() {
    // Your implementation: Get current system time and convert to Date
    Date d = {0, 0, 0};
    return d; // Placeholder
}

void format_date(Date date, char *buffer, size_t buffer_size) {
    // Your implementation: Format Date into YYYY-MM-DD string
}

ErrorCode parse_date(const char *date_str, Date *date_out) {
    // Your implementation: Parse YYYY-MM-DD string into Date
    return ERROR_OTHER; // Placeholder
}

// --- General Utility Functions Implementations ---

char* get_input_string() {
    // Your implementation: Safe string input using fgets and strdup
    return NULL; // Placeholder
}

int default_string_id_compare(const char *id1, const char *id2) {
    // Your implementation: Use strcmp for exact ID comparison
    return 0; // Placeholder
}

int default_string_compare(const char *name1, const char *name2) {
    // Your implementation: Case-insensitive string comparison (e.g., using tolower)
    return 0; // Placeholder
}
```

## 2. Tasks Structures & Functions

### File: `tasks.h`
```c
#ifndef TASKS_H
#define TASKS_H

#include <stdint.h>
#include "errors.h"
#include "utils.h" // For Date, StringIdCompareFunc
// Forward declaration of Resource structure (defined in resources.h)
// This is needed because Task contains a pointer to Resource, but Resource also contains a list of Tasks.
// This resolves the circular dependency.
typedef struct Resource Resource;

// --- Enums for Task Priority and Status ---
typedef enum {
    PRIORITY_LOW = 0,
    PRIORITY_MEDIUM,
    PRIORITY_HIGH
} TaskPriority;

typedef enum {
    STATUS_PENDING = 0,
    STATUS_IN_PROGRESS,
    STATUS_COMPLETED,
    STATUS_OVERDUE // Can be set automatically based on DueDate
} TaskStatus;

/**
 * @brief Structure representing a single task.
 */
typedef struct Task {
    char *task_id;          // Unique identifier for the task. Owns this memory.
    char *name;             // Name of the task. Owns this memory.
    char *description;      // Description of the task. Owns this memory.
    Date due_date;          // Due date of the task.
    TaskPriority priority;  // Priority of the task.
    TaskStatus status;      // Current status of the task.
    Resource *assigned_resource; // Pointer to the assigned resource (from the main resource list). Does NOT own this memory.
    struct Task *next;      // Pointer to the next task in the singly linked list.
} Task;

// --- Function Prototypes for Task List (Implement in tasks.c) ---

/**
 * @brief Creates a new Task node.
 * @param task_id Unique ID.
 * @param name Task name.
 * @param description Task description.
 * @param due_date Due date.
 * @param priority Task priority.
 * @return Pointer to the new node, or NULL on error.
 */
Task *create_task_node(const char *task_id, const char *name, const char *description, Date due_date, TaskPriority priority);

/**
 * @brief Inserts a new task into the list, maintaining it sorted by DueDate (earliest first), then Priority (HIGH first).
 * @param head Pointer to the pointer to the head of the Task list.
 * @param task_id ID of the task to insert.
 * @param name Task name.
 * @param description Task description.
 * @param due_date Due date.
 * @param priority Task priority.
 * @param id_compare_func Function for comparing TaskIDs.
 * @return ErrorCode.
 */
ErrorCode insert_task(Task **head, const char *task_id, const char *name, const char *description, Date due_date, TaskPriority priority, StringIdCompareFunc id_compare_func);

/**
 * @brief Finds a task in the list by its ID.
 * @param head Head of the Task list.
 * @param task_id ID of the task to find.
 * @param found_task Pointer to a pointer where the found task will be stored.
 * @param id_compare_func Function for comparing TaskIDs.
 * @return ErrorCode.
 */
ErrorCode find_task(Task *head, const char *task_id, Task **found_task, StringIdCompareFunc id_compare_func);

/**
 * @brief Deletes a task from the list by its ID.
 * Important: This function must also handle the removal of any *references* to this task from resources' assigned_tasks lists.
 * You will need a mechanism to access the main resource list for this.
 * @param head Pointer to the pointer to the head of the Task list.
 * @param task_id ID of the task to delete.
 * @param id_compare_func Function for comparing TaskIDs.
 * @return ErrorCode.
 */
ErrorCode delete_task(Task **head, const char *task_id, StringIdCompareFunc id_compare_func);

/**
 * @brief Frees all memory occupied by the Task list.
 * @param head Pointer to the pointer to the head of the Task list.
 */
void clear_tasks(Task **head);

/**
 * @brief Prints all tasks in the list. Includes assigned resource name if any.
 * @param head Head of the Task list.
 */
void print_tasks(Task *head);

/**
 * @brief Updates the status of tasks, automatically setting to OVERDUE if due date passed.
 * @param head Head of the Task list.
 */
void update_task_statuses(Task *head);

/**
 * @brief Assigns a resource to a task.
 * @param task_head Head of the main Task list.
 * @param resource_head Head of the main Resource list.
 * @param task_id ID of the task to assign.
 * @param resource_id ID of the resource to assign.
 * @param task_id_compare_func Function for comparing TaskIDs.
 * @param resource_id_compare_func Function for comparing ResourceIDs.
 * @return ErrorCode.
 */
ErrorCode assign_resource_to_task(Task *task_head, Resource *resource_head, const char *task_id, const char *resource_id, StringIdCompareFunc task_id_compare_func, StringIdCompareFunc resource_id_compare_func);

/**
 * @brief Removes resource assignment from a task.
 * @param task_head Head of the main Task list.
 * @param resource_head Head of the main Resource list.
 * @param task_id ID of the task to unassign.
 * @param task_id_compare_func Function for comparing TaskIDs.
 * @return ErrorCode.
 */
ErrorCode unassign_resource_from_task(Task *task_head, Resource *resource_head, const char *task_id, StringIdCompareFunc task_id_compare_func);


/**
 * @brief Loads tasks from a file into the list.
 * File format (example): task_id name description YYYY-MM-DD priority status [resource_id]
 * You MUST ensure that the Resource list (main_resource_list_head) is already loaded before calling this!
 * @param head Pointer to the pointer to the head of the Task list.
 * @param filepath Path to the file.
 * @param task_id_compare_func Function for comparing TaskIDs (for insertion).
 * @param main_resource_list_head Head of the main Resource list (for finding assigned resources).
 * @param resource_id_compare_func Function for comparing ResourceIDs.
 * @return ErrorCode.
 */
ErrorCode load_tasks_from_file(Task **head, const char *filepath, StringIdCompareFunc task_id_compare_func, Resource *main_resource_list_head, StringIdCompareFunc resource_id_compare_func);

/**
 * @brief Saves tasks from the list to a file.
 * @param head Head of the Task list.
 * @param filepath Path to the file.
 * @return ErrorCode.
 */
ErrorCode save_tasks_to_file(Task *head, const char *filepath);

#endif // TASKS_H
```

### File: `tasks.c`
``` c
#include "tasks.h"
#include "resources.h" // Needed for Resource definition, even if forward declared in .h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Implementations for Task List functions ---

Task *create_task_node(const char *task_id, const char *name, const char *description, Date due_date, TaskPriority priority) {
    // Your implementation
    return NULL; // Placeholder
}

ErrorCode insert_task(Task **head, const char *task_id, const char *name, const char *description, Date due_date, TaskPriority priority, StringIdCompareFunc id_compare_func) {
    // Your implementation (sort by DueDate, then Priority)
    return ERROR_OTHER; // Placeholder
}

ErrorCode find_task(Task *head, const char *task_id, Task **found_task, StringIdCompareFunc id_compare_func) {
    // Your implementation
    return ERROR_OTHER; // Placeholder
}

ErrorCode delete_task(Task **head, const char *task_id, StringIdCompareFunc id_compare_func) {
    // Your implementation (remember to handle assigned_tasks in resources!)
    return ERROR_OTHER; // Placeholder
}

void clear_tasks(Task **head) {
    // Your implementation
}

void print_tasks(Task *head) {
    // Your implementation
}

void update_task_statuses(Task *head) {
    // Your implementation
}

// NOTE: These two functions (assign/unassign resource to task) might need to directly interact
//       with the main resource list's internal TaskRef list.
//       This may require passing the Resource head and resource_id_compare_func,
//       or making some Resource functions accessible.
//       This is a design decision you need to make.
ErrorCode assign_resource_to_task(Task *task_head, Resource *resource_head, const char *task_id, const char *resource_id, StringIdCompareFunc task_id_compare_func, StringIdCompareFunc resource_id_compare_func) {
    // Your implementation
    return ERROR_OTHER; // Placeholder
}

ErrorCode unassign_resource_from_task(Task *task_head, Resource *resource_head, const char *task_id, StringIdCompareFunc task_id_compare_func) {
    // Your implementation
    return ERROR_OTHER; // Placeholder
}

// NOTE: The main_resource_list_head is passed to load_tasks_from_file because tasks might
//       have an assigned resource that needs to be found in the main resource list
//       to set the assigned_resource pointer correctly.
ErrorCode load_tasks_from_file(Task **head, const char *filepath, StringIdCompareFunc task_id_compare_func, Resource *main_resource_list_head, StringIdCompareFunc resource_id_compare_func) {
    // Your implementation
    return ERROR_OTHER; // Placeholder
}

ErrorCode save_tasks_to_file(Task *head, const char *filepath) {
    // Your implementation
    return ERROR_OTHER; // Placeholder
}
```

## 3. Structure & Functions

### File: `resources.h`
```c 
#ifndef RESOURCES_H
#define RESOURCES_H

#include <stdint.h>
#include "errors.h"
#include "utils.h" // For StringIdCompareFunc, StringCompareFunc
#include "tasks.h" // For Task structure, as Resource has a list of Task pointers

// --- Enums for Resource Role ---
typedef enum {
    ROLE_DEVELOPER = 0,
    ROLE_TESTER,
    ROLE_MANAGER,
    ROLE_OTHER
} ResourceRole;

/**
 * @brief Helper structure to create a list of tasks assigned to a resource.
 * Contains a POINTER to a Task from the MAIN Task list.
 * Does NOT own the memory pointed to by 'task_ptr'.
 */
typedef struct TaskRef {
    Task *task_ptr;       // Pointer to a task from the main Task list.
    struct TaskRef *next; // Pointer to the next task reference in the resource's list.
} TaskRef;

/**
 * @brief Structure representing a resource record in a doubly linked list.
 */
typedef struct Resource {
    char *resource_id;      // Unique identifier for the resource. Owns this memory.
    char *name;             // Name of the resource. Owns this memory.
    ResourceRole role;      // Role of the resource.
    float availability_hours; // Availability in hours (e.g., 8.0, 4.5).
    TaskRef *assigned_tasks; // Head of a singly linked list of pointers to Tasks that this resource is assigned to.
                             // THIS list (TaskRef nodes) is allocated and freed BY the Resource,
                             // but the *Task pointed to* is NOT.
    struct Resource *prev;  // Pointer to the previous record in the doubly linked list.
    struct Resource *next;  // Pointer to the next record in the doubly linked list.
} Resource;

// --- Function Prototypes for Resource List (Implement in resources.c) ---

/**
 * @brief Creates a new Resource node.
 * @param resource_id Unique ID.
 * @param name Resource name.
 * @param role Resource role.
 * @param availability_hours Daily availability in hours.
 * @return Pointer to the new node, or NULL on error.
 */
Resource *create_resource_node(const char *resource_id, const char *name, ResourceRole role, float availability_hours);

/**
 * @brief Inserts a new resource into the doubly linked list, maintaining alphabetical order by name.
 * @param head Pointer to the pointer to the head of the Resource list.
 * @param resource_id ID of the resource to insert.
 * @param name Resource name.
 * @param role Resource role.
 * @param availability_hours Availability in hours.
 * @param id_compare_func Function for comparing ResourceIDs.
 * @param name_compare_func Function for comparing resource names.
 * @return ErrorCode.
 */
ErrorCode insert_resource(Resource **head, const char *resource_id, const char *name, ResourceRole role, float availability_hours, StringIdCompareFunc id_compare_func, StringCompareFunc name_compare_func);

/**
 * @brief Finds a resource in the list by its ID.
 * @param head Head of the Resource list.
 * @param resource_id ID of the resource to find.
 * @param found_resource Pointer to a pointer where the found resource will be stored.
 * @param id_compare_func Function for comparing ResourceIDs.
 * @return ErrorCode.
 */
ErrorCode find_resource(Resource *head, const char *resource_id, Resource **found_resource, StringIdCompareFunc id_compare_func);

/**
 * @brief Deletes a resource from the list by its ID.
 * Important: This function must also **unassign this resource from ALL tasks** it was previously assigned to.
 * You will need a mechanism to access the main task list for this.
 * @param head Pointer to the pointer to the head of the Resource list.
 * @param resource_id ID of the resource to delete.
 * @param id_compare_func Function for comparing ResourceIDs.
 * @return ErrorCode.
 */
ErrorCode delete_resource(Resource **head, const char *resource_id, StringIdCompareFunc id_compare_func);

/**
 * @brief Frees all memory occupied by the Resource list.
 * Must correctly free Resource nodes and their internal TaskRef lists,
 * but NOT the Task nodes that these references point to.
 * @param head Pointer to the pointer to the head of the Resource list.
 */
void clear_resources(Resource **head);

/**
 * @brief Prints all resources in the list, including their assigned tasks.
 * @param head Head of the Resource list.
 */
void print_resources(Resource *head);

/**
 * @brief Assigns an existing task to a resource. Adds a reference to the resource's assigned_tasks list.
 * This should be called *from* assign_resource_to_task in tasks.c, to link both ways.
 * @param resource_node Pointer to the resource to which the task will be assigned.
 * @param task_ptr Pointer to the task to be assigned.
 * @return ErrorCode.
 */
ErrorCode add_task_to_resource_assignment_list(Resource *resource_node, Task *task_ptr);

/**
 * @brief Removes a task assignment from a resource (removes the reference from the resource's assigned_tasks list).
 * This should be called *from* unassign_resource_from_task or delete_task in tasks.c.
 * @param resource_node Pointer to the resource from which the task assignment will be removed.
 * @param task_id ID of the task to remove.
 * @param task_id_compare_func Function for comparing TaskIDs.
 * @return ErrorCode.
 */
ErrorCode remove_task_from_resource_assignment_list(Resource *resource_node, const char *task_id, StringIdCompareFunc task_id_compare_func);

/**
 * @brief Loads resources from a file into the list.
 * File format (example): resource_id name role availability_hours [task_id1 task_id2 ...]
 * You MUST ensure that the Task list (main_task_list_head) is already loaded before calling this!
 * @param head Pointer to the pointer to the head of the Resource list.
 * @param filepath Path to the file.
 * @param id_compare_func Function for comparing ResourceIDs (for insertion).
 * @param name_compare_func Function for comparing resource names (for insertion).
 * @param main_task_list_head Head of the main Task list (for finding assigned tasks).
 * @param task_id_compare_func Function for comparing TaskIDs.
 * @return ErrorCode.
 */
ErrorCode load_resources_from_file(Resource **head, const char *filepath, StringIdCompareFunc id_compare_func, StringCompareFunc name_compare_func, Task *main_task_list_head, StringIdCompareFunc task_id_compare_func);

/**
 * @brief Saves resources from the list to a file.
 * @param head Head of the Resource list.
 * @param filepath Path to the file.
 * @return ErrorCode.
 */
ErrorCode save_resources_to_file(Resource *head, const char *filepath);

#endif // RESOURCES_H
```

### File: `resources.c`
```c
#include "resources.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Implementations for Resource List functions ---

Resource *create_resource_node(const char *resource_id, const char *name, ResourceRole role, float availability_hours) {
    // Your implementation
    return NULL; // Placeholder
}

ErrorCode insert_resource(Resource **head, const char *resource_id, const char *name, ResourceRole role, float availability_hours, StringIdCompareFunc id_compare_func, StringCompareFunc name_compare_func) {
    // Your implementation (sort by name)
    return ERROR_OTHER; // Placeholder
}

ErrorCode find_resource(Resource *head, const char *resource_id, Resource **found_resource, StringIdCompareFunc id_compare_func) {
    // Your implementation
    return ERROR_OTHER; // Placeholder
}

ErrorCode delete_resource(Resource **head, const char *resource_id, StringIdCompareFunc id_compare_func) {
    // Your implementation (remember to unassign from ALL tasks!)
    return ERROR_OTHER; // Placeholder
}

void clear_resources(Resource **head) {
    // Your implementation
}

void print_resources(Resource *head) {
    // Your implementation
}

// NOTE: These two functions (add/remove task to resource's assignment list)
//       are primarily called by tasks.c functions to manage the bidirectional links.
ErrorCode add_task_to_resource_assignment_list(Resource *resource_node, Task *task_ptr) {
    // Your implementation
    return ERROR_OTHER; // Placeholder
}

ErrorCode remove_task_from_resource_assignment_list(Resource *resource_node, const char *task_id, StringIdCompareFunc task_id_compare_func) {
    // Your implementation
    return ERROR_OTHER; // Placeholder
}

// NOTE: The main_task_list_head is passed to load_resources_from_file because resources might
//       have assigned tasks that need to be found in the main task list
//       to set the task_ptr correctly.
ErrorCode load_resources_from_file(Resource **head, const char *filepath, StringIdCompareFunc id_compare_func, StringCompareFunc name_compare_func, Task *main_task_list_head, StringIdCompareFunc task_id_compare_func) {
    // Your implementation
    return ERROR_OTHER; // Placeholder
}

ErrorCode save_resources_to_file(Resource *head, const char *filepath) {
    // Your implementation
    return ERROR_OTHER; // Placeholder
}
```

## 4. Main Program

### File: `main.c` 
``` c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // For tolower() if used in default_string_compare

#include "errors.h"
#include "tasks.h"
#include "resources.h"
#include "utils.h"

// Define an error code for missing arguments (if running from CLI)
#define NOT_ENOUGH_ARGS_ERROR 9 // Or use an enum from errors.h

// --- Prototypes of helper menu functions ---
// (Implement these functions below main())
void print_main_menu();
void print_task_menu();
void print_resource_menu();

// --- Main program function ---
int main(int argc, char *argv[]) {
    Task *task_list_head = NULL;
    Resource *resource_list_head = NULL;
    int choice;
    ErrorCode err;

    // --- Command-line arguments processing (Implement this block) ---
    // Expected usage: ./main [path_to_resources_file] [path_to_tasks_file]
    // IMPORTANT: Resources must be loaded BEFORE tasks, as tasks may reference resources.
    // So, if argc == 3:
    // 1. Call load_resources_from_file().
    // 2. Then call load_tasks_from_file().
    // Otherwise, print usage info if argc > 1.


    // --- Main interactive loop ---
    do {
        print_main_menu();
        // Get user choice (integer)
        // Handle invalid input (non-integer, clear buffer)

        switch (choice) {
            case 1: { // Manage Tasks
                int task_choice;
                do {
                    print_task_menu();
                    // Get task menu choice (integer)
                    // Handle invalid input (non-integer, clear buffer)

                    switch (task_choice) {
                        case 1: { // Add Task
                            // Get task_id, name, description, due_date_str, priority_str from user
                            // Parse date string to Date struct (use parse_date from utils.h)
                            // Convert priority string to TaskPriority enum
                            // Call insert_task()
                            // Free allocated input strings
                            break;
                        }
                        case 2: { // Find Task
                            // Get task_id from user
                            // Call find_task()
                            // Print task details or "not found"
                            // Free allocated input string
                            break;
                        }
                        case 3: { // Update Task Status
                            // Get task_id and new status from user
                            // Find task, then update its status field
                            // Free allocated input string
                            break;
                        }
                        case 4: { // Delete Task
                            // Get task_id from user
                            // Call delete_task()
                            // Free allocated input string
                            break;
                        }
                        case 5: { // Assign Resource to Task
                            // Get task_id and resource_id from user
                            // Call assign_resource_to_task()
                            // Free allocated input strings
                            break;
                        }
                        case 6: { // Unassign Resource from Task
                            // Get task_id from user
                            // Call unassign_resource_from_task()
                            // Free allocated input string
                            break;
                        }
                        case 7: { // Display All Tasks
                            // Call print_tasks()
                            break;
                        }
                        case 8: { // Update All Task Statuses (e.g., check for overdue)
                            // Call update_task_statuses()
                            break;
                        }
                        case 9: // Back
                            break;
                        default:
                            printf("Invalid option.\n");
                            break;
                    }
                } while (task_choice != 9);
                break;
            }
            case 2: { // Manage Resources
                int resource_choice;
                do {
                    print_resource_menu();
                    // Get resource menu choice (integer)
                    // Handle invalid input (non-integer, clear buffer)

                    switch (resource_choice) {
                        case 1: { // Add Resource
                            // Get resource_id, name, role_str, availability_str from user
                            // Convert role string to ResourceRole enum, availability string to float
                            // Call insert_resource()
                            // Free allocated input strings
                            break;
                        }
                        case 2: { // Find Resource
                            // Get resource_id from user
                            // Call find_resource()
                            // Print resource details or "not found"
                            // Free allocated input string
                            break;
                        }
                        case 3: { // Update Resource Availability
                            // Get resource_id and new availability from user
                            // Find resource, then update its availability_hours field
                            // Free allocated input string
                            break;
                        }
                        case 4: { // Delete Resource
                            // Get resource_id from user
                            // Call delete_resource()
                            // Free allocated input string
                            break;
                        }
                        case 5: { // Display All Resources
                            // Call print_resources()
                            break;
                        }
                        case 6: // Back
                            break;
                        default:
                            printf("Invalid option.\n");
                            break;
                    }
                } while (resource_choice != 6);
                break;
            }
            case 3: { // Load Data
                // Get file paths for resources and tasks from user input
                // IMPORTANT: Load resources first, then tasks.
                // Call load_resources_from_file()
                // Call load_tasks_from_file()
                // Free allocated input strings
                break;
            }
            case 4: { // Save Data
                // Get file paths for resources and tasks from user input
                // Call save_resources_to_file()
                // Call save_tasks_to_file()
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
    // IMPORTANT: First, free tasks' assigned_resource pointers (set to NULL),
    //            then clear tasks, then clear resources.
    //            Or simply ensure clear_tasks handles assigned_resource pointers
    //            correctly without freeing the resource itself.
    //            The key is to avoid freeing the same memory twice.
    //            A good strategy is to clear the list that CONTAINS references FIRST,
    //            then the list that OWNS the actual data.
    //            In this case, Task nodes reference Resources, and Resource nodes reference Tasks.
    //            This mutual referencing requires careful cleanup.
    //            Perhaps a two-pass approach:
    //            1. Iterate through tasks, set their assigned_resource to NULL (just the pointer).
    //            2. Clear the tasks list (frees Task nodes).
    //            3. Iterate through resources, set their assigned_tasks to NULL (just the list of refs).
    //            4. Clear the resources list (frees Resource nodes).
    //            Or rely on the robust delete/clear functions to handle cross-list updates.
    //            A simpler approach for cleanup might be:
    //            Clear users (which clears their TaskRefs, but not actual Tasks).
    //            Then clear tasks (which frees actual Task nodes).
    clear_tasks(&task_list_head); // This function should also ensure resources no longer point to deleted tasks
    clear_resources(&resource_list_head); // This function should also ensure tasks no longer point to deleted resources

    return 0;
}


// --- Implementations of helper menu functions ---

void print_main_menu() {
    printf("\n--- Task and Resource Management System ---\n");
    printf("1. Manage Tasks\n");
    printf("2. Manage Resources\n");
    printf("3. Load Data from Files\n");
    printf("4. Save Data to Files\n");
    printf("5. Exit\n");
    printf("Choose option: ");
}

void print_task_menu() {
    printf("\n--- Task Management ---\n");
    printf("1. Add new task\n");
    printf("2. Find task\n");
    printf("3. Update task status\n");
    printf("4. Delete task\n");
    printf("5. Assign resource to task\n");
    printf("6. Unassign resource from task\n");
    printf("7. Display all tasks\n");
    printf("8. Update all task statuses (check for overdue)\n");
    printf("9. Back to main menu\n");
    printf("Choose option: ");
}

void print_resource_menu() {
    printf("\n--- Resource Management ---\n");
    printf("1. Add new resource\n");
    printf("2. Find resource\n");
    printf("3. Update resource availability\n");
    printf("4. Delete resource\n");
    printf("5. Display all resources\n");
    printf("6. Back to main menu\n");
    printf("Choose option: ");
}

// get_input_string() is declared in utils.h and implemented in utils.c
```

## **Technical Requirements and Additional Guidelines**

- **No `[]` (Array Indexing)**:  
    Consistently use **pointer arithmetic** for all list manipulations and string operations.
    

---

### **Memory Management**

- Every memory allocation (`malloc`, `calloc`, `strdup`) must be **checked for `NULL`**.
    
- All strings inside structures (`task_id`, `name`, `description`, `resource_id`) must be **deep copied** (using `strdup` or `malloc` + `strcpy`).  
    List nodes are the **owners** of this memory and are responsible for freeing it.
    
- The functions `clear_tasks` and `clear_resources` (as well as `delete_task`, `delete_resource`) must **carefully free all allocated memory** to prevent memory leaks.
    
- **Critical for `assigned_resource` and `assigned_tasks`:**
    
    - `Task->assigned_resource` is a **pointer** to a `Resource` node in the main resource list.  
        You **must not free** the memory pointed to by this field when freeing a task.
        
    - `Resource->assigned_tasks` is a list of `TaskRef` elements, each holding a **pointer** to a `Task` node in the main task list.  
        You **must not free** the memory pointed to by `TaskRef->task_ptr` when freeing a resource or a `TaskRef`.
        
    - The delete functions (`delete_task`, `delete_resource`) must **update bidirectional relationships**:
        
        - When deleting a task, you must locate the resource it was assigned to and remove the reference to this task from the resource’s `assigned_tasks` list.
            
        - When deleting a resource, you must locate all tasks it was assigned to and set their `assigned_resource` to `NULL`.
            

---

### **Date and Time (`<time.h>`)**

Use `struct tm`, `time_t`, `mktime`, `strftime`, `localtime`, and `time()` for:

- Parsing dates from strings (e.g., "YYYY-MM-DD") into `Date` or `struct tm` structures.
    
- Formatting dates from structures to strings for display/saving.
    
- Comparing dates (e.g., to determine if a task is `OVERDUE`).
    
- Retrieving the current system date.
    

---

### **Function Pointers**

You must use `StringIdCompareFunc` and `StringCompareFunc` to dynamically pass comparison functions.

---

### **Error Handling**

All functions that return `ErrorCode` must correctly handle errors and return appropriate values from `errors.h`.

---

### **Sorting**

- The `Task` list should be sorted by `DueDate` (earliest tasks first), then by `Priority`  
    (higher priority first, e.g., `HIGH > MEDIUM > LOW`).
    
- The `Resource` list should be sorted alphabetically by `Name`.
    

---

### **ID Uniqueness**

Ensure that duplicate `TaskID` and `ResourceID` values **cannot** be added.

---

### **File Loading/Saving**

- Define a simple text format for both types of data (e.g., space-separated or tab-delimited).
    
- **Loading order is critical**: Always load **resources first**, then **tasks**.  
    Why? Because tasks may reference an assigned resource. The resource must already exist in memory so the `Task->assigned_resource` pointer can be correctly set.  
    Similarly, if a resource lists assigned tasks, those tasks must already exist.
    
    This means your `load_tasks_from_file` and `load_resources_from_file` functions should accept a pointer to the head of the other list as an argument.
    

---

### **Interactive Console Interface**

Create a user-friendly text-based menu (similar to your previous project) for managing tasks and resources interactively.

---

### **`main` Function Cleanup**

Ensure that `clear_tasks` and `clear_resources` are called correctly upon program exit to free all memory and avoid leaks.  
⚠️ The **order matters** due to the bidirectional references: you should always **clear tasks first, then resources**.

---

## **Suggested Implementation Process**

1. **Setup**: Create all `.h` and `.c` files according to the structure above.
    
2. **`errors.h`**: Copy or define your `ErrorCode` enumeration.
    
3. **`utils.h` / `utils.c`**:  
    Implement all utility functions, especially for handling strings and dates. Test them separately.
    
4. **`tasks.h` / `resources.h`**:  
    Ensure correct forward declarations and definitions of all enums and structures.
    
5. **Initial `tasks.c` Implementation**:
    
    - Start with `create_task_node`, `insert_task` (initially without sorting; add sorting and ID duplication checks later), `print_tasks`, `clear_tasks`.  
        Test thoroughly.
        
6. **Initial `resources.c` Implementation**:
    
    - Similarly: `create_resource_node`, `insert_resource`, `print_resources`, `clear_resources`.  
        Add sorting and duplicate ID checks later. Test well.
        
7. **`main.c`**:
    
    - Implement the main menu, task menu, and resource menu.
        
    - Connect basic features (adding, displaying, clearing).
        
    - Focus on building the interactive loop.
        
8. **Further Development in `tasks.c` and `resources.c`**:
    
    - Implement `find_task`, `delete_task`, `find_resource`, `delete_resource`.  
        Pay close attention to memory management and updating linked references.
        
    - Implement `assign_resource_to_task` and `unassign_resource_from_task`, and their counterparts in `resources.c`:  
        `add_task_to_resource_assignment_list`, `remove_task_from_resource_assignment_list`.  
        This requires careful handling of the bidirectional relationship.
        
    - Implement `update_task_statuses` (use `get_current_date` and `compare_dates` for detecting overdue tasks).
        
9. **File Handling (`load`/`save`)**:
    
    - Implement `load_resources_from_file`, `load_tasks_from_file`, `save_resources_to_file`, `save_tasks_to_file`.  
        Remember the **loading order** and the need to pass the head of the other list for cross-referencing.
        
10. **Refactor and Test**:
    

- Thoroughly test the entire system.
    
- Use tools like `valgrind` to detect memory leaks.
    
- Pay special attention to edge cases: empty lists, deleting the only element, deleting tasks or resources with references.
    

---

## **Sample Files**

### **Sample Data for `resources.txt`**

This file contains a list of human resources.  
Each line represents one resource and includes the following:

- **Resource ID**
    
- **Full name**
    
- **Role**
    
- **Availability in hours**
**`resources.txt`**

```
RES001 Jan_Kowalski DEVELOPER 8.0
RES002 Anna_Nowak TESTER 7.5
RES003 Piotr_Zielinski MANAGER 6.0
RES004 Alicja_Wojcik DEVELOPER 8.0
RES005 Marek_Dabrowski TESTER 7.0
RES006 Kasia_Lis OTHER 4.0
RES007 Bartek_Wrobel DEVELOPER 8.0
RES008 Dominika_Szymanska MANAGER 5.0
RES009 Grzegorz_Nowicki DEVELOPER 8.0
RES010 Natalia_Kowalska TESTER 7.0
RES011 Filip_Mazur DEVELOPER 8.0
RES012 Ewa_Wojtczak MANAGER 6.5
RES013 Adam_Nowak DEVELOPER 8.0
RES014 Beata_Kowalik TESTER 7.0
RES015 Cezary_Wisniewski DEVELOPER 7.0
RES016 Dorota_Maciejewska MANAGER 5.5
RES017 Edward_Duda DEVELOPER 8.0
RES018 Franciszek_Krawczyk TESTER 7.5
RES019 Gabriela_Lesniak DEVELOPER 8.0
RES020 Hubert_Majewski MANAGER 6.0
```


---

## **Sample Data for `tasks.txt`**

This file contains a list of tasks.  
Each line represents a single task and consists of the following fields:

- **Task ID**
    
- **Name**
    
- **Description** (either a single word or multiple words joined with underscores)
    
- **Due date** (`YYYY-MM-DD`)
    
- **Priority**
    
- **Status**
    
- Optionally: **Assigned Resource ID**

**`tasks.txt`**
```
TASK001 Implement_Login_Module 2025-07-10 HIGH PENDING RES001
TASK002 Write_Unit_Tests 2025-07-12 MEDIUM IN_PROGRESS RES002
TASK003 Design_Database_Schema 2025-07-05 HIGH COMPLETED RES003
TASK004 Prepare_Deployment_Plan 2025-07-01 LOW OVERDUE RES003 ; Przykładowe zadanie przeterminowane
TASK005 Develop_User_Profile 2025-07-15 HIGH PENDING RES004
TASK006 Review_Code_Base 2025-07-18 MEDIUM PENDING RES001
TASK007 Fix_Bug_123 2025-07-08 HIGH IN_PROGRESS RES002
TASK008 Create_Reports_Module 2025-07-20 MEDIUM PENDING RES001
TASK009 Test_Integration 2025-07-22 HIGH PENDING RES005
TASK010 Document_API 2025-07-25 LOW PENDING RES006
TASK011 Optimize_Performance 2025-07-30 HIGH PENDING RES007
TASK012 UI_Redesign 2025-08-01 MEDIUM PENDING RES008
TASK013 Feature_X_Implementation 2025-08-05 HIGH PENDING RES009
TASK014 Regression_Testing 2025-08-08 MEDIUM PENDING RES010
TASK015 Database_Migration 2025-08-10 HIGH PENDING RES011
TASK016 Client_Meeting 2025-08-12 LOW PENDING RES012
TASK017 Research_New_Tech 2025-08-15 MEDIUM PENDING RES013
TASK018 Performance_Testing 2025-08-18 HIGH PENDING RES014
TASK019 Backend_Refactoring 2025-08-20 MEDIUM PENDING RES015
TASK020 Setup_CI_CD 2025-08-22 HIGH PENDING RES016
TASK021 User_Training 2025-08-25 LOW PENDING RES017
TASK022 Security_Audit 2025-08-28 HIGH PENDING RES018
TASK023 Final_Documentation 2025-08-30 MEDIUM PENDING RES019
TASK024 Budget_Review 2025-09-01 LOW PENDING RES020
TASK025 Debug_Critical_Issue 2025-07-02 HIGH IN_PROGRESS NON_EXISTENT_RES ; Przykładowe zadanie z nieistniejącym zasobem
```


---

## **Important Notes for `tasks.txt`**

- **Task `TASK004`** (`Prepare_Deployment_Plan`) has a due date of `2025-07-01`, which is **today**.  
    This means it could be automatically marked as `OVERDUE` when the program starts, depending on how you've implemented status updating.  
    It's a good **test case** for date handling and automatic status updates.
    
- **Task `TASK025`** (`Debug_Critical_Issue`) is assigned to a resource `NON_EXISTENT_RES`, which **does not exist** in the sample `resources.txt` file.  
    Your `load_tasks_from_file` function should handle this case.  
    The best approach is to still load the task, but leave the `assigned_resource` set to `NULL`, and display a **warning** on the console about the unknown resource.  
    This is a critical **test case for the robustness** of your loading logic.
    
- Keep in mind that the file format is only a suggestion.  
    You are free to adjust it to your needs, as long as your loading and saving functions correctly support the chosen format.