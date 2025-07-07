#include "galactic_func.h"
#include <stdlib.h>

int main(void){
  char *file = "../galactic_data.txt";

  // Main struct declaration
  struct galaxy_history_t *data = (struct galaxy_history_t *) malloc(sizeof(struct galaxy_history_t));
  if (!data) return 8;

  // Initialization
  int res = initialize_history(&data);
  if(res) {
    free(data);
    return 4;
  }

  // Read from a file

  return 0;
}
