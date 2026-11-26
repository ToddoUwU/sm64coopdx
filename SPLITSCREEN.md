# Local split screen for Super Mario 64 Co-op DX: what was done and why

Repository: `~/QuiverLauncher/src/sm64coopdx`, fork `ToddoUwU/sm64coopdx`, branch `PersonalUpdates`,
based on upstream release `v1.5.1`. The upstream push URL is disabled. Plan: `personal/SPLITSCREEN_PLAN.md`.

## Why

The earlier setup ran two copies of Co-op DX on one PC, networked to each other. It failed in three ways:
- **Two windows.** COSMIC ignored the requested positions, so the windows overlapped.
- **Controllers did nothing.** One cause found: upstream declares `background_gamepad` as a number but stores it
  as true/false, so the setting saved as garbage. Fixed here.
- **Sound played twice.** Two games each played their own audio.

Co-op DX is built for online play only. This change gives it real split screen: **one game, one window,
one view per player**. Two players get top and bottom halves; three or four players share a 2×2 grid.

Isaac0-dev's `splitscreen` branch was read as a reference only. It works by swapping each player's data into
slot 0 in turn, and its known bugs come from those swaps. Here every player keeps their own slot. Systems that
exist once per person at this PC (camera, HUD, dialog, rumble, sound listener) keep one copy per local player.
A "current local player" scope (`gCurrLocalPlayer`) says which player's copy is in use.

## Core idea (`src/game/local_players.{c,h}`)

- Local players are slots `0 .. gLocalPlayerCount-1`. With one player, everything is identical to upstream.
- `is_local_player()`, `mario_is_local()`, `local_mario()`, `current_local_mario()`.
- Helpers such as `nearest_local_mario_to_object()`, `local_mario_on_platform()`, `local_mario_held_by()` and
  `cur_obj_collided_with_local_mario()`.
- `LOCAL_PLAYER_SCOPE(...)` switches the scope for a block and restores it automatically.

## What changed, by stage (each stage is its own commit)

1. **Session.** Hosting with more than one local player uses a no-op network system (`network_local.c`), so
   nothing listens and nobody can join. Players 2–4 are `NPT_LOCAL` network players that never time out and
   always share player 1's level and area. Config: `splitscreen_players`, `splitscreen_gamepad_2..4`,
   `splitscreen_character_2..4`; CLI: `--local-players N`. The Host panel has a "Local Players" option.
2. **Input.** Players 2–4 each get a gamepad: `splitscreen_gamepad_N`, or the next free controller,
   re-assigned when pads are plugged in or out. Menus anyone can use read a combined controller. A dialog box
   reads the controller of the player who opened it. Rumble is per player.
3. **Game logic.** 229 checks that meant "simulated on this PC" now include every local player.
   - Warps move everyone together.
   - Instant warps move only the player on the warp floor, unless the warp changes area (then everyone moves).
   - Coins are shared, as in online co-op.
   - The HUD is per player (`gHudDisplays[]`).
   - NPC conversations belong to the player who starts them, one at a time.
   - About 60 objects and NPCs now react to every local player: Yoshi, Toad's stars, caps taken by Klepto or
     Ukiki, platforms, switches, shockwaves, the tilting pyramid, and others.
   - Rooms (castle, Big Boo's Haunt) are tracked per player.
   - Cap music keeps playing while either player still has a cap.
   - Any local player can pause.
4. **Cameras.** About 110 camera, free-camera, first-person and rom-hack camera variables moved into
   `struct CameraView`, one per player. The old names became macros over the active view, so the 12,000 lines
   of camera logic didn't change. Each area allocates one camera per player. Camera functions handed a camera
   switch to its owner automatically. World shakes reach every view, scaled by distance.
5. **Rendering.**
   - The gfx backend has view rectangles (`G_VIEW_EXT` / `gfx_set_view`). Inside a view, viewport, scissor,
     aspect ratio and screen size all behave as if the view were the whole window. Each half therefore gets a
     wider field of view instead of a stretched picture.
   - `render_game()` draws the world, HUD, Lua HUD hooks, name tags and text once per view. A dialog box
     appears in its owner's view. Pause, transitions and Co-op DX menus cover the whole window. Areas without
     a 3D camera, such as act select, use the whole window.
   - Frame interpolation keeps its records per view. Every view reads the shared per-object history; only the
     last view writes it.
   - Snow, bubbles and HUD animations are per view.
7. **Audio.** One game, so music and global sounds play once. Each positional sound uses the nearest player's
   camera.
8. **Lua and mods.**
   - New functions: `get_local_player_count`, `is_local_player`, `mario_is_local`, `local_mario`, and others.
   - While a mod's HUD hook draws a view, `gMarioStates[0]`, `gNetworkPlayers[0]`, `gHudDisplay` and
     `gLakituState` refer to that view's player.
   - Star Road, SM74, the personal star counter and the cheats mod were updated to treat every local player as
     local. `personal/check-lua.sh` syntax-checks mod Lua.
9. **UI.** The "Local Players" selector on the Host panel; mouse look stays on player 1.

## Verification so far

- Every stage builds cleanly in the memory-capped unit (`personal/build.sh`, which now also fails on implicit
  function declarations).
- A full clean build passed with no new warnings.
- All 44 mod Lua files compile.
- **Nothing has been run yet.** As requested, I stopped before any visual or runtime testing.

## Known limits

- Split screen works only when this PC hosts. Online play with more than one player on a PC is not supported.
- Arena and Hide and Seek (online game modes) and third-party mods that check `m.playerIndex == 0` act only for
  player 1.
- Bowser's intro cutscene plays for player 1.

## Still to do

1. Install script: build into its own folder next to the Quiver install (`--server 7777 --local-players 2`),
   and add a Moonlight tile. Keep the old two-window tile until this one works.
2. First run and visual testing (next session, with you).
