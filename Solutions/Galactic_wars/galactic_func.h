#ifndef GALACTIC_FUNC_H
#define GALACTIC_FUNC_H
#include <stddef.h>

// 1. struct fleet_status_t: Represents the status of a single fleet.
struct fleet_status_t {
  unsigned int status_flags;   // Bit-encoded fleet status flags. Bit 0: "Ready for Jump", Bit 1: "Shields Active", etc.
  unsigned short total_ships;  // Total number of ships in this fleet.
  char *fleet_name;            // Name of the fleet (e.g., "Red Squadron").
};


// 2. struct battle_t: Represents a single battle.
struct battle_t {
  char *battle_name;           // Name of the battle (e.g., "Battle of Coruscant").
  unsigned int battle_date;    // Date of the battle in YYYYMMDD format.
  struct fleet_status_t **fleet_statuses; // Array of POINTERS to struct fleet_status_t last element must be NULL
  size_t num_fleets;           // Number of fleets in fleet_statuses (excluding the trailing NULL).
};


// 3. struct battle_node_t: A node in a doubly linked list, storing battle data.
struct battle_node_t {
  struct battle_t *battle;         // Pointer to a battle_t structure .
  struct battle_node_t *prev;      // Pointer to the previous node in the list.
  struct battle_node_t *next;      // Pointer to the next node in the list.
};


// 4. struct galaxy_history_t: Main structure storing the entire history of wars.
struct galaxy_history_t {
  struct battle_node_t *head;      // Pointer to the head (beginning) of the battle list.
  struct battle_node_t *tail;      // Pointer to the tail (end) of the battle list.
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


#endif //GALACTIC_FUNC_H
