// shock_wave.c.inc

f32 D_8032F420[] = { 1.9f, 2.4f, 4.0f, 4.8f };

void bhv_bowser_shock_wave_loop(void) {
    f32 sp2C, sp28, sp24, sp20;
    s16 sp1E = 70;
    o->oBowserShockWaveUnkF4 = o->oTimer * 10;
    cur_obj_scale(o->oBowserShockWaveUnkF4);
    if (gGlobalTimer % 3)
        o->oOpacity -= 1;
    if (o->oTimer > sp1E)
        o->oOpacity -= 5;
    if (o->oOpacity <= 0)
        obj_mark_for_deletion(o);
    // the ring hits each player on this PC who is on the ground inside it
    for (u8 i = 0; i < gLocalPlayerCount; i++) {
        struct MarioState *m = &gMarioStates[i];
        if (m->marioObj == NULL || !is_player_active(m)) { continue; }
        if (o->oTimer < sp1E && mario_is_in_air_action(m) == 0) {
            f32 distanceToMario = dist_between_objects(o, m->marioObj);
            sp2C = o->oBowserShockWaveUnkF4 * D_8032F420[0];
            sp28 = o->oBowserShockWaveUnkF4 * D_8032F420[1];
            sp24 = o->oBowserShockWaveUnkF4 * D_8032F420[2];
            sp20 = o->oBowserShockWaveUnkF4 * D_8032F420[3];
            if ((sp2C < distanceToMario && distanceToMario < sp28)
                || (sp24 < distanceToMario && distanceToMario < sp20))
                m->marioObj->oInteractStatus |= INT_STATUS_HIT_BY_SHOCKWAVE;
        }
    }
}
