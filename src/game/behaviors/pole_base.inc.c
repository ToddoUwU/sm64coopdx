// pole_base.inc.c

void bhv_pole_base_loop(void) {
    for (u8 i = 0; i < gLocalPlayerCount; i++) {
        struct Object *player = gMarioStates[i].marioObj;
        if (!player) { continue; }
        if (o->oPosY - 10.0f < player->oPosY && player->oPosY < o->oPosY + o->hitboxHeight + 30.0f) {
            if (o->oTimer > 10) {
                if (!(gMarioStates[i].action & MARIO_PUNCHING)) {
                    cur_obj_push_mario_away(70.0f);
                }
            }
        }
    }
}
