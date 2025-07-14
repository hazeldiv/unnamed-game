#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <entity.h>
#include <game.h>
#include <tile.h>
#include <string.h>
#include <stdlib.h>
#include <vec2.h>

#pragma comment(lib, "Ws2_32.lib")  // Link with Winsock library

#define PORT 25565
#define MAXPLAYER 32

game *gameState;

struct world {
    Uint8 block;
    Uint8 background;
};

struct player {
    int id;
    vec2 position;
};

struct update_world {
    vec2_int position;
    struct world currentTile;
};

int playerCount = 0;
int idCount = 0;
struct player players[MAXPLAYER];
struct world currentWorld[100][100];
static int getWorldSize = 16;
struct world worldPayload[16][16];

void generate_world(struct world world[100][100]) {
    for (int y=0;y<100;y++) {
        for (int x=0;x<100;x++) {
            world[y][x].block = 0;
            world[y][x].background= 0;
            if (y>50) {
                world[y][x].block = 1;
                if (y>60) {
                    world[y][x].block = 2;
                }
            }
        }
    }
}

int find_empty_index() {
    for (int i=0;i<MAXPLAYER;i++) {
        if (players[i].id == -1) {
            return i;
        }
    }
    return -1;
}

DWORD WINAPI ClientHandler(void* arg) {
    SOCKET clientSocket = (SOCKET)arg;
    int bytes, instruction, currentId, status, index;
    entity received;
    if (playerCount >= MAXPLAYER) {
        closesocket(clientSocket);
        return 0;
    }
    struct player client;
    struct update_world updatedTile;
    struct player playersPayload[MAXPLAYER];
    while (1) {
        bytes = recv(clientSocket, (char*)&instruction, sizeof(instruction), 0);
        if (bytes <= 0) break;
        if (instruction == 1) {
            bytes = recv(clientSocket, (char*)gameState->players[playerCount].name, sizeof(gameState->players[playerCount].name), 0);
            playerCount++;
            idCount++;
            currentId = idCount;
            index = find_empty_index();
            players[index].id = currentId;
            send(clientSocket, (char*)&currentId, sizeof(currentId), 0);
            printf("New Client from id : %d\n", currentId);
        } else if (instruction == 2) {
            bytes = recv(clientSocket, (char*)&client, sizeof(struct player), 0);
            players[index].position = client.position;
            for (int y=0;y<getWorldSize;y++) {
                int tempY = client.position.y-getWorldSize/2 + y;
                for (int x=0;x<getWorldSize;x++) {
                    int tempX = client.position.x-getWorldSize/2 + x;
                    worldPayload[y][x].block = currentWorld[tempY][tempX].block;
                    worldPayload[y][x].background = currentWorld[tempY][tempX].background;
                }
            }
            
            //if (received.id == currentId) {
                //printf("send world to %d %d | size : %d\n", received.id, gameState->world[51][50].block, sizeof(game));
            send(clientSocket, (char*)&worldPayload, sizeof(worldPayload), 0);
            send(clientSocket, (char*)players, sizeof(players), 0);
            Uint64 now = SDL_GetTicks();
            send(clientSocket, (char*)&now, sizeof(now), 0);
            send(clientSocket, (char*)&client.position, sizeof(vec2), 0);
            //} else break;
        } else if (instruction == 3) {
            recv(clientSocket, (char*)&(updatedTile), sizeof(struct update_world), 0);
            currentWorld[updatedTile.position.y][updatedTile.position.x].block = updatedTile.currentTile.block;
            currentWorld[updatedTile.position.y][updatedTile.position.x].background = updatedTile.currentTile.background;
            status = 200;
            send(clientSocket, (char*)&status, sizeof(status), 0);
        } else break;
    }
    playerCount--;
    players[index].id = -1;
    printf("Client id %d disconnected.\n", currentId);
    closesocket(clientSocket);
    return 0;
}

int main() {
    
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientSize = sizeof(clientAddr);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        printf("WSAStartup failed\n");
        return 1;
    }

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        WSACleanup();
        return 1;
    }

    // Bind
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind failed\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    gameState = (game*)malloc(sizeof(game));
    
    generate_world(currentWorld);
    for (int i=0;i<MAXPLAYER;i++) {
        players[i].id = -1;
    }
    // Listen
    listen(serverSocket, SOMAXCONN);
    printf("Server listening on port %d...\n", PORT);

    // Receive
    
    while (1) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientSize);
        if (clientSocket == INVALID_SOCKET) continue;
        //printf("New Client from : %d\n", (int)clientSocket);
        CreateThread(NULL, 0, ClientHandler, (void*)clientSocket, 0, NULL);


        // if (bytes == sizeof(received)) {
        //     printf("Got Player: ID=%s X=%.2f Y=%.2f\n", received.name, received.x, received.y);
        // } else {
        //     printf("Incomplete struct received. Got %d bytes\n", bytes);
        // }
        
        
    }
    

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    while(1);
    return 0;
}