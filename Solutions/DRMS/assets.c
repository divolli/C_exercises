#include "assets.h"
#include "errors.h"
#include "stdlib.h"
#include "string.h"


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

/**
 * @brief Finds an asset in the list by its hash.
 * @param head Head of the DigitalAsset list.
 * @param hash Hash of the asset to find.
 * @param found_asset Pointer to a pointer where the found asset will be stored.
 * @param compare_func Function pointer for comparing hashes.
 * @return ErrorCode.
 */
ErrorCode find_asset(DigitalAsset *head, const char *hash, DigitalAsset **found_asset, AssetHashCompareFunc compare_func);

/**
 * @brief Deletes an asset from the list by its hash.
 * Important: This function must also handle the removal of all *references* to this asset
 * from users' `owned_assets` lists. You will need a mechanism to access the user list for this.
 * @param head Pointer to the pointer to the head of the DigitalAsset list.
 * @param hash Hash of the asset to delete.
 * @param compare_func Function pointer for comparing hashes.
 * @return ErrorCode.
 */
ErrorCode delete_asset(DigitalAsset **head, const char *hash, AssetHashCompareFunc compare_func);

/**
 * @brief Frees all memory occupied by the DigitalAsset list.
 * @param head Pointer to the pointer to the head of the DigitalAsset list.
 */
void clear_assets(DigitalAsset **head);

/**
 * @brief Prints all assets in the list.
 * @param head Head of the DigitalAsset list.
 */
void print_assets(DigitalAsset *head);

/**
 * @brief Loads assets from a file into the list.
 * File format (example): hash size_bytes flags
 * @param head Pointer to the pointer to the head of the DigitalAsset list.
 * @param filepath Path to the file.
 * @param compare_func Function pointer for comparing hashes (for insertion).
 * @return ErrorCode.
 */
ErrorCode load_assets_from_file(DigitalAsset **head, const char *filepath, AssetHashCompareFunc compare_func);

/**
 * @brief Saves assets from the list to a file.
 * @param head Head of the DigitalAsset list.
 * @param filepath Path to the file.
 * @return ErrorCode.
 */
ErrorCode save_assets_to_file(DigitalAsset *head, const char *filepath);
