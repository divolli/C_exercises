#include "galactic_func.h"
#include <stdlib.h>
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
    printf("ERROR OCCUR %d", load);
    return load;
  }

  display_galactic_history(data);
  //free data
  destroy_galactic_history(&data);
  return 0;
}
