#include "galactic_func.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global strings for setting fleet status and printing
char *first_bit = "Ready for Jump";
char *second_bit = "Shield Active";
char *third_bit = "Critical Damage";
char *fourth_bit = "Withdrawal";
// Function to set a fleet status
unsigned char set_fleet_status(char *line){
  if (!line) return -1;

  unsigned char status = 0;
  if (strstr(line, first_bit)){
    //set first bit to one
    status = status | (1 << 0);
  }
  if (strstr(line, second_bit)){
    //set second bit to one
    status = status | (1 << 1);
  }
  if (strstr(line, third_bit)){
    //set third bit to one
    status = status | (1 << 2);
  }
  if (strstr(line, fourth_bit)){
    //set fourth bit to one
    status = status | (1 << 3);
  }

  return status;
}

// Helper functions for double linked list structure.........


// creates new battle node and filles the data
struct battle_node_t* create_new_battle(char *battle_name, unsigned int battle_date){
  if (!battle_name) return NULL;

  // Allocating memory for node
  struct battle_node_t *node = (struct battle_node_t *) malloc(sizeof(struct battle_node_t));
  if (!node) return NULL;

  // Allocating memory for battle
  node->battle = (struct battle_t *) malloc(sizeof(struct battle_t));
  if (!node->battle){
    free(node);
    return NULL;
  }
  node->next = NULL;
  node->prev = NULL;

  // Writing data to a battle struct
  struct battle_t *curr_battle = node->battle;
  curr_battle->battle_date = battle_date;
  curr_battle->num_fleets = 0;
  curr_battle->battle_name = strdup(battle_name);
  if (!curr_battle->battle_name){
    free(node->battle);
    free(node);
    return NULL;
  }

  // Allocating memory for fleet statuses struct
  curr_battle->fleet_statuses = (struct fleet_status_t **) calloc(4, sizeof(struct fleet_status_t *)); // Capacity for 0 fleets + NULL, or 1 fleet.
  if (!curr_battle->fleet_statuses){
      free(curr_battle->battle_name);
      free(node->battle);
      free(node);
      return NULL;
  }
  curr_battle->fleet_statuses[0] = NULL;

  return node;
}


// Fill fleet statuses in specific battle
struct fleet_status_t *create_fleet_statuse(char *fleet_name, unsigned short total_ships, unsigned int status_flag){
  if (!fleet_name) return NULL;

  struct fleet_status_t *fleet = (struct fleet_status_t *) malloc(sizeof(struct fleet_status_t));
  if(!fleet){
    return NULL;
  }

  fleet->fleet_name = strdup(fleet_name);
  if (!fleet->fleet_name) return NULL;

  fleet->status_flags = status_flag;
  fleet->total_ships = total_ships;
  return fleet;
}


// Push front node
void pushfront_node(struct galaxy_history_t *history , struct battle_node_t *current_battle){
  if (!history || !current_battle) return;

  current_battle->next = history->head;

  if (history->head)
    history->head->prev = current_battle;
  else
    history->tail = current_battle;

  history->head = current_battle;
  return;
}


/*
// Push back node (Because of double linked list I think is optional +
// we don't need to insert data to a list in sorted way I assume, (if it is it also be nice thing to implement insertion at some index)
//  so main logic is that, the tail of the list is going to be a very first line of the file)
void pushback_node(struct galaxy_history_t *history , struct battle_node_t *current_battle){}
*/

// MAIN FUNCTIONS
// galactic_func.h functions implementation...

int initialize_history(struct galaxy_history_t **history_ptr){
  if (!history_ptr) return 1;

  *history_ptr = (struct galaxy_history_t *) malloc(sizeof(struct galaxy_history_t));
  if (!*history_ptr) return 4; // Memory allocation error

  // Init history
  (*history_ptr)->head = NULL;
  (*history_ptr)->tail = NULL;
  (*history_ptr)->total_battles = 0;

  return 0;
}

