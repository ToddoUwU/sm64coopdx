// Network system for a local split screen session: every player is on this PC, so nothing is sent or received
// and nobody can join. Selected by network_init() when hosting with more than one local player.

#include <stdbool.h>
#include <string.h>
#include "network.h"

static bool ns_local_initialize(UNUSED enum NetworkType networkType, UNUSED bool reconnecting) {
    return true;
}

static s64 ns_local_get_id(UNUSED u8 localIndex) {
    return 0;
}

static char *ns_local_get_id_str(UNUSED u8 localIndex) {
    static char sId[] = "0";
    return sId;
}

static void ns_local_save_id(UNUSED u8 localIndex, UNUSED s64 networkId) {
}

static void ns_local_clear_id(UNUSED u8 localIndex) {
}

static void *ns_local_dup_addr(UNUSED u8 localIndex) {
    return NULL;
}

static bool ns_local_match_addr(void *addr1, void *addr2) {
    return addr1 == addr2;
}

static void ns_local_update(void) {
}

static int ns_local_send(UNUSED u8 localIndex, UNUSED void *addr, UNUSED u8 *data, UNUSED u16 dataLength) {
    return 0;
}

static void ns_local_get_lobby_id(char *destination, u32 destLength) {
    if (destLength > 0) { destination[0] = '\0'; }
}

static void ns_local_get_lobby_secret(char *destination, u32 destLength) {
    if (destLength > 0) { destination[0] = '\0'; }
}

static void ns_local_shutdown(UNUSED bool reconnecting) {
}

struct NetworkSystem gNetworkSystemLocal = {
    .initialize       = ns_local_initialize,
    .get_id           = ns_local_get_id,
    .get_id_str       = ns_local_get_id_str,
    .save_id          = ns_local_save_id,
    .clear_id         = ns_local_clear_id,
    .dup_addr         = ns_local_dup_addr,
    .match_addr       = ns_local_match_addr,
    .update           = ns_local_update,
    .send             = ns_local_send,
    .get_lobby_id     = ns_local_get_lobby_id,
    .get_lobby_secret = ns_local_get_lobby_secret,
    .shutdown         = ns_local_shutdown,
    .requireServerBroadcast = false,
    .name             = "Local",
};
