#include <stdio.h>
#include <float.h>

#include "local_players.h"
#include "sm64.h"
#include "characters.h"
#include "player_palette.h"
#include "obj_behaviors.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "pc/configfile.h"
#include "pc/debuglog.h"
#include "pc/network/network.h"
#include "pc/network/network_player.h"

u8 gLocalPlayerCount = 1;
u8 gCurrLocalPlayer = 0;

u8 get_local_player_count(void) {
    return gLocalPlayerCount;
}

u8 get_current_local_player(void) {
    return gCurrLocalPlayer;
}

bool is_local_player(s32 playerIndex) {
    return playerIndex >= 0 && playerIndex < gLocalPlayerCount;
}

bool mario_is_local(const struct MarioState *m) {
    return m != NULL && is_local_player(m->playerIndex);
}

struct MarioState *local_mario(u8 localIndex) {
    return &gMarioStates[localIndex < gLocalPlayerCount ? localIndex : 0];
}

struct MarioState *current_local_mario(void) {
    return local_mario(gCurrLocalPlayer);
}

bool split_screen_active(void) {
    return gLocalPlayerCount > 1;
}

u8 local_player_scope_begin(u8 localIndex) {
    u8 previous = gCurrLocalPlayer;
    gCurrLocalPlayer = (localIndex < gLocalPlayerCount) ? localIndex : 0;
    return previous;
}

void local_player_scope_end(u8 previousLocalIndex) {
    gCurrLocalPlayer = (previousLocalIndex < gLocalPlayerCount) ? previousLocalIndex : 0;
}

u8 local_player_scope_begin_mario(const struct MarioState *m) {
    if (!mario_is_local(m)) { return gCurrLocalPlayer; }
    return local_player_scope_begin((u8) m->playerIndex);
}

struct MarioState *nearest_local_mario_to_point(f32 x, f32 y, f32 z) {
    struct MarioState *nearest = NULL;
    f32 nearestDistSq = FLT_MAX;
    for (u8 i = 0; i < gLocalPlayerCount; i++) {
        struct MarioState *m = &gMarioStates[i];
        if (m->marioObj == NULL || !is_player_active(m)) { continue; }
        f32 dx = m->pos[0] - x;
        f32 dy = m->pos[1] - y;
        f32 dz = m->pos[2] - z;
        f32 distSq = dx * dx + dy * dy + dz * dz;
        if (distSq < nearestDistSq) {
            nearestDistSq = distSq;
            nearest = m;
        }
    }
    // every local player can be inactive at once (all in bubbles): fall back to player 1
    return nearest ? nearest : &gMarioStates[0];
}

struct MarioState *nearest_local_mario_to_object(struct Object *obj) {
    if (obj == NULL) { return &gMarioStates[0]; }
    return nearest_local_mario_to_point(obj->oPosX, obj->oPosY, obj->oPosZ);
}

bool any_local_mario_within(struct Object *obj, f32 distance) {
    if (obj == NULL) { return false; }
    for (u8 i = 0; i < gLocalPlayerCount; i++) {
        struct MarioState *m = &gMarioStates[i];
        if (m->marioObj == NULL || !is_player_active(m)) { continue; }
        f32 dx = m->pos[0] - obj->oPosX;
        f32 dy = m->pos[1] - obj->oPosY;
        f32 dz = m->pos[2] - obj->oPosZ;
        if (dx * dx + dy * dy + dz * dz < distance * distance) { return true; }
    }
    return false;
}

bool is_local_mario_object(struct Object *obj) {
    if (obj == NULL) { return false; }
    for (u8 i = 0; i < gLocalPlayerCount; i++) {
        if (gMarioStates[i].marioObj == obj) { return true; }
    }
    return false;
}

struct MarioState *local_mario_on_platform(struct Object *platform) {
    if (platform == NULL) { return NULL; }
    for (u8 i = 0; i < gLocalPlayerCount; i++) {
        struct MarioState *m = &gMarioStates[i];
        if (m->marioObj != NULL && m->marioObj->platform == platform) { return m; }
    }
    return NULL;
}

struct MarioState *local_mario_held_by(struct Object *obj) {
    if (obj == NULL) { return NULL; }
    for (u8 i = 0; i < gLocalPlayerCount; i++) {
        if (gMarioStates[i].heldByObj == obj) { return &gMarioStates[i]; }
    }
    return NULL;
}

struct MarioState *local_mario_using_object(struct Object *obj) {
    if (obj == NULL) { return NULL; }
    for (u8 i = 0; i < gLocalPlayerCount; i++) {
        if (gMarioStates[i].usedObj == obj) { return &gMarioStates[i]; }
    }
    return NULL;
}

struct MarioState *local_mario_in_action(u32 action) {
    for (u8 i = 0; i < gLocalPlayerCount; i++) {
        if (gMarioStates[i].action == action) { return &gMarioStates[i]; }
    }
    return NULL;
}

struct MarioState *local_mario_with_cap_flag(u32 capFlag) {
    for (u8 i = 0; i < gLocalPlayerCount; i++) {
        if (gMarioStates[i].cap & capFlag) { return &gMarioStates[i]; }
    }
    return NULL;
}

bool cur_obj_collided_with_local_mario(void) {
    if (gCurrentObject == NULL) { return false; }
    for (u8 i = 0; i < gLocalPlayerCount; i++) {
        struct MarioState *m = &gMarioStates[i];
        if (m->marioObj == NULL || !m->visibleToObjects) { continue; }
        if (obj_check_if_collided_with_object(gCurrentObject, m->marioObj) == 1) { return true; }
    }
    return false;
}

bool local_mario_ground_pounding_platform(struct Object *platform) {
    for (u8 i = 0; i < gLocalPlayerCount; i++) {
        if (obj_is_mario_ground_pounding_platform(&gMarioStates[i], platform)) { return true; }
    }
    return false;
}

struct MarioState *local_mario_or_nearest(struct MarioState *candidate, struct Object *obj) {
    if (mario_is_local(candidate)) { return candidate; }
    return nearest_local_mario_to_object(obj);
}

u8 local_players_requested_count(void) {
    return (u8) MAX(1, MIN(configSplitscreenPlayers, MAX_LOCAL_PLAYERS));
}

// Palette preset named after the character (palettes/<Character>.ini), or Mario's default colors.
static const struct PlayerPalette *character_palette(u8 characterIndex) {
    for (u16 i = 0; i < gPresetPaletteCount; i++) {
        if (!strcmp(gPresetPalettes[i].name, gCharacters[characterIndex].name)) {
            return &gPresetPalettes[i].palette;
        }
    }
    return &DEFAULT_MARIO_PALETTE;
}

void local_players_start_session(void) {
    gLocalPlayerCount = local_players_requested_count();
    gCurrLocalPlayer = 0;
    for (u8 i = 1; i < gLocalPlayerCount; i++) {
        u8 character = (u8) configSplitscreenCharacter[i];
        char name[MAX_CONFIG_STRING];
        snprintf(name, sizeof(name), "Player %u", i + 1);
        network_player_connected(NPT_LOCAL, i, character, character_palette(character), name, "");
    }
    if (gLocalPlayerCount > 1) {
        LOG_INFO("local split screen: %u players", gLocalPlayerCount);
    }
}

void local_players_end_session(void) {
    gLocalPlayerCount = 1;
    gCurrLocalPlayer = 0;
}
