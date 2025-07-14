#ifndef ASSETS_H
#define ASSETS_H

#include <stdint.h> // For uint32_t, uint8_t
#include "errors.h" // For ErrorCode

// Bit flags definitions for an asset.
// Use bitwise operators to manipulate these flags.
#define ASSET_FLAG_ENCRYPTED   (1U << 0) // Asset is encrypted
#define ASSET_FLAG_READ_ONLY   (1U << 1) // Asset is read-only
#define ASSET_FLAG_ARCHIVED    (1U << 2) // Asset is archived
#define ASSET_FLAG_CORRUPTED   (1U << 3) // Asset is corrupted
// You can add more flags as needed, e.g., ASSET_FLAG_SHARED, ASSET_FLAG_PUBLIC

/**
 * @brief Structure representing a single digital asset (file).
 */
typedef struct DigitalAsset {
    char *hash;             // Unique identifier for the asset (e.g., SHA256 as a hex string). The node owns this memory.
    uint32_t size_bytes;    // Size of the file in bytes.
    uint8_t flags;          // Bit flags indicating the asset's state.
    struct DigitalAsset *next; // Pointer to the next asset in the singly linked list.
} DigitalAsset;

/**
 * @brief Function pointer for comparing two hashes (strings).
 * Returns <0 if hash1 < hash2, 0 if equal, >0 if hash1 > hash2.
 */
typedef int (*AssetHashCompareFunc)(const char *hash1, const char *hash2);

/**
 * @brief Function pointer for counting '01' bit pairs in a string.
 */
typedef uint8_t (*StringBitPairCounter)(const char *str);


// --- Function Prototypes for DigitalAsset List (Implement in assets.c) ---

/**
 * @brief Creates a new DigitalAsset node.
 * @param hash Unique asset hash (string).
 * @param size Size of the asset in bytes.
 * @param flags Bit flags for the asset.
 * @return Pointer to the newly created node, or NULL if memory allocation fails or data is invalid.
 */
DigitalAsset *create_asset_node(const char *hash, uint32_t size, uint8_t flags);

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

#endif // ASSETS_H
