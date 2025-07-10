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
unsigned char *set_fleet_status(char *line){
  if (!line) return NULL;

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

  unsigned char *res = &status;

  return res;
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
  curr_battle->fleet_statuses = (struct fleet_status_t **) calloc(2, sizeof(struct fleet_status_t *));
  if (!curr_battle->fleet_statuses){
    free(curr_battle->battle_name);
    free(node->battle);
    free(node);
    return NULL;
  }

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

  //Init history
  int res = initialize_history(history_ptr);
  if (res) return 1;

  // File handling
  FILE *fptr = fopen(fname, "r");
  if (!fptr) return 2;

  // Declaring helping buffers
  char line[256], battle_name[58], fleet_name[58];
  unsigned int battle_date, total_ships;

  // Entering main loop
  int resize = 1;
  while (fgets(line, sizeof(line), fptr) != NULL && *line != '\n'){

    if (sscanf(line, "BATTLE:%57[^\n]" ,battle_name) == 1){
      //TODO reset not used fleet status** after resizing
      //.....

      // Creating new battle node
      struct battle_node_t *new_battle = create_new_battle(battle_name, 1);
      if(!new_battle){
        fclose(fptr);
        // free logic
        destroy_galactic_history(history_ptr);
        return 4;
      }
      // push new battle to the start (first battle in file will be at the tail of the list)
      pushfront_node(*history_ptr, new_battle);
      (*history_ptr)->total_battles++;
      continue;
    }

    if (sscanf(line, "DATE:%u" ,&battle_date) == 1){
      // Writing a year
      (*history_ptr)->head->battle->battle_date = battle_date;
      continue;
    }

    if (sscanf(line, "FLEET:%57[^|]|0|%u|", fleet_name, &total_ships) == 2){
      // dereference the fleets ptrptr
      struct fleet_status_t **fleets = (*history_ptr)->head->battle->fleet_statuses;
      int curr_fleet = (int)(*history_ptr)->head->battle->num_fleets ;

      // Resize logic for fleets
      if (resize == (curr_fleet - 1)){
        resize += 4;
        struct fleet_status_t **temp = (struct fleet_status_t **) realloc(fleets,resize * sizeof(struct fleet_status_t *));
        if (!temp){
          fclose(fptr);
          // free logic
          destroy_galactic_history(history_ptr);
          return 4;
        }
        fleets = temp;
      }

      // Setting the status flag
      unsigned char *status_flag = set_fleet_status(line);

      // Creating a fleet
      *(fleets + curr_fleet) = create_fleet_statuse(fleet_name, total_ships, *status_flag);
      (*history_ptr)->head->battle->num_fleets++;
      continue;
    }
    else {
      fclose(fptr);
      // Free logic
      destroy_galactic_history(history_ptr);
      // Corrupted file format
      return 3;
    }
  }

  fclose(fptr);
  return 0;
}


void destroy_galactic_history(struct galaxy_history_t **history_ptr){
  if (!history_ptr || !*history_ptr) return;

  struct battle_node_t *current = (*history_ptr)->head;

  while(current){
    struct battle_node_t *next = current->next;

    // struct fleet_status_t **fleet = current->battle->fleet_statuses;
    // while (*fleet){
    //   struct fleet_status_t *temp = *fleet;
    //   if (temp->fleet_name) free(temp->fleet_name);
    //   if (temp) free(temp);
    //   fleet++;
    // }

    for (int i = 0; i < current->battle->num_fleets; ++i){
      free(current->battle->fleet_statuses[i]->fleet_name);
      free(current->battle->fleet_statuses[i]);
    }
    if (current->battle->fleet_statuses) free(current->battle->fleet_statuses);
    if (current->battle->battle_name) free(current->battle->battle_name);
    if (current->battle) free(current->battle);
    if (current) free(current);
    current = next;
  }
}


void display_galactic_history(const struct galaxy_history_t *history){
  if (!history) return;

  struct battle_node_t *current = history->head;

  while(current){
    struct battle_node_t *next = current->next;
    struct fleet_status_t **fleet = current->battle->fleet_statuses;
    printf("%s WAS ON %u YEARS AFTER FIRST GALACTIC ERA\n TOTAL AMOUNT OF FLEETS : %lu", current->battle->battle_name, current->battle->battle_date, current->battle->num_fleets);
    while (fleet){
      struct fleet_status_t *temp = *fleet;
      printf("%s AMOUNT OF SHIPS IN THIS FLEET %u flag statuses: ", temp->fleet_name, temp->total_ships);
      for (int i = 0; i < (int)sizeof(unsigned char) * 8; ++i){
        if (temp->status_flags & (1u << i)) printf("%d 1", i);
          // logic for printing status flags
      }
      fleet++;
    }
    current = next;
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
