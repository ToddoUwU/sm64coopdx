#ifndef CAMERA_VIEW_H
#define CAMERA_VIEW_H

// Per-player camera state for local split screen.
//
// Every variable that describes one camera's motion, mode, input or cutscene progress lives in a CameraView, one
// per local player. The original variable names are macros over the view of gCurrLocalPlayer, so the camera code
// reads and writes the camera of whichever player is in scope (see local_players.h) without being rewritten.
// State that is shared by every camera (course settings, mod overrides, signals to objects such as
// gObjCutsceneDone, gSecondCameraFocus) stays global in camera.c.
//
// This header is not parsed by the Lua autogen.

#include <PR/ultratypes.h>
#include "types.h"
#include "camera.h"
#include "bettercamera.h"
#include "first_person_cam.h"
#include "local_players.h"

#define CAMERA_VIEW_CUTSCENE_VARS 10
#define CAMERA_VIEW_CREDITS_SPLINE_POINTS 32
#define CAMERA_VIEW_HANDHELD_SHAKE_POINTS 4

struct CameraView {
    // the area camera this view drives (one of gCurrentArea->cameras)
    struct Camera *camera;
    struct LakituState lakituState;
    struct CameraFOVStatus fovState;
    struct TransitionInfo modeTransition;
    struct PlayerGeometry marioGeometry;
    struct PlayerCameraState *marioCamState;
    NewCamera newCamera;
    struct FirstPersonCamera firstPersonCamera;

    Vec3f oldPosition;
    Vec3f oldFocus;
    Vec3f player2FocusOffset;
    s16 creditsPlayer2Pitch;
    s16 creditsPlayer2Yaw;
    u8 framesPaused;

    s16 avoidYawVel;
    s16 cameraYawAfterDoorCutscene;
    struct CutsceneSplinePoint curCreditsSplinePos[CAMERA_VIEW_CREDITS_SPLINE_POINTS];
    struct CutsceneSplinePoint curCreditsSplineFocus[CAMERA_VIEW_CREDITS_SPLINE_POINTS];
    f32 cutsceneSplineSegmentProgress;
    s16 cutsceneSplineSegment;

    struct HandheldShakePoint handheldShakeSpline[CAMERA_VIEW_HANDHELD_SHAKE_POINTS];
    s16 handheldShakeMag;
    f32 handheldShakeTimer;
    f32 handheldShakeInc;
    s16 handheldShakePitch;
    s16 handheldShakeYaw;
    s16 handheldShakeRoll;

    s16 selectionFlags;
    s16 cameraMovementFlags;
    s16 statusFlags;
    s16 secondRotateFlags;
    s16 cameraSoundFlags;
    u16 cButtonsPressed;
    s32 cutsceneDialogID;
    s16 cutsceneShot;
    s16 cutsceneTimer;
    s16 areaYaw;
    s16 areaYawChange;
    s16 lakituDist;
    s16 lakituPitch;
    f32 zoomAmount;
    s16 cSideButtonYaw;
    s16 behindMarioSoundTimer;
    f32 zeroZoomDist;
    s16 cUpCameraPitch;
    s16 modeOffsetYaw;
    s16 spiralStairsYawOffset;
    s16 eightDirModeBaseYaw;
    s16 eightDirModeYawOffset;
    f32 panDistance;
    f32 cannonYOffset;
    struct CutsceneVariable cutsceneVars[CAMERA_VIEW_CUTSCENE_VARS];
    struct ModeTransitionInfo modeInfo;
    Vec3f castleEntranceOffset;
    u32 parTrackIndex;
    struct ParallelTrackingPoint *parTrackPath;
    struct CameraStoredInfo parTrackTransOff;
    struct CameraStoredInfo cameraStoreCUp;
    struct CameraStoredInfo cameraStoreCutscene;
    struct Object *cutsceneFocus;
    s16 yawSpeed;
    u8 objectCutscene;
    u8 recentCutscene;
    u8 framesSinceCutsceneEnded;
    u8 cutsceneDialogResponse;
    Vec3f fixedModeBasePosition;

    // rom hack camera
    s16 romHackYaw;
    u8 romHackZoom;
    s8 romHackIsUpdate;
    f32 romHackWaterFocus;
    f32 romHackWaterPitchOffset;
    s16 romHackOffset;

    // were function-local statics in newcam_update_camera()
    u32 newcamLastAction;
    bool newcamForceCentering;
};

extern struct CameraView gCameraViews[MAX_LOCAL_PLAYERS];

// Resets every view to the game's starting camera state (called once at boot).
void camera_views_init(void);
// Runs `fn` once with each camera view in scope (all MAX_LOCAL_PLAYERS views).
void camera_views_for_each(void (*fn)(void));
// The camera view of the player in scope.
#define gCurrCameraView (&gCameraViews[gCurrLocalPlayer])

// Area cameras: one per local player (struct Area.cameras; area->camera is player 1's).
struct Camera *area_camera_of_local_player(struct Area *area, u8 localIndex);
// The area camera of the player in scope.
struct Camera *area_scoped_camera(struct Area *area);
// `m`'s own camera when `m` plays on this PC, otherwise player 1's.
struct Camera *mario_area_camera(struct MarioState *m);
// update_camera() / reset_camera() for every local player's camera in the current area.
void update_local_cameras(void);
void reset_local_cameras(void);

// Variables used outside camera.c. Names that only camera.c uses are mapped at the top of camera.c.
#define gCamera              (gCameraViews[gCurrLocalPlayer].camera)
#define gLakituState         (gCameraViews[gCurrLocalPlayer].lakituState)
#define gFOVState            (gCameraViews[gCurrLocalPlayer].fovState)
#define gNewCamera           (gCameraViews[gCurrLocalPlayer].newCamera)
#define gFirstPersonCamera   (gCameraViews[gCurrLocalPlayer].firstPersonCamera)
#define gCameraMovementFlags (gCameraViews[gCurrLocalPlayer].cameraMovementFlags)
#define gCutsceneTimer       (gCameraViews[gCurrLocalPlayer].cutsceneTimer)
#define gCutsceneFocus       (gCameraViews[gCurrLocalPlayer].cutsceneFocus)
#define gRecentCutscene      (gCameraViews[gCurrLocalPlayer].recentCutscene)
#define sSelectionFlags      (gCameraViews[gCurrLocalPlayer].selectionFlags)
#define sCameraSoundFlags    (gCameraViews[gCurrLocalPlayer].cameraSoundFlags)
#define sCButtonsPressed     (gCameraViews[gCurrLocalPlayer].cButtonsPressed)
#define sCutsceneVars        (gCameraViews[gCurrLocalPlayer].cutsceneVars)
#define sMarioCamState       (gCameraViews[gCurrLocalPlayer].marioCamState)

#endif // CAMERA_VIEW_H
