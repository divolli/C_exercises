#include "galactic_func.h"
#include <stdlib.h>
#include <stdio.h>

int main(void){
  char *file = "../galactic_data.txt";

  // Main struct declaration
  struct galaxy_history_t *data = (struct galaxy_history_t *) calloc(1,sizeof(struct galaxy_history_t));
  if (!data) return 8;

  // Initialization
  int res = initialize_history(&data);
  if(res) {
    destroy_galactic_history(&data);
    return 4;
  }

  // Read from a file
  int load = load_galactic_history(file, &data);
  if (load != 0){
    destroy_galactic_history(&data);
    printf("ERROR OCCUR %d", load);
    return load;
  }

  // display_galactic_history(data);
  //free data
  destroy_galactic_history(&data);
  return 0;
}