// Returns: 0 - success, 1 - invalid input (NULL), 2 - file opening error,
//          3 - corrupted file format, 4 - memory allocation error.
int load_galactic_history(const char *fname, struct galaxy_history_t **history_ptr){
  if (!fname || !history_ptr) return 1;

  // File handling
  FILE *fptr = fopen(fname, "r");
  if (!fptr) return 2;

  // Declaring helping buffers
  char line[256], battle_name[58], fleet_name[58];
  unsigned int battle_date, total_ships;

  // Entering main loop
  int resize = 3;
  struct battle_node_t *current_battle_node = NULL; // Track the battle being currently populated

  while (fgets(line, sizeof(line), fptr) != NULL && *line != '\n'){

    // If a new BATTLE is encountered, finalize the previous battle's fleet_statuses array
    if (strstr(line, "BATTLE:") == line){ // Check if line starts with "BATTLE:"
      if (current_battle_node != NULL && current_battle_node->battle != NULL && current_battle_node->battle->fleet_statuses != NULL){
        size_t fleets_count = current_battle_node->battle->num_fleets;
        // Trim unused allocated memory for the *previous* battle
        struct fleet_status_t **resized = (struct fleet_status_t **) realloc(current_battle_node->battle->fleet_statuses, (fleets_count + 1) * sizeof(struct fleet_status_t *));
        if (!resized) {
            fclose(fptr);
            destroy_galactic_history(history_ptr);
            return 4;
        }
        current_battle_node->battle->fleet_statuses = resized;
        current_battle_node->battle->fleet_statuses[fleets_count] = NULL;
      }

      if (sscanf(line, "BATTLE:%57[^\n]", battle_name) == 1){
        // Creating new battle node
        struct battle_node_t *new_battle = create_new_battle(battle_name, 0);
        if(!new_battle){
            fclose(fptr);
            destroy_galactic_history(history_ptr);
            return 4;
        }
        pushfront_node(*history_ptr, new_battle);
        (*history_ptr)->total_battles++;
        current_battle_node = new_battle; // Update current_battle_node
        continue;
      } else {
        fclose(fptr);
        destroy_galactic_history(history_ptr);
        return 3; // Malformed BATTLE line
      }
    }


    if (sscanf(line, "DATE:%u", &battle_date) == 1){
      if (!current_battle_node || !current_battle_node->battle) {
        fclose(fptr);
        destroy_galactic_history(history_ptr);
        return 3; // Corrupted file: DATE without BATTLE
      }
      current_battle_node->battle->battle_date = battle_date;
      continue;
    }

    if (strstr(line, "FLEET:") == line && sscanf(line, "FLEET:%57[^|]|%*d|%u|", fleet_name, &total_ships) == 2){ // Use %*d to skip the 0
      if (!current_battle_node || !current_battle_node->battle) {
        fclose(fptr);
        destroy_galactic_history(history_ptr);
        return 3; // Corrupted file: FLEET without BATTLE
      }

      struct battle_t *current_battle_data = current_battle_node->battle;

      // Check if we need to resize the fleet_statuses array
      if ((int)current_battle_data->num_fleets >= resize) {
          resize += 4;

          struct fleet_status_t **temp = (struct fleet_status_t **) realloc(current_battle_data->fleet_statuses, resize * sizeof(struct fleet_status_t *));
          if (!temp){
            fclose(fptr);
            destroy_galactic_history(history_ptr);
            return 4;
          }
          current_battle_data->fleet_statuses = temp;
      }

      unsigned char status_flag = set_fleet_status(line);

      current_battle_data->fleet_statuses[current_battle_data->num_fleets] = create_fleet_statuse(fleet_name, total_ships, status_flag);
      if (!current_battle_data->fleet_statuses[current_battle_data->num_fleets]) {
        fclose(fptr);
        destroy_galactic_history(history_ptr);
        return 4;
      }
      current_battle_data->num_fleets++;
      // The NULL terminator will be added either on next realloc or at the end of the file/battle.
      continue;
    }
    else { // Handle empty lines or malformed lines that are not BATTLE, DATE, or FLEET
      // Only return error if it's not an empty line (just a newline character)
      if (strlen(line) > 1) {
        fclose(fptr);
        destroy_galactic_history(history_ptr);
        return 3; // Corrupted file format
      }
    }
  }

  // After the loop, the last battle's fleet_statuses array needs to be finalized
  if((*history_ptr)->head != NULL && (*history_ptr)->head->battle != NULL && (*history_ptr)->head->battle->fleet_statuses != NULL){
    size_t fleets_count = (*history_ptr)->head->battle->num_fleets;
    struct fleet_status_t **resized = (struct fleet_status_t **) realloc((*history_ptr)->head->battle->fleet_statuses, (fleets_count + 1) * sizeof(struct fleet_status_t *));
    if (!resized) {
      fclose(fptr);
      destroy_galactic_history(history_ptr);
      return 4;
    }
    (*history_ptr)->head->battle->fleet_statuses = resized;
    (*history_ptr)->head->battle->fleet_statuses[fleets_count] = NULL;
  }

  fclose(fptr);
  return 0;
}


