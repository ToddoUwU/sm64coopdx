// tower_door.c.inc

static bool any_local_mario_attacking(void) {
    for (u8 i = 0; i < gLocalPlayerCount; i++) {
        if (check_mario_attacking(&gMarioStates[i])) { return true; }
    }
    return false;
}

void bhv_tower_door_loop(void) {
    if (o->oTimer == 0)
        o->oMoveAngleYaw -= 0x4000;
    if ((o->oInteractStatus & INT_STATUS_INTERACTED) || any_local_mario_attacking()) {
        obj_explode_and_spawn_coins(80.0f, 0);
        create_sound_spawner(SOUND_GENERAL_WALL_EXPLOSION);

        if (!(o->oInteractStatus & INT_STATUS_INTERACTED)) {
            network_send_collect_item(o);
        }
        o->oInteractStatus &= ~INT_STATUS_INTERACTED;
    }
}
