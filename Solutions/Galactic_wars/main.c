#include "galactic_func.h"
#include <stdio.h>

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

  display_galactic_history(data);
  printf("\nChanging fleet statuses\n");
  int first_bit_set = count_fleets_with_status_bits(data, (1u << 0));
  printf("Fleet statuses with first set statuse -> %d\n", first_bit_set);
  int modified = modify_fleet_statuses_in_battle(data, "Battle of Yavin", 1, (1u << 3));
  printf("Modified fleet statuses -> %d\n", modified);
  //free data
  destroy_galactic_history(&data);
  return 0;
}
