#ifndef CONTROLLER_SDL_H
#define CONTROLLER_SDL_H

#include "controller_api.h"

#define VK_BASE_SDL_GAMEPAD 0x1000

// mouse buttons are also in the controller namespace, just offset 0x100
#define VK_OFS_SDL_MOUSE 0x0100
#define VK_BASE_SDL_MOUSE (VK_BASE_SDL_GAMEPAD + VK_OFS_SDL_MOUSE)
#define MWHEELUP    0x20
#define MWHEELDOWN  0x40

extern struct ControllerAPI controller_sdl;

// Split screen: reads the gamepad of local player `localIndex` (1-3) into `pad` (OR-ed in, not cleared).
// Players 2-4 only use gamepads; the keyboard and mouse belong to player 1.
void controller_sdl_read_local_player(u8 localIndex, OSContPad *pad);
// Rumbles the gamepad of local player `localIndex` (0 = player 1's gamepad).
void controller_sdl_rumble_local_player(u8 localIndex, float strength, float length);
void controller_sdl_rumble_stop_local_player(u8 localIndex);

#endif
