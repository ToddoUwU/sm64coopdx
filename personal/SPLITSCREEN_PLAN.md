# Local split screen for sm64coopdx (PersonalUpdates)

Goal: 2–4 players on one PC, one game instance, one window divided into views
(2 players = top/bottom, 3–4 = 2×2 grid). Each player has their own Mario,
camera, HUD and controller. Music and global sounds play once; positional sounds
are heard from the nearest player's camera. All local players stay in the same
level and area: when one warps, everyone warps.

Base: upstream tag `v1.5.1` (matches the installed release and its mods).
Reference only: Isaac0-dev/sm64coopdx `splitscreen` branch (June 2025). Its idea
of swapping player data into slot 0 is NOT used here.

## Survey (v1.5.1)

Places that assume "slot 0 is the one person at this computer":

| Pattern | Refs | Files |
|---|---|---|
| `gMarioStates[0]` | 319 | 93 |
| `gMarioState` (current Mario pointer) | 299 | 21 |
| `gLakituState` | 274 | 15 |
| `gPlayer1Controller` | 144 | 20 |
| `playerIndex == 0` / `!= 0` | 187 | 23+16 |
| `gNetworkPlayerLocal` | 110 | 46 |
| `gHudDisplay` | 80 | 16 |
| `gCamera` | 77 | 21 |
| `gMarioObject` | 67 | 32 |

camera.c alone holds ~100 file-level variables of per-view state.

## Design

### 1. Local players (`src/game/local_players.{c,h}`)
- `gLocalPlayerCount` (1–4). Local players occupy Mario/network slots
  `0 .. gLocalPlayerCount-1`; data never moves between slots.
- `is_local_player(u8 playerIndex)`, `mario_is_local(m)`, `local_mario(i)`.
- A current-player context, `gCurrLocalPlayer`, says which local player a
  per-view system (camera, HUD, dialog, listener) is working for. It is set
  explicitly:
  - while local player *i*'s Mario updates;
  - while view *i* updates its camera and renders;
  - while view *i*'s HUD draws;
  - by game logic that acts for a specific Mario (`local_player_scope(m)`).
  Otherwise it is 0. Per-view state is reached through this index; nothing is
  copied or swapped.
- Split screen only runs in a session hosted on this PC. While
  `gLocalPlayerCount > 1`, remote joins are refused, because the network
  protocol has one player per machine. Local players 1–3 are `NPT_LOCAL`
  network players with global indices 1–3. They never time out, and their
  level/area always mirrors player 1.

### 2. Input
- Player 1: keyboard + gamepad `gamepad_number` (unchanged).
- Players 2–4: the next connected SDL game controllers, in order, skipping
  player 1's pad. Can be overridden with `splitscreen_gamepad_2..4`.
  The button bindings are shared.
- Each local player's Mario reads `gControllers[i]`. Menus and dialogs read
  the controller of the player who owns them; the pause menu reads any local
  player.

### 3. Game logic sweep
- `playerIndex == 0` meaning "simulated here" becomes `mario_is_local(m)`.
- `gMarioStates[0]` meaning "the person here" becomes the scoped player,
  the nearest local player, or a loop over local players, depending on the site.
- Warps: any local player's warp moves every local player. On arrival, every
  local Mario is placed at the destination, spread out so they don't overlap.
- Coins, stars, lives, death and bubbles follow the existing co-op rules, with
  local players treated like players who are online.

### 4. Cameras
- `struct Area` gets `cameras[MAX_LOCAL_PLAYERS]`. `camera` stays as
  `cameras[0]` for mods.
- Every per-view camera.c / bettercamera / first-person variable moves into
  `struct CameraView`, one per local player, reached through
  `gCurrCameraView`. The old names become macros over the active view, so the
  12k lines of camera logic are unchanged.
- The camera's C buttons, modes, cutscenes and shake belong to that view's
  player. Screen shake from a point applies to every view by distance.

### 5. Rendering
- The gfx backend gets *sub-windows*: a rectangle of the real window that acts as
  the whole screen. Viewport, scissor, aspect ratio and the HUD's ortho
  projection all work inside it unchanged. A custom display-list command selects
  the sub-window, so interpolated replays of the same display list follow it.
- `render_game()` draws the area graph once per view, inside that view's
  sub-window, with that view's camera. It then draws that view's HUD and Lua HUD
  hooks. Shared overlays (pause menu, screen transitions, djui menus and chat)
  are drawn last over the full window.
- Interpolation: per-node "previous tick" state is read by every view but
  written only by the last view of the tick, so all views interpolate from the
  same history. Camera, perspective, viewport and skybox interpolation records
  are kept per view.
- Rooms (castle, BBH): each view shows its own player's room. Room-culled
  objects are active if any local player's room shows them.
- Each view's aspect ratio comes from its sub-window, so the view shows more to
  the sides (Hor+) rather than stretching.

### 6. HUD, dialogs, menus
- `gHudDisplay` becomes per local player (`gHudDisplays[]`, plus a macro over
  the scoped player).
- A dialog belongs to the local player who opened it. It is drawn in their view
  and driven by their controller. While it is open, another local player's
  request waits.
- Pause, act select and file select: shared, full window.

### 7. Audio
- One game, so music and global sounds already play once.
- A positional sound's camera-space position (`cameraToObject`) is taken from
  the nearest local camera. Each view's render offers its value and the closest
  one wins for that tick.

### 8. Lua / mods
- New API: `get_local_player_count()`, `is_local_player(index)`,
  `get_current_local_player()`.
- While a HUD hook runs for view *i*, `gMarioStates[0]` and
  `gNetworkPlayers[0]` resolve to local player *i*.
- Bundled mods' gameplay checks that mean "me" become "any local player".

### 9. UI, launch, install
- Host panel: "Local players" (1–4). CLI: `--local-players N`.
  Config: `splitscreen_players`, `splitscreen_gamepad_2..4`,
  `splitscreen_character_2..4`.
- Built into its own folder (the Quiver install is not touched), with a new
  Moonlight tile.

## Stages
Each stage builds, is committed and is pushed.

0. Plan, build script. ✔ when committed.
1. Local-player core + session setup + config/CLI.
2. Input.
3. Game logic sweep (mario, interaction, level_update, behaviors, network gating).
4. Camera views.
5. Rendering: sub-windows, per-view render, interpolation, rooms.
6. HUD / dialogs / menus.
7. Audio.
8. Lua API + bundled mods.
9. Host UI, install script, launcher tile, headless logic checks.
— STOP before any visual test (user's instruction, 2026-11-26).

## Status
See the bottom of this file, updated each stage.
