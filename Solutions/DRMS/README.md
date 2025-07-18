# Solution Overview
## `utils.h` and `asset.h`

I began by implementing the functions declared in these two header files. At this stage, there were no major difficulties or edge cases worth mentioning, so there’s nothing specific to discuss here.

### First challenge
I'm encountering a challenge with the `delete_asset` function. Due to some past miscommunication or oversight in task planning, `delete_asset` currently needs to interact with the `UserRecord` list, but it doesn't have a way to access it. I've decided to postpone completing `delete_asset` until after I've finished working on `users.c`.

For now, I'm considering a few temporary design options to allow `delete_asset` to access the `UserRecord` list, even though I know I'll need to refine this approach later for a better, more permanent solution:

- Make the `UserRecord` list (specifically its head pointer) a global variable. This would allow `delete_asset` to directly access it from `assets.c`.

- Move the `UserRecord` struct definition into `assets.h`. This would technically make the `UserRecord` struct visible to `assets.c`, but it doesn't solve the problem of accessing the list instance itself.

- Move the `delete_asset` function (or at least its user-related logic) into `users.c` or a more centralized place. This would mean the users module would handle asset reference removal.

I recognize these are temporary workarounds, and I'll need to revisit the overall design for how these two modules (assets and users) interact to ensure `delete_asset` can correctly remove user references in a clean and modular way.
