#ifndef MARIO_ACTIONS_CUTSCENE_H
#define MARIO_ACTIONS_CUTSCENE_H

#include <PR/ultratypes.h>

#include "macros.h"
#include "types.h"

void print_displaying_credits_entry(void);
void bhv_end_peach_loop(void);
void bhv_end_toad_loop(void);
s32 geo_switch_peach_eyes(s32 run, struct GraphNode *node, UNUSED s32 a2);
s32 mario_ready_to_speak(struct MarioState* m);
u8 should_start_or_continue_dialog(struct MarioState* m, struct Object* object);
// The local Mario that talks to `object`: whoever already is in a dialog with it, else `preferred` when it plays
// on this PC, else the nearest local Mario.
struct MarioState *local_dialog_mario(struct Object *object, struct MarioState *preferred);
s32 set_mario_npc_dialog(struct MarioState* m, s32 actionArg, u8(*inContinueDialogFunction)(void));
s32 mario_execute_cutscene_action(struct MarioState *m);
void cutscene_take_cap_off(struct MarioState *m);
void cutscene_put_cap_on(struct MarioState *m);

#endif // MARIO_ACTIONS_CUTSCENE_H
