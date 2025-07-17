#include "assets.h"
#include "stdlib.h"
#include "string.h"

/**
 * @brief Creates a new DigitalAsset node.
 * @param hash Unique asset hash (string).
 * @param size Size of the asset in bytes.
 * @param flags Bit flags for the asset.
 * @return Pointer to the newly created node, or NULL if memory allocation fails or data is invalid.
 */
DigitalAsset *create_asset_node(const char *hash, uint32_t size, uint8_t flags){
  if (!hash) return NULL;

  // Creating new node
  DigitalAsset *new_asset = (DigitalAsset *) calloc(1, sizeof(DigitalAsset));
  if(!new_asset) return NULL;

  //Writing hash to the asset hash
  new_asset->hash = strdup(hash);
}

/**
 * @brief Inserts a new asset into the list, maintaining alphabetical order by hash.
 * @param head Pointer to the pointer to the head of the DigitalAsset list.
 * @param hash Hash of the asset to insert.
 * @param size Size of the asset.
 * @param flags Flags of the asset.
 * @param compare_func Function pointer for comparing hashes.
 * @return ErrorCode.
 */
ErrorCode insert_asset(DigitalAsset **head, const char *hash, uint32_t size, uint8_t flags, AssetHashCompareFunc compare_func);

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
