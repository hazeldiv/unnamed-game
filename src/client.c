#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <game.h>
#include <tile.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "Ws2_32.lib")

// #define SERVER_HOST "29.ip.gl.ply.gg"
// #define SERVER_PORT "33009"

// localhost
#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT "25565"

#define MAXWORLD 20

SOCKET sock;

int instruction;

struct world {
    Uint8 block;
    Uint8 background;
};

struct update_world {
    vec2_int position;
    struct world currentTile;
};

struct player {
    Uint8 id;
    char name[50];
    vec2 position_a;
    Uint64 timeStamp_a;
    vec2 position_b;
    Uint64 timeStamp_b;
};

struct initPacket {
    Uint8 id;
    Uint64 timeStamp;
    Uint16 status;
};

struct packet {
    struct world world[MAXWORLD][MAXWORLD];
    struct player players[32];
    Uint16 status;
};

void client_init(game *gameState) {
    WSADATA wsaData;
    struct addrinfo hints, *res;
    int result;

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Setup hints
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve server address and port
    result = getaddrinfo(SERVER_HOST, SERVER_PORT, &hints, &res);
    if (result != 0) {
        printf("getaddrinfo failed: %d\n", result);
        WSACleanup();
    }

    // Create socket
    sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock == INVALID_SOCKET) {
        printf("socket() failed\n");
        freeaddrinfo(res);
        WSACleanup();
    }

    // Connect
    result = connect(sock, res->ai_addr, (int)res->ai_addrlen);
    if (result == SOCKET_ERROR) {
        printf("Connection failed\n");
        closesocket(sock);
        freeaddrinfo(res);
        WSACleanup();
    }

    //printf("Connected to server!\n");

    // Send message
    instruction = 1;
    struct initPacket packet;
    Uint64 t0 = SDL_GetTicks();
    send(sock, (char*)&instruction, sizeof(instruction), 0);
    send(sock, (char*)gameState->entityState.name, sizeof(gameState->entityState.name), 0);
    // recv(sock, (char*)&gameState->entityState.id, sizeof(gameState->entityState.id), 0);
    recv(sock, (char*)&packet, sizeof(struct initPacket), 0);
    Uint64 t1 = SDL_GetTicks();
    gameState->serverOffsetTime = packet.timeStamp - ((t1 - t0)/2 + t0);
    gameState->entityState.id = packet.id;
    
    
    //printf("%d %d %d %d\n", init_payload.id,init_payload.world[0][0].block, init_payload.world[51][50].block, sizeof(struct world_payload));
    
    // Receive reply
    // char buffer[1024] = {0};
    // recv(sock, buffer, sizeof(buffer), 0);
    // printf("Received: %s\n", buffer);
}

struct world world_inbound[20][20];
struct player players[32];
int count = 0;


void client_iterate(game *gameState) {
    struct packet packet;
    instruction = 2;
    send(sock, (char*)&instruction, sizeof(instruction), 0);
    vec2 originalPosition = gameState->entityState.position;
    struct player send_packet;
    send_packet.id = gameState->entityState.id;
    strcpy(send_packet.name, gameState->entityState.name);
    send_packet.position_a = originalPosition;
    send(sock, (char*)&send_packet, sizeof(struct player), 0);
    int byte = recv(sock, (char*)&packet, sizeof(packet), 0);
    if (byte != sizeof(packet)) return;
    if (packet.status != 200) return;

    for (int y=0;y<MAXWORLD;y++) {
        int tempY = originalPosition.y-MAXWORLD/2 + y;
        for (int x=0;x<MAXWORLD;x++) {
            int tempX = originalPosition.x-MAXWORLD/2 + x;
            gameState->world[tempY][tempX].block = packet.world[y][x].block;
            gameState->world[tempY][tempX].background = packet.world[y][x].background;
            gameState->world[tempY][tempX].coords = (vec2){tempX, tempY};
        }
    }
    for (int i=0;i<32;i++) {
        if (packet.players[i].id != gameState->entityState.id && packet.players[i].id != 0) {
            if (gameState->surroundingPlayer[i].id == 0) {
                gameState->surroundingPlayer[i].position = (vec2) {50.5f, 50.6f};
            }
            gameState->surroundingPlayer[i].id = packet.players[i].id;
            if (!vec2_compare(gameState->surroundingPlayer[i].targetPos, gameState->surroundingPlayer[i].startPos) &&
                vec2_compare(gameState->surroundingPlayer[i].targetPos, packet.players[i].position_b) &&
                vec2_compare(gameState->surroundingPlayer[i].startPos, packet.players[i].position_a)) continue;
            gameState->surroundingPlayer[i].targetPos = packet.players[i].position_b;
            gameState->surroundingPlayer[i].startPos = gameState->surroundingPlayer[i].position;
            gameState->surroundingPlayer[i].prevTimeStamp = packet.players[i].timeStamp_a - gameState->serverOffsetTime;
            gameState->surroundingPlayer[i].timeStamp = packet.players[i].timeStamp_b - gameState->serverOffsetTime;
            strcpy(gameState->surroundingPlayer[i].name, packet.players[i].name);
            gameState->surroundingPlayer[i].currentTimeStamp = SDL_GetTicks();
        } else gameState->surroundingPlayer[i].id = 0;
    }
    gameState->isClientBusy = 0;
}

DWORD WINAPI change_tile_thread(LPVOID lpParam) {
    tile *currentTile = (tile*)lpParam;
    int status;
    instruction = 3;
    // printf("change tile on %f %f\n", currentTile->coords.x, currentTile->coords.y);
    send(sock, (char*)&instruction, sizeof(instruction), 0);
    send(sock, (char*)&(struct update_world){(vec2_int){currentTile->coords.x, currentTile->coords.y}, (struct world){currentTile->block, currentTile->background}}, sizeof(struct update_world), 0);
    
    recv(sock, (char*)&status, sizeof(status), 0);
    return 0;
}

void tile_change(tile *arg) {
    CreateThread(NULL, 0, change_tile_thread, arg, 0, NULL);
}

void client_quit() {
    printf("quit\n");
    closesocket(sock);
    WSACleanup();
}