void destroy_galactic_history(struct galaxy_history_t **history_ptr){
  if (!history_ptr || !*history_ptr) return;

  struct battle_node_t *current = (*history_ptr)->head;

  while(current){
    struct battle_node_t *next = current->next;

    if (current->battle){
      if (current->battle->fleet_statuses){
        struct fleet_status_t **fleet_ptr_iter = current->battle->fleet_statuses;
        while (*fleet_ptr_iter != NULL) {
            struct fleet_status_t *fleet_to_free = *fleet_ptr_iter;
            if (fleet_to_free->fleet_name) {
                free(fleet_to_free->fleet_name);
            }
            free(fleet_to_free);
            fleet_ptr_iter++;
        }
        free(current->battle->fleet_statuses);
        current->battle->fleet_statuses = NULL;
      }

      if (current->battle->battle_name) {
          free(current->battle->battle_name);
          current->battle->battle_name = NULL;
      }

      free(current->battle);
      current->battle = NULL;
    }

    free(current);
    current = next;
  }
  free(*history_ptr);
  *history_ptr = NULL;
}


void display_galactic_history(const struct galaxy_history_t *history){
    if (!history) return;

    struct battle_node_t *current = history->head;

    while(current){
        // Make sure current->battle is not NULL before accessing its members
        if (!current->battle) {
            current = current->next;
            continue;
        }

        printf("%s WAS ON %u YEARS AFTER FIRST GALACTIC ERA\n TOTAL AMOUNT OF FLEETS : %zu\n", current->battle->battle_name, current->battle->battle_date, current->battle->num_fleets);

        for (unsigned int i = 0; i < current->battle->num_fleets; ++i){ // Use unsigned int for loop counter
            struct fleet_status_t *temp = current->battle->fleet_statuses[i];
            // Check if temp is NULL before dereferencing
            if (!temp) {
                printf("Error: Fleet status at index %u is NULL.\n", i);
                continue; // Skip to the next fleet
            }

            printf("%s AMOUNT OF SHIPS IN THIS FLEET %u\n", temp->fleet_name, temp->total_ships);
            // Assuming status flags are 4 bits as per set_fleet_status
            printf("status flags: ");
            if (temp->status_flags & (1u << 0)) printf("Ready for Jump ");
            if (temp->status_flags & (1u << 1)) printf("Shield Active ");
            if (temp->status_flags & (1u << 2)) printf("Critical Damage ");
            if (temp->status_flags & (1u << 3)) printf("Withdrawal ");
            printf("\n");
        }
        printf("\n");
        current = current->next;
    }
}


// Bitwise operation function: Returns the count of fleets that have at least one of the
// specified status bits set.
// Returns: The count of fleets meeting the criteria, -1 on error (e.g., NULL history).
int count_fleets_with_status_bits(const struct galaxy_history_t *history, unsigned int mask){
  return 0; // place holder
}


// Bitwise operation function: Modifies the status of all fleets within a given battle.
// `battle_name`: The name of the battle to find.
// `operation_type`: 0 - set bits (OR), 1 - clear bits (AND NOT), 2 - toggle bits (XOR).
// `mask`: The bitmask to apply the operation with.
// Returns: The number of fleets modified, -1 if battle not found or on error.
int modify_fleet_statuses_in_battle(struct galaxy_history_t *history, const char *battle_name, int operation_type, unsigned int mask){
  return 0; // place holder
}
