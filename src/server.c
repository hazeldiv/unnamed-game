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
#define MAXWORLD 20

game *gameState;

struct world {
    Uint8 block;
    Uint8 background;
};

struct player {
    Uint8 id;
    vec2 position;
};

struct update_world {
    vec2_int position;
    struct world currentTile;
};

struct packet {
    struct world world[MAXWORLD][MAXWORLD];
    struct player players[MAXPLAYER];
    Uint64 timeStamp;
    Uint16 status;
};

int playerCount = 0;
int idCount = 0;
struct player players[MAXPLAYER];
struct world currentWorld[100][100];

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
        if (players[i].id == 0) {
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
    struct packet packet;
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
            if (currentId == 0) break;
            bytes = recv(clientSocket, (char*)&client, sizeof(struct player), 0);
            if (client.id == currentId) {
                if (bytes != sizeof(struct player)) {
                    packet.status = 400;
                    send(clientSocket, (char*)&packet, sizeof(packet), 0);
                }
                players[index].position = client.position;
                for (int i=0;i<MAXPLAYER;i++) {
                    if (players[i].id != 0) {
                        packet.players[i].id = players[i].id;
                        packet.players[i].position = players[i].position;
                    } else {
                        packet.players[i].id = 0;
                    }
                }
                // packet.players[index].position = client.position;
                for (int y=0;y<MAXWORLD;y++) {
                    int tempY = client.position.y-MAXWORLD/2 + y;
                    for (int x=0;x<MAXWORLD;x++) {
                        int tempX = client.position.x-MAXWORLD/2 + x;
                        packet.world[y][x].block = currentWorld[tempY][tempX].block;
                        packet.world[y][x].background = currentWorld[tempY][tempX].background;
                    }
                }
                packet.status = 200;
                packet.timeStamp = SDL_GetTicks();
                
                send(clientSocket, (char*)&packet, sizeof(packet), 0);
            }
            
        } else if (instruction == 3) {
            if (currentId == 0) break;
            recv(clientSocket, (char*)&(updatedTile), sizeof(struct update_world), 0);
            currentWorld[updatedTile.position.y][updatedTile.position.x].block = updatedTile.currentTile.block;
            currentWorld[updatedTile.position.y][updatedTile.position.x].background = updatedTile.currentTile.background;
            status = 200;
            send(clientSocket, (char*)&status, sizeof(status), 0);
        } else break;
    }
    if (currentId != 0) {
        playerCount--;
        players[index].id = 0;
    }
    
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
        players[i].id = 0;
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