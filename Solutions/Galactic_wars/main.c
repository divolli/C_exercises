#include "galactic_func.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void){
  char *file = "../galactic_data.txt";

  // Main struct declaration
  struct galaxy_history_t *data = NULL;

  int res_init = initialize_history(&data);
  if (res_init != 0) {
      printf("ERROR: History initialization failed.\n");
      return res_init;
  }

  // Read from a file
  int load = load_galactic_history(file, &data);
  if (load != 0){
    destroy_galactic_history(&data);
    printf("ERROR OCCUR %d\n", load);
    return load;
  }

  struct fleet_status_t *new_fleet = (struct fleet_status_t *) malloc(sizeof(struct fleet_status_t));
  if (!new_fleet) {
    destroy_galactic_history(&data);
    return 8;
  }
  new_fleet->fleet_name = strdup("The Andromeda legion defense");
  if(!new_fleet->fleet_name){
    free(new_fleet);
    destroy_galactic_history(&data);
    return 4;
  }

  new_fleet->total_ships = 134;
  new_fleet->status_flags = 5;
  int added = add_fleet_to_battle(data, "Battle of Yavin", 19770525, new_fleet);
  if (added != 0) printf("ERROR WHILE ADDING FLEER\n");

  display_galactic_history(data);

  printf("\nChanging fleet statuses\n");
  int first_bit_set = count_fleets_with_status_bits(data, (1u << 0));
  printf("Fleet statuses with first set statuse -> %d\n", first_bit_set);

  int modified = modify_fleet_statuses_in_battle(data, "Battle of Yavin", 19770525, 1, (1u << 3));
  printf("Modified fleet statuses -> %d\n", modified);

  //free data
  destroy_galactic_history(&data);
  return 0;
}
