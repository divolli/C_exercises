#include "galactic_func.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global strings for setting fleet status and printing
char *first_bit = "Ready for Jump";
char *second_bit = "Shield Active";
char *third_bit = "Critical Damage";
char *fourth_bit = "Withdrawal";


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
void fleet_statuses_fill(struct fleet_status_t *fleet, char *fleet_name, unsigned short total_ships, unsigned int status_flag){
  if (!fleet || !fleet_name) return;

  fleet->fleet_name = strdup(fleet_name);
  if (!fleet->fleet_name) return;

  fleet->status_flags = status_flag;
  fleet->total_ships = total_ships;
  return;
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
// Push back node, Do i even need this?
void pushback_node(struct galaxy_history_t *history , struct battle_node_t *current_battle){}
*/


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
  if (!fname || !history_ptr || !*history_ptr) return 1;

  // File handling
  FILE *fptr = fopen(fname, "r");
  if (!fname) return 2;

  // Declaring helping buffers
  char line[256], battle_name[58], fleet_name[58];
  unsigned int battle_date, total_ships;
  unsigned char status_flag;

  fclose(fptr);
  return 0;
}


void display_galactic_history(const struct galaxy_history_t *history){

}


// Bitwise operation function: Returns the count of fleets that have at least one of the
// specified status bits set.
// Returns: The count of fleets meeting the criteria, -1 on error (e.g., NULL history).
int count_fleets_with_status_bits(const struct galaxy_history_t *history, unsigned int mask){

}


// Bitwise operation function: Modifies the status of all fleets within a given battle.
// `battle_name`: The name of the battle to find.
// `operation_type`: 0 - set bits (OR), 1 - clear bits (AND NOT), 2 - toggle bits (XOR).
// `mask`: The bitmask to apply the operation with.
// Returns: The number of fleets modified, -1 if battle not found or on error.
int modify_fleet_statuses_in_battle(struct galaxy_history_t *history, const char *battle_name, int operation_type, unsigned int mask){

}


void destroy_galactic_history(struct galaxy_history_t **history_ptr){

}

