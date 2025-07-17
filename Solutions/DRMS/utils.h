#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

// --- Prototypes of comparison functions (implement in utils.c or main.c) ---

uint8_t count_01_pairs_in_char(char val);
uint8_t count_01_pairs_in_string(const char *str);

/**
  @brief Function for comparing two hashes (strings).
  @return <0 if hash1 < hash2, 0 if equal, >0 if hash1 > hash2.
*/
int compare_asset_hashes(const char *hash1, const char *hash2);

/**
  @brief Function for comparing two usernames (strings).
  Implement simple case-insensitivity (e.g., convert to lowercase for comparison).
  @return <0 if name1 < name2, 0 if equal, >0 if name1 > name2.
*/
int compare_user_names(const char *name1, const char *name2);

#endif // UTILS_H
