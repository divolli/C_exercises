#ifndef USERS_H
#define USERS_H

#include <stdint.h>
#include "errors.h"
#include "assets.h" // Needed because UserRecord will link to DigitalAsset

/**
 * @brief Helper structure to create a list of assets assigned to a user.
 * It contains a POINTER to a DigitalAsset from the MAIN DigitalAsset list.
 * It does not own the memory pointed to by 'asset_ptr'.
 */
typedef struct UserAssetRef {
    DigitalAsset *asset_ptr; // Pointer to an asset from the main list.
    struct UserAssetRef *next; // Pointer to the next asset reference in the user's list.
} UserAssetRef;

/**
 * @brief Structure representing a user record in a doubly linked list.
 */
typedef struct UserRecord {
    char *username;         // Username. The node owns this memory.
    uint32_t user_id;       // Unique user identifier.
    UserAssetRef *owned_assets; // Head of a singly linked list of pointers to DigitalAssets owned by this user.
                                // THIS list (UserAssetRef nodes) is allocated and freed BY the UserRecord,
                                // but the *DigitalAsset pointed to* is NOT.
    struct UserRecord *prev; // Pointer to the previous record in the doubly linked list.
    struct UserRecord *next; // Pointer to the next record in the doubly linked list.
} UserRecord;

/**
 * @brief Function pointer for comparing two usernames (strings).
 * Returns <0 if name1 < name2, 0 if equal, >0 if name1 > name2.
 */
typedef int (*UserNameCompareFunc)(const char *name1, const char *name2);


// --- Function Prototypes for UserRecord List (Implement in users.c) ---

/**
 * @brief Creates a new UserRecord node.
 * @param username Username (string).
 * @param user_id User ID.
 * @return Pointer to the newly created node, or NULL if memory allocation fails or data is invalid.
 */
UserRecord *create_user_node(const char *username, uint32_t user_id);

/**
 * @brief Inserts a new user into the doubly linked list, maintaining alphabetical order by username.
 * @param head Pointer to the pointer to the head of the UserRecord list.
 * @param username Username to insert.
 * @param user_id User ID.
 * @param compare_func Function pointer for comparing usernames.
 * @return ErrorCode.
 */
ErrorCode insert_user(UserRecord **head, const char *username, uint32_t user_id, UserNameCompareFunc compare_func);

/**
 * @brief Finds a user in the list by username.
 * @param head Head of the UserRecord list.
 * @param username Username to find.
 * @param found_user Pointer to a pointer where the found user will be stored.
 * @param compare_func Function pointer for comparing usernames.
 * @return ErrorCode.
 */
ErrorCode find_user(UserRecord *head, const char *username, UserRecord **found_user, UserNameCompareFunc compare_func);

/**
 * @brief Deletes a user from the list by username.
 * Important: This function must free memory only for the UserRecord node and its internal UserAssetRef list,
 * but NOT for the DigitalAssets pointed to by UserAssetRef.
 * @param head Pointer to the pointer to the head of the UserRecord list.
 * @param username Username to delete.
 * @param compare_func Function pointer for comparing usernames.
 * @return ErrorCode.
 */
ErrorCode delete_user(UserRecord **head, const char *username, UserNameCompareFunc compare_func);

/**
 * @brief Frees all memory occupied by the UserRecord list.
 * Must correctly free UserRecord nodes and their internal UserAssetRef lists,
 * but NOT the DigitalAssets that these references point to.
 * @param head Pointer to the pointer to the head of the UserRecord list.
 */
void clear_users(UserRecord **head);

/**
 * @brief Prints all users and their assigned assets.
 * @param head Head of the UserRecord list.
 */
void print_users(UserRecord *head);

/**
 * @brief Assigns an existing asset to a user. Adds a reference to the user's owned_assets list.
 * @param user_head Head of the UserRecord list (to find the user).
 * @param asset_head Head of the main DigitalAsset list (to find the asset).
 * @param username Username.
 * @param asset_hash Hash of the asset to assign.
 * @param user_compare Function pointer for comparing usernames.
 * @param asset_compare Function pointer for comparing asset hashes.
 * @return ErrorCode.
 */
ErrorCode assign_asset_to_user(UserRecord *user_head, DigitalAsset *asset_head, const char *username, const char *asset_hash, UserNameCompareFunc user_compare, AssetHashCompareFunc asset_compare);

/**
 * @brief Removes an asset assignment from a user (removes the reference from the user's owned_assets list).
 * @param user_head Head of the UserRecord list.
 * @param username Username.
 * @param asset_hash Hash of the asset to remove.
 * @param user_compare Function pointer for comparing usernames.
 * @param asset_compare Function pointer for comparing asset hashes.
 * @return ErrorCode.
 */
ErrorCode remove_asset_from_user(UserRecord *user_head, const char *username, const char *asset_hash, UserNameCompareFunc user_compare, AssetHashCompareFunc asset_compare);

/**
 * @brief Loads users from a file into the list.
 * File format (example): username user_id asset_hash1 asset_hash2 ...
 * Important: When loading, you must find the corresponding DigitalAsset in the main list (main_asset_list_head)
 * and add pointers to these *existing* DigitalAssets to the user's owned_assets list.
 * @param head Pointer to the pointer to the head of the UserRecord list.
 * @param filepath Path to the file.
 * @param compare_func Function pointer for comparing usernames (for insertion).
 * @param main_asset_list_head Head of the main DigitalAsset list (needed to find assets for assignment).
 * @return ErrorCode.
 */
ErrorCode load_users_from_file(UserRecord **head, const char *filepath, UserNameCompareFunc compare_func, DigitalAsset *main_asset_list_head);

/**
 * @brief Saves users and their assigned assets to a file.
 * @param head Head of the UserRecord list.
 * @param filepath Path to the file.
 * @return ErrorCode.
 */
ErrorCode save_users_to_file(UserRecord *head, const char *filepath);

#endif // USERS_H
