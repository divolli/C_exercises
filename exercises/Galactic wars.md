## **Task Guidelines: Galactic War History Management System**

Your goal is to implement an advanced system for managing data about galactic battles, with a strong focus on complex memory management, pointer operations, and bitwise manipulations.

---

### **Key Requirements**

#### **Data Structures**

You must implement **four main data structures** as described below:

- `struct fleet_status_t`:
    Contains **bit-coded status flags**, the **number of ships**, and the **fleet name**.

- `struct battle_t`:
    Contains the **battle name**, **date**, and an **array of pointers** to `struct fleet_status_t`.

- `struct battle_node_t`:
    A **doubly-linked list node** that stores a pointer to `struct battle_t`.

- `struct galaxy_history_t`:
    The **main structure**, which contains **pointers to the head and tail** of the battle list, as well as the **total number of battles**.


---

### **Memory and Pointer Management**

- ❌ **No `[]` operator**:
    Wherever possible, you must use **pointer arithmetic** instead of the indexing operator `[]`.

- 📦 **Dynamic allocation**:
    All `char*` fields (e.g., `fleet_name`, `battle_name`) must be **dynamically allocated** using functions like `strdup()` or `malloc()` + `strcpy()`.

- 🧩 **Nested allocation**:
    Memory must be allocated on **multiple levels**:

    - for `galaxy_history_t`,

    - for each `battle_node_t`,

    - for each `battle_t`,

    - for the **array of fleet_status pointers** in `battle_t`,

    - and finally for each individual `fleet_status_t`.

- 🔚 **NULL termination**:
    The array of `fleet_status_t*` pointers in `battle_t` **must end with a NULL pointer** — this is essential for proper iteration over fleets.

- 🎯 **Exact allocation**:
    Your data loading function must allocate **only as much memory as needed**, minimizing overallocation (e.g., by using `realloc()` after filling the array).

- 🧹 **Memory cleanup**:
    The function `destroy_galactic_history` must **free all allocated memory** across all levels of the data structure.
    This is the **most important part** to avoid memory leaks.
    It must also **work correctly even in case of partial data load** (e.g., after an error).


---

### **Parsing Data from File (`galactic_data.txt`)**

The file contains data about **battles**, their **dates**, and **fleets**.
The format is specific:

- Lines begin with `BATTLE:`, `DATE:`, and `FLEET:` (details were provided earlier).

- Each `FLEET:` line contains:

    - fleet name,

    - base status flag value (numeric),

    - number of ships,

    - and **verbal descriptions of additional status flags**, separated by `|`.


You must correctly **map these verbal descriptions to bits**, and set the appropriate bits in the `status_flags` field (in addition to the base numeric value).

#### **Mapping verbal status to bits:**

|Description|Bit|Value|
|---|---|---|
|`Ready_for_Jump`|0|1|
|`Shields_Active`|1|2|
|`Critical_Damage`|2|4|
|`Withdrawal`|3|8|

---

### **Function Implementation**

You must implement **all declared functions**, including:

- `initialize_history` – Initializes the main structure.

- `load_galactic_history` – Loads data from the file and builds the full data structure (a list of battles with arrays of fleets).
    Includes **robust error handling** (allocation errors, file corruption, etc.).

- `display_galactic_history` – Displays the contents of the entire data structure.

- `count_fleets_with_status_bits` – Bitwise function: counts fleets with specified status bits.

- `modify_fleet_statuses_in_battle` – Bitwise function: modifies fleet statuses in a given battle using `OR`, `AND NOT`, `XOR` operations with a bitmask.

- `destroy_galactic_history` – A key function that frees **all allocated memory**.


---

### **Task Goals**

The main goal is to demonstrate your skills in:

- Designing and implementing **complex nested data structures**.

- **Precise dynamic memory management** (allocation, reallocation, freeing) at multiple levels.

- Efficient usage of **pointers and pointer arithmetic**.

- Implementing **bitwise operations**.

- Writing **robust error handling**, especially for memory allocation failures.


---

