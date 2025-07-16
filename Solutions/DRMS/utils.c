#include "utils.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

// Macro generating a function to count '01' bit pairs in a given numeric type.
// E.g., CREATE_BIT_PAIR_COUNT_FUNC(char) will create a prototype: uint8_t count_01_pairs_char(char val);
#define CREATE_BIT_PAIR_COUNT_FUNC(type_name)              \
  uint8_t count_01_pairs_in_##type_name(type_name val){    \
    uint8_t pairs = 0;                                     \
    int size = sizeof(type_name) * 8;                      \
    for (int i = 0; i < (size - 1); ++i){                  \
      if (val & (1u << i)){                                \
        if(!(val & (1u << (i + 1)))){                      \
          pairs++;                                         \
        }                                                  \
      }                                                    \
    }                                                      \
    return pairs;                                          \
  }                                                        \

// Macro generating a function to count '01' bit pairs in each byte of a string.
// This function will be used for hashes and names.
// In its implementation, you will need to call the count_01_pairs_char function.
#define CREATE_STRING_BIT_PAIR_COUNT_FUNC(func_name)       \
  uint8_t func_name(const char *str){                      \
    int length = strlen(str);                              \
    uint8_t pairs = 0;                                     \
    for (int i = 0; i < length; ++i){                      \
      pairs += count_01_pairs_in_char(*(str + i));         \
    }                                                      \
    return pairs;                                          \
  }                                                        \


// Declare a specific instance for 'char' so it can be used in the string counting function.
CREATE_BIT_PAIR_COUNT_FUNC(char)
// Declare the function to count '01' pairs in strings, which will use count_01_pairs_char.
CREATE_STRING_BIT_PAIR_COUNT_FUNC(count_01_pairs_in_string)


/**
  @brief Function for comparing two hashes (strings).
  @return <0 if hash1 < hash2, 0 if equal, >0 if hash1 > hash2.
*/
int compare_asset_hashes(const char *hash1, const char *hash2, ErrorCode *error){
  if (!hash1 || !hash2) {
    if (error) *error = ERROR_INVALID_ARGUMENT;
    return 0;
  }
  if (error) *error = SUCCESS;
  return strcmp(hash1, hash2);
}

/**
  @brief Function for comparing two usernames (strings).
  Implement simple case-insensitivity (e.g., convert to lowercase for comparison).
  @return <0 if name1 < name2, 0 if equal, >0 if name1 > name2.
*/
int compare_user_names(const char *name1, const char *name2, ErrorCode *error){
  if (!name1 || !name2){
    if (error) *error = ERROR_INVALID_ARGUMENT;
    return 0;
  }

  char *lower_name1 = strdup(name1);
  if(!lower_name1){
    if (error) *error = ERROR_INVALID_ARGUMENT;
    return 0;
  }
  char *lower_name2 = strdup(name2);
  if(!lower_name2){
    if (error) *error = ERROR_INVALID_ARGUMENT;
    free(lower_name1);
    return 0;
  }

  char *temp1 = lower_name1;
  char *temp2 = lower_name2;

  for ( ; *temp1; ++temp1) *temp1 = tolower((unsigned char)*temp1);
  for ( ; *temp2; ++temp2) *temp2 = tolower((unsigned char)*temp2);

  int result = strcmp(lower_name1, lower_name2);

  free(lower_name1);
  free(lower_name2);

  if (error) *error = SUCCESS;
  return result;
}
