#include "assets.h"
#include "errors.h"
#include "stdlib.h"
#include "string.h"
#include "utils.h"
#include <stdint.h>
#include <stdio.h>
#define CHECK 0


DigitalAsset *create_asset_node(const char *hash, uint32_t size, uint8_t flags){
  if (!hash) return NULL;

  // Creating new node
  DigitalAsset *new_asset = (DigitalAsset *) calloc(1, sizeof(DigitalAsset));
  if(!new_asset) return NULL;

  //Writing hash to the asset hash
  new_asset->hash = strdup(hash);
  if(!new_asset->hash){
    free(new_asset);
    return NULL;
  }

  //Write size, flags and set next to NULL
  new_asset->flags = flags;
  new_asset->size_bytes = size;
  new_asset->next = NULL;

  return new_asset;
}


ErrorCode insert_asset(DigitalAsset **head, const char *hash, uint32_t size, uint8_t flags, AssetHashCompareFunc compare_func){
  if (!head || !hash || !compare_func) return ERROR_INVALID_ARGUMENT;

  // Creating new asset
  DigitalAsset *new_asset = create_asset_node(hash, size, flags);
  if (!new_asset) return ERROR_MEMORY_ALLOCATION_FAILED;

  // If head is NULL set new node to head
  DigitalAsset *current = *head;
  DigitalAsset *previous = NULL;
  if (!current){
    *head = new_asset;
    return SUCCESS;
  }

  // Compare all nodes to insert by alphabetical order
  while (current){
    int comp = compare_func(new_asset->hash, current->hash);
    if ( comp < 0 ){
      if(!previous){ // If previous is NULL, it means current is the original head
        new_asset->next = *head;
        *head = new_asset;
        return SUCCESS;
      }
      // Insertion in the middle of a list
      new_asset->next = current;
      previous->next = new_asset;
      return SUCCESS;
    }
    else if (comp == 0){ // If our hash is duplicate
      free(new_asset->hash);
      free(new_asset);
      return ERROR_DUPLICATE_ENTRY;
    }
    previous = current;
    current = current->next;
  }

  // Hash in new asset is the smallest insert at the end
  previous->next = new_asset;
  return SUCCESS;
}


ErrorCode find_asset(DigitalAsset *head, const char *hash, DigitalAsset **found_asset, AssetHashCompareFunc compare_func){
  if (!head || !hash || !found_asset || !compare_func) return ERROR_INVALID_ARGUMENT;

  // Searching for match in hashes
  DigitalAsset *current = head;
  while(current){
    int comp = compare_func(current->hash, hash);
    if(comp == 0){
      *found_asset = current;
      return SUCCESS;
    }
    current = current->next;
  }
  // Not found
  return ERROR_NOT_FOUND;
}


void clear_assets(DigitalAsset **head){
  if ( !head ) return;

  DigitalAsset *current = *head;
  while(current){
    DigitalAsset *next = current->next;
    free(current->hash);
    free(current);
    current = next;
  }

  *head = NULL;
}


void print_assets(DigitalAsset *head){
  if (!head) return;

  DigitalAsset *current = head;
  while (current){
    printf("%s | Size: %d bytes | ", current->hash, current->size_bytes);
    // Printing asset flag
    uint8_t flag = current->flags;
    if (flag & ASSET_FLAG_ENCRYPTED){
      printf("Asset is encrypted ");
    }
    if (flag & ASSET_FLAG_READ_ONLY){
      printf("Asset is read only ");
    }
    if (flag & ASSET_FLAG_ARCHIVED){
      printf("Asset is archived ");
    }
    if (flag & ASSET_FLAG_CORRUPTED){
      printf("Asset is corrupted ");
    }
    printf("\n");
    current = current->next;
  }
}


ErrorCode load_assets_from_file(DigitalAsset **head, const char *filepath, AssetHashCompareFunc compare_func){
  if (!head || !filepath || !compare_func) return ERROR_INVALID_ARGUMENT;

  // Opening the file
  FILE *file = fopen(filepath, "r");
  if (!file) return ERROR_FILE_NOT_FOUND;

  // helper buffers
  char line[256], hash[126]; uint32_t bsize; uint8_t flag;

  // Main file reading loop
  while (fgets(line , sizeof(line), file)) {
    // Trim line
    char *comment_start = strchr(line, ';');
    if (comment_start) *comment_start = '\0';


    // Check if empty line
    if (*line == '\n') continue;

    // Scan & check the line
    if (sscanf(line, "%s %u %c", hash, &bsize, &flag) != 3){
      fclose(file);
      clear_assets(head);
      return ERROR_FILE_CORRUPTED;
    }

    // Pushing node
    ErrorCode error = insert_asset(head, hash, bsize, flag, compare_func);
    if(error != SUCCESS){
      fclose(file);
      clear_assets(head);
      return error;
    }
  }

  fclose(file);
  return SUCCESS;
}


ErrorCode save_assets_to_file(DigitalAsset *head, const char *filepath){
  if (!head || !filepath) return ERROR_INVALID_ARGUMENT;

  // Open file
  FILE *file = fopen(filepath, "a");
  if (!file) return ERROR_FILE_NOT_FOUND;

  DigitalAsset *current = head;
  while(current){
    fprintf(file, "%s %u %u\n", current->hash, current->size_bytes, current->flags);
    current = current->next;
  }

  fclose(file);
  return SUCCESS;
}


/**
 * @brief Deletes an asset from the list by its hash.
 * Important: This function must also handle the removal of all *references* to this asset
 * from users' `owned_assets` lists. You will need a mechanism to access the user list for this.
 * @param head Pointer to the pointer to the head of the DigitalAsset list.
 * @param hash Hash of the asset to delete.
 * @param compare_func Function pointer for comparing hashes.
 * @return ErrorCode.
 */
ErrorCode delete_asset(DigitalAsset **head, const char *hash, AssetHashCompareFunc compare_func){ // TO DO end this function after users.c
  if (!head || !hash || !compare_func) return ERROR_INVALID_ARGUMENT;

  // Searching for match in hashes for deletion
  DigitalAsset *found = NULL;
  ErrorCode error = find_asset(*head, hash, &found, compare_func);
  if (error != SUCCESS) return error;

  // TO DO >>>

  return SUCCESS;
}


//Quick check
int asset_quick_check(void){
  if (CHECK){
    printf("START OF THE PROGRAM\n");
    DigitalAsset *head = NULL;
    ErrorCode err = load_assets_from_file(&head, "../assets.txt", compare_asset_hashes);
    if(err != SUCCESS) {
      return err;
    }
    print_assets(head);

    ErrorCode load = save_assets_to_file(head, "../res.txt");
    if (load != SUCCESS){
      clear_assets(&head);
      return load;
    }
    clear_assets(&head);
    return 0;
  }
  return 0;
}
