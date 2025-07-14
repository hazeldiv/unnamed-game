#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <game.h>
#include <tile.h>
#include <stdlib.h>

#pragma comment(lib, "Ws2_32.lib")

#define SERVER_HOST "29.ip.gl.ply.gg"
#define SERVER_PORT "33009"

// localhost
// #define SERVER_HOST "127.0.0.1"
// #define SERVER_PORT "25565"

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
    int id;
    vec2 position;
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
    send(sock, (char*)&instruction, sizeof(instruction), 0);
    send(sock, (char*)gameState->entityState.name, sizeof(gameState->entityState.name), 0);

    int received_id;
    recv(sock, (char*)&gameState->entityState.id, sizeof(gameState->entityState.id), 0);
    printf("%d\n",gameState->entityState.id);
    
    //printf("%d %d %d %d\n", init_payload.id,init_payload.world[0][0].block, init_payload.world[51][50].block, sizeof(struct world_payload));
    
    // Receive reply
    // char buffer[1024] = {0};
    // recv(sock, buffer, sizeof(buffer), 0);
    // printf("Received: %s\n", buffer);
}

static int getWorldSize = 16;
struct world world_inbound[16][16];
struct player players[32];
Uint64 prevTimeStamp = 0;

void client_iterate(game *gameState) {
    instruction = 2;
    send(sock, (char*)&instruction, sizeof(instruction), 0);
    //printf("sending from id : %d\n", gameState->entityState.id);
    send(sock, (char*)&(struct player){gameState->entityState.id, gameState->entityState.position}, sizeof(struct player), 0);
    recv(sock, (char*)&world_inbound, sizeof(world_inbound), 0);
    recv(sock, (char*)players, sizeof(players), 0);
    Uint64 timeStamp;
    recv(sock, (char*)&timeStamp, sizeof(timeStamp), 0);
    vec2 originalPos;
    recv(sock, (char*)&originalPos, sizeof(vec2), 0);
    for (int y=0;y<getWorldSize;y++) {
        int tempY = originalPos.y-getWorldSize/2 + y;
        for (int x=0;x<getWorldSize;x++) {
            int tempX = originalPos.x-getWorldSize/2 + x;
            gameState->world[tempY][tempX].block = world_inbound[y][x].block;
            gameState->world[tempY][tempX].background = world_inbound[y][x].background;
            gameState->world[tempY][tempX].coords = (vec2){tempX, tempY};
        }
    }
    if (prevTimeStamp != 0) {
        for (int i=0;i<32;i++) {
            if (players[i].id != gameState->entityState.id && players[i].id != -1) {
                gameState->surroundingPlayer[i].id = players[i].id;
                gameState->surroundingPlayer[i].velocity.x = (players[i].position.x - gameState->surroundingPlayer[i].position.x)/((timeStamp-prevTimeStamp)/1000.0f);
                gameState->surroundingPlayer[i].velocity.y = (players[i].position.y - gameState->surroundingPlayer[i].position.y)/((timeStamp-prevTimeStamp)/1000.0f)*-1;
            }
        }
    }
    prevTimeStamp = timeStamp;
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