### ✅ Example Data File
```
BATTLE:Battle of Yavin
DATE:19770525
FLEET:Red Squadron|0|12|Ready for Jump|Shields Active
FLEET:Gold Squadron|0|8|Shields Active
FLEET:Imperial Death Star Defense|0|100|Critical Damage
BATTLE:Siege of Hoth
DATE:19800521
FLEET:Rebel Ground Forces|0|50|Critical Damage|Withdrawal
FLEET:Imperial Blizzard Force|0|800|Shields Active|Ready for Jump
FLEET:Rogue Squadron|0|6|Ready for Jump
BATTLE:Skirmish at Ord Mantell
DATE:19820716
FLEET:Smuggler Alliance|0|15|Ready for Jump|Shields Active
FLEET:Imperial Blockade Fleet|0|30|Shields Active
BATTLE:Liberation of Kashyyyk
DATE:20050519
FLEET:Wookiee Freedom Fighters|0|200|Ready for Jump|Shields Active
FLEET:Republic Fleet Support|0|50|Shields Active
FLEET:Separatist Occupation Force|0|400|Critical Damage|Withdrawal
BATTLE:First Battle of Geonosis
DATE:20020516
FLEET:Republic Grand Army|0|1000|Ready for Jump|Shields Active
FLEET:Confederacy Droid Army|0|5000|Critical Damage
BATTLE:Battle of Coruscant
DATE:20050519
FLEET:Republic Home Fleet|0|3000|Ready for Jump|Shields Active
FLEET:Separatist Invasion Fleet|0|2500|Critical Damage
FLEET:Jedi Starfighter Corps|0|100|Ready for Jump|Shields Active
BATTLE:Clash at Mygeeto
DATE:20050519
FLEET:Galactic Marine Corps|0|150|Ready for Jump
FLEET:Separatist Artillery|0|300|Critical Damage
BATTLE:Fall of Umbara
DATE:20050519
FLEET:501st Legion|0|200|Shields Active|Ready for Jump
FLEET:Umbara Militia|0|180|Critical Damage|Withdrawa
```

## Functions Prototype
```c
#include <stdlib.h>
#include <stdio.h>


// 1. struct fleet_status_t: Represents the status of a single fleet.
struct fleet_status_t {
unsigned char status_flags;   // Bit-encoded fleet status flags.
                                // e.g., Bit 0: "Ready for Jump", Bit 1: "Shields Active", etc.
unsigned int total_ships;  // Total number of ships in this fleet.
char *fleet_name;            // Name of the fleet (e.g., "Red Squadron").
                                // Must be dynamically allocated (e.g., using strdup).
};

// 2. struct battle_t: Represents a single battle.
struct battle_t {
char *battle_name;           // Name of the battle (e.g., "Battle of Coruscant").
                                // Must be dynamically allocated (e.g., using strdup).
unsigned int battle_date;    // Date of the battle in YYYYMMDD format.
struct fleet_status_t **fleet_statuses; // Array of POINTERS to struct fleet_status_t.
                                        // This is a dynamically allocated array of pointers.
                                        // THE LAST ELEMENT OF THIS ARRAY MUST BE A NULL POINTER.
size_t num_fleets;           // Number of fleets in fleet_statuses (excluding the trailing NULL).
};

// 3. struct battle_node_t: A node in a doubly linked list, storing battle data.
struct battle_node_t {
struct battle_t *battle;         // Pointer to a battle_t structure.
                                    // Memory for battle_t must be allocated separately.
struct battle_node_t *prev;      // Pointer to the previous node in the list.
struct battle_node_t *next;      // Pointer to the next node in the list.
};

// 4. struct galaxy_history_t: Main structure storing the entire history of wars.
struct galaxy_history_t {
struct battle_node_t *head;      // Pointer to the head (beginning) of the battle list.
                                    // NULL if the list is empty.
struct battle_node_t *tail;      // Pointer to the tail (end) of the battle list.
                                    // NULL if the list is empty.
size_t total_battles;            // Total number of battles in the system.
};


// Initializes the galaxy_history_t structure.
// Returns 0 on success, 1 on error (e.g., NULL history_ptr).
int initialize_history(struct galaxy_history_t **history_ptr);

// Loads galactic war history data from a file.
// `fname`: Path to the file.
// `history_ptr`: Pointer to a pointer to the galaxy_history_t structure, which will be
//                initialized and populated with data.
// Returns: 0 - success, 1 - invalid input (NULL), 2 - file opening error,
//          3 - corrupted file format, 4 - memory allocation error.
int load_galactic_history(const char *fname, struct galaxy_history_t **history_ptr);

// Displays all battle data in the system.
// `history`: Pointer to the galaxy_history_t structure.
void display_galactic_history(const struct galaxy_history_t *history);

// Bitwise operation function: Returns the count of fleets that have at least one of the
// specified status bits set.
// `history`: Pointer to the galaxy_history_t structure.
// `mask`: The bitmask to check against.
// Returns: The count of fleets meeting the criteria, -1 on error (e.g., NULL history).
int count_fleets_with_status_bits(const struct galaxy_history_t *history, unsigned int mask);

// Bitwise operation function: Modifies the status of all fleets within a given battle.
// `history`: Pointer to the galaxy_history_t structure.
// `battle_name`: The name of the battle to find.
// `operation_type`: 0 - set bits (OR), 1 - clear bits (AND NOT), 2 - toggle bits (XOR).
// `mask`: The bitmask to apply the operation with.
// Returns: The number of fleets modified, -1 if battle not found or on error.
int modify_fleet_statuses_in_battle(struct galaxy_history_t *history, const char *battle_name, int operation_type, unsigned int mask);

// Frees all memory allocated for the galactic war history system.
// `history_ptr`: Pointer to a pointer to the galaxy_history_t structure.
void destroy_galactic_history(struct galaxy_history_t **history_ptr);


```
