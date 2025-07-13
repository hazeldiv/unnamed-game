#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <game.h>

#pragma comment(lib, "Ws2_32.lib")

#define SERVER_HOST "29.ip.gl.ply.gg"
#define SERVER_PORT "33009"

SOCKET sock;

int instruction;

struct world {
    Uint8 block;
    Uint8 background;
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
    send(sock, &instruction, sizeof(instruction), 0);
    send(sock, gameState->entityState.name, sizeof(gameState->entityState.name), 0);

    int received_id;
    recv(sock, &gameState->entityState.id, sizeof(gameState->entityState.id), 0);
    printf("%d\n",gameState->entityState.id);
    
    //printf("%d %d %d %d\n", init_payload.id,init_payload.world[0][0].block, init_payload.world[51][50].block, sizeof(struct world_payload));
    
    // Receive reply
    // char buffer[1024] = {0};
    // recv(sock, buffer, sizeof(buffer), 0);
    // printf("Received: %s\n", buffer);
    
    return 0;
}

struct world world_inbound[20][20];
void client_iterate(game *gameState) {
    instruction = 2;
    send(sock, &instruction, sizeof(instruction), 0);
    //printf("sending from id : %d\n", gameState->entityState.id);
    send(sock, &gameState->entityState.position, sizeof(gameState->entityState.position), 0);

    recv(sock, &world_inbound, sizeof(world_inbound), 0);
    for (int y=0;y<20;y++) {
        int tempY = gameState->entityState.position.y-10 + y;
        for (int x=0;x<20;x++) {
            int tempX = gameState->entityState.position.x-10 + x;
            gameState->world[tempY][tempX].block = world_inbound[y][x].block;
            gameState->world[tempY][tempX].background = world_inbound[y][x].background;
        }
    }
    // for (int y=0;y<100;y++) {
    //     for (int x=0;x<100;x++) {
    //         gameState->world[y][x] = received.world[y][x];
    //     }
    // }
    // for (int i=0;i<10;i++) {
    //     gameState->players[i] = received.players[i];
    // }
}


void client_quit() {
    printf("quit\n");
    closesocket(sock);
    WSACleanup();
}