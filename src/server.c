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

static int playerCount = 0;
game *gameState;

struct world {
    Uint8 block;
    Uint8 background;
};

struct world currentWorld[100][100];
struct world worldPayload[20][20];

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

DWORD WINAPI ClientHandler(void* arg) {
    SOCKET clientSocket = (SOCKET)arg;
    int bytes, instruction, currentId;
    entity received;
    if (playerCount >= 10) {
        closesocket(clientSocket);
        return 0;
    }
    tile tile;
    vec2 clientPos;
    while (1) {
        bytes = recv(clientSocket, (char*)&instruction, sizeof(instruction), 0);
        if (bytes <= 0) break;
        printf("instruction : %d\n", instruction);
        if (instruction == 1) {
            bytes = recv(clientSocket, gameState->players[playerCount].name, sizeof(gameState->players[playerCount].name), 0);
            playerCount++;
            currentId = playerCount;
            send(clientSocket, &currentId, sizeof(currentId), 0);
        } else if (instruction == 2) {
            bytes = recv(clientSocket, &clientPos, sizeof(vec2), 0);
            //printf("%f %f\n", clientPos.x,clientPos.y);
            for (int y=0;y<20;y++) {
                int tempY = clientPos.y-10 + y;
                for (int x=0;x<20;x++) {
                    int tempX = clientPos.x-10 + x;
                    worldPayload[y][x].block = currentWorld[tempY][tempX].block;
                    worldPayload[y][x].background = currentWorld[tempY][tempX].background;
                    //if (tempY==51 && tempX==50) printf("%d %d %d\n", currentWorld[51][50].block,y,x);
                }
            }
            
            //if (received.id == currentId) {
                //printf("send world to %d %d | size : %d\n", received.id, gameState->world[51][50].block, sizeof(game));
                send(clientSocket, &worldPayload, sizeof(worldPayload), 0);
            //} else break;
        } else if (instruction == 3) {
            bytes = recv(clientSocket, (char*)&tile, sizeof(tile), 0);
            gameState->world[(int)tile.coords.y][(int)tile.coords.x] = tile;
        } else break;
    }
    //printf("Client %d disconnected.\n", (int)clientSocket);
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
    // Listen
    listen(serverSocket, SOMAXCONN);
    printf("Server listening on port %d...\n", PORT);

    // Receive
    
    while (1) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientSize);
        if (clientSocket == INVALID_SOCKET) continue;
        printf("New Client from : %d\n", (int)clientSocket);
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