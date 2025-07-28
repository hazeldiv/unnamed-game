#ifndef CLIENT_H
#define CLIENT_H
#include <tile.h>
#include <game.h>
#include <stdint.h>

#define MAXWORLD 20
#define MAXPLAYER 32

enum {
    TYPE_INIT,
    TYPE_POSITION,
    TYPE_TILE,
    TYPE_CHAT
};

enum {
    BROADCAST_POSITION,
    BROADCAST_TILE,
    BROADCAST_CHAT,
    BROADCAST_WORLD
};

enum {
    CONNECT_LOGIN,
    CONNECT_WORLD
};

typedef struct simpleTile {
    Uint8 block;
    Uint8 background;
} simpleTile;

typedef struct player {
    uint8_t id;
    char name[50];
    vec2 position_a;
    vec2 position_b;
    Uint64 timeStamp_a;
    Uint64 timeStamp_b;
} player;

typedef struct clientInit {
    uint8_t type;
    char name[50];
} clientInit;

typedef struct serverInit {
    uint8_t id;
    Uint64 timeStamp;
    struct simpleTile tile[WORLD_HEIGHT][WORLD_HEIGHT];
} serverInit;

typedef struct positionUpdate {
    uint8_t type;
    uint8_t id;
    vec2 position;
} positionUpdate;

typedef struct positionBroadcast {
    // struct world world[MAXWORLD][MAXWORLD];
    uint8_t type;
    struct player players[MAXPLAYER];
} positionBroadcast;

typedef struct tileUpdate {
    uint8_t type;
    simpleTile tile;
    vec2_int position;
} tileUpdate;

typedef struct clientHeader {
    uint8_t type;
} clientHeader;

typedef struct worldConnectPacket {
    uint8_t type;
    int port;
} worldConnectPacket;

typedef struct worldRequestPacket {
    uint8_t type;
    char playerName[50];
    char worldName[50];
} worldRequestPacket;

void tile_update(tile *tile);

void client_player_init(game *gameState);

void client_join_world(game *gameState);

// void client_world_init(game *gameState, int port);

void update_position(game *gameState);

#endif