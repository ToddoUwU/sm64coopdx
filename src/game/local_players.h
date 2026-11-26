#ifndef LOCAL_PLAYERS_H
#define LOCAL_PLAYERS_H

// Local split screen: several players on this PC in one game.
//
// Local players occupy the first player slots: gMarioStates[0 .. gLocalPlayerCount - 1] and the matching
// gNetworkPlayers. A player's data never moves between slots. Systems that exist once per player on this PC
// (camera, HUD, dialog, sound listener) keep one copy per local player and use gCurrLocalPlayer to know which
// player they are working for; see local_player_scope_begin().
//
// With one local player (the default, and always when playing online) everything behaves like upstream.

#include <stdbool.h>
#include <PR/ultratypes.h>
#include "types.h"

#define MAX_LOCAL_PLAYERS 4

// Number of players on this PC in the current session (1 when split screen is off).
extern u8 gLocalPlayerCount;
// The local player that per-player systems are currently working for.
extern u8 gCurrLocalPlayer;

/* |description|Gets how many players are playing on this PC (1 unless local split screen is on)|descriptionEnd| */
u8 get_local_player_count(void);
/* |description|Gets the local player that the game is currently updating, rendering or drawing the HUD for|descriptionEnd| */
u8 get_current_local_player(void);
/* |description|Checks if the player in slot `playerIndex` is playing on this PC|descriptionEnd| */
bool is_local_player(s32 playerIndex);

bool mario_is_local(const struct MarioState *m);
struct MarioState *local_mario(u8 localIndex);
// The Mario of the player that per-player systems are currently working for.
struct MarioState *current_local_mario(void);
bool split_screen_active(void);

// Makes per-player systems work for `localIndex` until the matching local_player_scope_end(). Returns the
// previous player so scopes can nest.
u8 local_player_scope_begin(u8 localIndex);
void local_player_scope_end(u8 previousLocalIndex);
// Same as local_player_scope_begin() for the player owning `m`; non-local Marios keep the current scope.
u8 local_player_scope_begin_mario(const struct MarioState *m);

// Nearest active local Mario to an object or point (never NULL while a session runs).
struct MarioState *nearest_local_mario_to_object(struct Object *obj);
struct MarioState *nearest_local_mario_to_point(f32 x, f32 y, f32 z);
// Checks if any active local Mario is within `distance` of `obj`.
bool any_local_mario_within(struct Object *obj, f32 distance);

// Session lifetime (hosting on this PC). Starting adds local players 2..N as network players.
u8 local_players_requested_count(void);
void local_players_start_session(void);
void local_players_end_session(void);

#endif
