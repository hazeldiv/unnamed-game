#ifndef PACKET_H
#define PACKET_H
#include <stdint.h>
#include <vec2.h>
#include <enet/enet.h>

#define MAX_WORLD 20
#define MAX_PLAYER 32
#define WORLD_WIDTH 100
#define WORLD_HEIGHT 100

enum {
    TYPE_INIT,
    TYPE_POSITION,
    TYPE_TILE,
    TYPE_CHAT,
    TYPE_DISCONNECT
};

enum {
    BROADCAST_POSITION,
    BROADCAST_TILE,
    BROADCAST_CHAT,
};

typedef struct header {
    uint8_t type;
} header;

typedef struct simpleTile {
    uint8_t block;
    uint8_t background;
} simpleTile;

typedef struct player {
    uint8_t id;
    char name[50];
    vec2 position_a;
    vec2 position_b;
    uint64_t timeStamp_a;
    uint64_t timeStamp_b;
} player;

typedef struct client {
    ENetPeer *peer;
    player data;
} client;

typedef struct world {
    simpleTile tile[WORLD_HEIGHT][WORLD_WIDTH];
    client clienState[MAX_PLAYER];
} world;

typedef struct clientInit {
    uint8_t type;
    char name[50];
} clientInit;

typedef struct serverInit {
    uint8_t id;
    uint64_t timeStamp;
    simpleTile tile[WORLD_HEIGHT][WORLD_WIDTH];
} serverInit;

typedef struct positionUpdate {
    uint8_t type;
    uint8_t id;
    vec2 position;
} positionUpdate;

typedef struct positionBroadcast {
    // simpleTile tile[MAX_WORLD][MAX_WORLD];
    // player players[MAX_PLAYER];
    uint8_t type;
    player players[MAX_PLAYER];
} positionBroadcast;

typedef struct tileUpdate {
    uint8_t type;
    simpleTile tile;
    vec2_int position;
} tileUpdate;

#endif