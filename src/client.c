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
    vec2 position;
};

struct packet {
    struct world world[MAXWORLD][MAXWORLD];
    struct player players[32];
    Uint64 timeStamp;
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

struct world world_inbound[20][20];
struct player players[32];
int count = 0;


void client_iterate(game *gameState) {
    struct packet packet;
    instruction = 2;
    // for (int i=0;i<32;i++) {
    //     if (gameState->surroundingPlayer[i].id != gameState->entityState.id && gameState->surroundingPlayer[i].id != 0) {
    //         printf("dsafasd %d %d\n", gameState->surroundingPlayer[i].id, i);
    //         gameState->surroundingPlayer[i].startPos = gameState->surroundingPlayer[i].position;
    //     }
    // }
    send(sock, (char*)&instruction, sizeof(instruction), 0);
    //printf("sending from id : %d\n", gameState->entityState.id);
    vec2 originalPosition = gameState->entityState.position;
    send(sock, (char*)&(struct player){gameState->entityState.id, originalPosition}, sizeof(struct player), 0);
    
    
    // recv(sock, (char*)&world_inbound, sizeof(world_inbound), 0);
    // recv(sock, (char*)players, sizeof(players), 0);
    // Uint64 timeStamp;
    // recv(sock, (char*)&timeStamp, sizeof(timeStamp), 0);
    int byte = recv(sock, (char*)&packet, sizeof(packet), 0);
    
    
    if (byte != sizeof(packet)) return;
    if (packet.status != 200) return;
    // printf("%d %d\n", packet.timeStamp, sizeof(packet));
    // printf("%f %f\n", originalPosition.x, originalPosition.y);
    for (int y=0;y<MAXWORLD;y++) {
        int tempY = originalPosition.y-MAXWORLD/2 + y;
        for (int x=0;x<MAXWORLD;x++) {
            int tempX = originalPosition.x-MAXWORLD/2 + x;
            gameState->world[tempY][tempX].block = packet.world[y][x].block;
            gameState->world[tempY][tempX].background = packet.world[y][x].background;
            gameState->world[tempY][tempX].coords = (vec2){tempX, tempY};
            // if (tempY==51 && x==9) {
                //printf("%d %f %d %d %d %d\n", packet.world[y][9].block, originalPosition.y, x,y,packet.world[y][x].block, packet.timeStamp);
            // }
        }
    }
    // Uint64 end = SDL_GetTicks();
    // int delta = end-prevTimeStamp;
    // delta = delta>300 ? 0 : 300-(delta);
    // SDL_Delay(delta);
    // if (count < 5) {
    //     count++;
    //     gameState->isClientBusy = 0;
    //     for (int i=0;i<32;i++) {
    //         if (packet.players[i].id != gameState->entityState.id && packet.players[i].id != 0) {
    //                 printf("%f %f\n", packet.players[i].position.x, packet.players[i].position.y);
    //         }
    //     }
    //     return;
    // }
    // if (prevTimeStamp != 0) {
        for (int i=0;i<32;i++) {
            if (packet.players[i].id != gameState->entityState.id && packet.players[i].id != 0) {
                if (gameState->surroundingPlayer[i].id == 0) {
                    gameState->surroundingPlayer[i].position = (vec2) {50.5f, 50.6f};
                }
                gameState->surroundingPlayer[i].id = packet.players[i].id;
                // gameState->surroundingPlayer[i].velocity.x = (packet.players[i].position.x - gameState->surroundingPlayer[i].position.x)/((float)(packet.timeStamp-prevTimeStamp)/1000.0f);
                gameState->surroundingPlayer[i].targetPos = packet.players[i].position;
                gameState->surroundingPlayer[i].startPos = gameState->surroundingPlayer[i].position;
                // gameState->surroundingPlayer[i].startPos = originalPosition;
                // gameState->surroundingPlayer[i].velocity.y = (packet.players[i].position.y - gameState->surroundingPlayer[i].position.y)/((float)(packet.timeStamp-prevTimeStamp)/1000.0f)*-1;
                // printf("%f %f %f %f %f\n", gameState->surroundingPlayer[i].velocity.y, packet.players[i].position.y - gameState->surroundingPlayer[i].position.y, packet.players[i].position.y, gameState->surroundingPlayer[i].position.y, ((float)(packet.timeStamp-prevTimeStamp)/1000.0f));
            } else gameState->surroundingPlayer[i].id = 0;
        }
    // }
    //prevTimeStamp = start;
    gameState->isClientBusy = 0;
    for (int i=0;i<32;i++) {
        if (gameState->surroundingPlayer[i].id != 0) {
            printf("done %f %f\n", gameState->surroundingPlayer[i].startPos.x, gameState->surroundingPlayer[i].targetPos.x);
        }
    }
    
    //printf("%d\n", SDL_GetTicks()-start);
    
}

// void client_test(game *gameState) {
//     Uint64 now = SDL_GetTicks() - 100;
//     int delta = timeStamp - prevTimeStamp;
//     if (delta > 0) {
//         //printf("%f\n", gameState->t);
//         gameState->t = (float)(now - prevTimeStamp)/(float)(delta);
//         if (gameState->t > 1.0f) gameState->t = 1.0f;
//         printf("%f %d %d %d\n", gameState->t, now, now - prevTimeStamp, delta);
//     } else gameState->t = 0;
// }

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