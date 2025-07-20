#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vec2.h>
#include <enet/enet.h>
#include <packet.h>
#include <stdint.h>
#include <windows.h>

// #pragma comment(lib, "Ws2_32.lib")  // Link with Winsock library

#define PORT 25565

// struct world {
//     Uint8 block;
//     Uint8 background;
// };

// struct update_world {
//     vec2_int position;
//     struct world currentTile;
// };

// struct player {
//     Uint8 id;
//     char name[50];
//     vec2 position_a;
//     Uint64 timeStamp_a;
//     vec2 position_b;
//     Uint64 timeStamp_b;
// };

// struct initPacket {
//     Uint8 id;
//     Uint64 timeStamp;
//     Uint16 status;
// };

// struct packet {
//     struct world world[MAXWORLD][MAXWORLD];
//     struct player players[32];
//     Uint16 status;
// };

// int playerCount = 0;
// int idCount = 0;
// struct player players[MAXPLAYER];
// struct world currentWorld[100][100];



// int find_empty_index() {
//     for (int i=0;i<MAXPLAYER;i++) {
//         if (players[i].id == 0 && i!=30) {
//             return i;
//         }
//     }
//     return -1;
// }

// DWORD WINAPI ClientHandler(void* arg) {
//     SOCKET clientSocket = (SOCKET)arg;
//     int bytes, instruction, currentId, status, index;
//     entity received;
//     if (playerCount >= MAXPLAYER) {
//         closesocket(clientSocket);
//         return 0;
//     }
//     struct player client;
//     struct update_world updatedTile;
//     struct packet packet;
//     while (1) {
//         bytes = recv(clientSocket, (char*)&instruction, sizeof(instruction), 0);
//         if (bytes <= 0) break;
//         // printf("id %d send instruction %d\n", currentId, instruction);
//         if (instruction == 1) {
//             char name[50];
//             bytes = recv(clientSocket, name, sizeof(name), 0);
//             playerCount++;
//             idCount++;
//             currentId = idCount;
//             index = find_empty_index();
//             players[index].id = currentId;
//             strcpy(players[index].name, name);

//             struct initPacket payload = (struct initPacket) {currentId, SDL_GetTicks(), 200};
//             send(clientSocket, (char*)&payload, sizeof(payload), 0);
//             printf("New Client from id : %d\n", currentId);
//         } else if (instruction == 2) {
//             if (currentId == 0) break;
//             bytes = recv(clientSocket, (char*)&client, sizeof(struct player), 0);
//             if (client.id == currentId) {
//                 players[index].position_a = players[index].position_b;
//                 players[index].timeStamp_a = players[index].timeStamp_b;
//                 players[index].position_b = client.position_a;
//                 players[index].timeStamp_b = SDL_GetTicks();
//                 for (int i=0;i<MAXPLAYER;i++) {
//                     if (players[i].id != 0) {
//                         packet.players[i].id = players[i].id;
//                         packet.players[i].position_a = players[i].position_a;
//                         packet.players[i].timeStamp_a = players[i].timeStamp_a;
//                         packet.players[i].position_b = players[i].position_b;
//                         packet.players[i].timeStamp_b = players[i].timeStamp_b;
//                         strcpy(packet.players[i].name, players[i].name);
//                     } else {
//                         packet.players[i].id = 0;
//                     }
//                 }
//                 // packet.players[index].position = client.position;
//                 for (int y=0;y<MAXWORLD;y++) {
//                     int tempY = client.position_a.y-MAXWORLD/2 + y;
//                     for (int x=0;x<MAXWORLD;x++) {
//                         int tempX = client.position_a.x-MAXWORLD/2 + x;
//                         packet.world[y][x].block = currentWorld[tempY][tempX].block;
//                         packet.world[y][x].background = currentWorld[tempY][tempX].background;
//                     }
//                 }
//                 packet.status = 200;
//                 // printf("test %d\n", currentId);
//                 int byte = send(clientSocket, (char*)&packet, sizeof(packet), 0);
//                 // printf("send %d bytes to id %d\n", byte, currentId);
//             }
            
//         } else if (instruction == 3) {
//             if (currentId == 0) break;
//             int byte = recv(clientSocket, (char*)&(updatedTile), sizeof(struct update_world), 0);
//             printf("get %d bytes from id %d changing tile\n", byte, byte);
//             currentWorld[updatedTile.position.y][updatedTile.position.x].block = updatedTile.currentTile.block;
//             currentWorld[updatedTile.position.y][updatedTile.position.x].background = updatedTile.currentTile.background;
//             // printf("test a\n");
//             // status = 200;
//             // send(clientSocket, (char*)&status, sizeof(status), 0);
//         } else break;
//     }
//     if (currentId != 0) {
//         playerCount--;
//         players[index].id = 0;
//     }
    
//     printf("Client id %d disconnected.\n", currentId);
//     closesocket(clientSocket);
//     return 0;
// }
world worldState;


void generate_world() {
    for (int y=0;y<100;y++) {
        for (int x=0;x<100;x++) {
            worldState.tile[y][x].block = 0;
            worldState.tile[y][x].background= 0;
            if (y>50) {
                worldState.tile[y][x].block = 1;
                if (y>60) {
                    worldState.tile[y][x].block = 2;
                }
            }
        }
    }
}

int get_client_index(client clientState[MAX_PLAYER], ENetPeer *key) {
    for (int i=0;i<MAX_PLAYER;i++) {
        if (clientState[i].peer == key) {
            return i;
        }
    }
    return -1;
}
int currentId = 0;

int handle_message(ENetPeer* peer, ENetPacket* packet, ENetHost *server) {
    header *messageHeader = (header*)packet->data;
    switch(messageHeader->type) {
        case TYPE_INIT: {
            clientInit *receivedPakcet = (clientInit*)packet->data;
            int index = get_client_index(worldState.clienState, NULL);

            currentId++;
            worldState.clienState[index].data.id = currentId;
            worldState.clienState[index].peer = peer;
            strcpy(worldState.clienState[index].data.name, receivedPakcet->name);
            serverInit sendData = {currentId, (uint64_t)SDL_GetTicks()};
            for (int y=0;y<WORLD_HEIGHT;y++) {
                for (int x=0;x<WORLD_WIDTH;x++) {
                    sendData.tile[y][x].block = worldState.tile[y][x].block;
                    sendData.tile[y][x].background = worldState.tile[y][x].background;
                }
            }
            ENetPacket* sendPacket = enet_packet_create(&sendData, sizeof(sendData), ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(peer, 0, sendPacket);
            break;
        }
        case TYPE_POSITION: {
            positionUpdate *receivedPakcet = (positionUpdate*)packet->data;
            int index = get_client_index(worldState.clienState, peer);
            // if (receivedPakcet->id == worldState.clienState[index].data.id) {
            worldState.clienState[index].data.position_a = worldState.clienState[index].data.position_b;
            worldState.clienState[index].data.position_b = receivedPakcet->position;
            worldState.clienState[index].data.timeStamp_a = worldState.clienState[index].data.timeStamp_b;
            worldState.clienState[index].data.timeStamp_b = (uint64_t)SDL_GetTicks();

            // serverUpdate sendData;
            // for (int i=0;i<MAX_PLAYER;i++) {
            //     if (worldState.clienState[i].data.id != receivedPakcet->id && worldState.clienState[i].peer != NULL) {
            //         sendData.players[i].id =  worldState.clienState[i].data.id;
            //         strcpy(sendData.players[i].name, worldState.clienState[i].data.name);
            //         sendData.players[i].position_a = worldState.clienState[i].data.position_a;
            //         sendData.players[i].position_b = worldState.clienState[i].data.position_b;
            //         sendData.players[i].timeStamp_a = worldState.clienState[i].data.timeStamp_a;
            //         sendData.players[i].timeStamp_b = worldState.clienState[i].data.timeStamp_b;
            //     } else sendData.players[i].id = 0;
            // }
            // for (int y=0;y<MAX_WORLD;y++) {
            //     int tempY = receivedPakcet->position.y-MAX_WORLD/2 + y;
            //     for (int x=0;x<MAX_WORLD;x++) {
            //         int tempX = receivedPakcet->position.x-MAX_WORLD/2 + x;
            //         sendData.tile[y][x].block = worldState.tile[tempY][tempX].block;
            //         sendData.tile[y][x].background = worldState.tile[tempY][tempX].background;
            //     }
            // }

            // ENetPacket* sendPacket = enet_packet_create(&sendData, sizeof(sendData), ENET_PACKET_FLAG_RELIABLE);
            // enet_peer_send(peer, 0, sendPacket);
            // } return 0;
            break;
        }
        case TYPE_TILE: {
            tileUpdate *receivedPakcet = (tileUpdate*)packet->data;
            int index = get_client_index(worldState.clienState, peer);
            // simpleTile sendData = {receivedPakcet->tile.block, receivedPakcet->tile.background};
            for (int i=0;i<MAX_PLAYER;i++) {
                if (worldState.clienState[index].data.id != receivedPakcet->id) {
                    ENetPacket *packet = enet_packet_create((char*)&receivedPakcet, sizeof(receivedPakcet), ENET_PACKET_FLAG_RELIABLE);
                    enet_peer_send(worldState.clienState[i].peer, 0, packet);
                    enet_host_flush(server);
                }
            }

            break;
        }
        case TYPE_CHAT: {

            break;
        }
        case TYPE_DISCONNECT: {

            break;
        }
    }
    return 1;
}

void handle_disconnect(ENetPeer* peer) {
    int index = get_client_index(worldState.clienState, peer);
    worldState.clienState[index].peer = NULL;
    worldState.clienState[index].data.id = 0;
}

DWORD WINAPI listener(LPVOID lpParam) {
    printf("Server listening on port %d...\n", PORT);
    ENetHost *server = (ENetHost*)lpParam;
    ENetEvent event;
    while (1) {
        while (enet_host_service(server, &event, 1000) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    printf("A new client connected.\n");
                    break;
                case ENET_EVENT_TYPE_RECEIVE:
                    // printf("Received: %s\n", event.packet->data);
                    handle_message(event.peer, event.packet, server);
                    enet_packet_destroy(event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    handle_disconnect(event.peer);
                    printf("Client disconnected.\n");
                    break;
                default:
                    break;
            }
        }
    }
    
    return 0;
}

int main() {
    
    // WSADATA wsaData;
    // SOCKET serverSocket, clientSocket;
    // struct sockaddr_in serverAddr, clientAddr;
    // int clientSize = sizeof(clientAddr);

    // // Initialize Winsock
    // if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
    //     printf("WSAStartup failed\n");
    //     return 1;
    // }

    // // Create socket
    // serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    // if (serverSocket == INVALID_SOCKET) {
    //     printf("Socket creation failed\n");
    //     WSACleanup();
    //     return 1;
    // }

    // // Bind
    // serverAddr.sin_family = AF_INET;
    // serverAddr.sin_addr.s_addr = INADDR_ANY;
    // serverAddr.sin_port = htons(PORT);

    // if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
    //     printf("Bind failed\n");
    //     closesocket(serverSocket);
    //     WSACleanup();
    //     return 1;
    // }
    // gameState = (game*)malloc(sizeof(game));
    
    // generate_world(currentWorld);
    // for (int i=0;i<MAXPLAYER;i++) {
    //     players[i].id = 0;
    // }
    // // Listen
    // listen(serverSocket, SOMAXCONN);
    // printf("Server listening on port %d...\n", PORT);

    // // Receive
    
    // while (1) {
    //     clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientSize);
    //     if (clientSocket == INVALID_SOCKET) continue;
    //     //printf("New Client from : %d\n", (int)clientSocket);
    //     CreateThread(NULL, 0, ClientHandler, (void*)clientSocket, 0, NULL);


    //     // if (bytes == sizeof(received)) {
    //     //     printf("Got Player: ID=%s X=%.2f Y=%.2f\n", received.name, received.x, received.y);
    //     // } else {
    //     //     printf("Incomplete struct received. Got %d bytes\n", bytes);
    //     // }
        
        
    // }
    

    // closesocket(clientSocket);
    // closesocket(serverSocket);
    // WSACleanup();
    // while(1);
    generate_world();
    if (enet_initialize() != 0) {
        printf("ENet failed to initialize.\n");
        return 1;
    }

    ENetAddress address;
    ENetHost *server;

    address.host = ENET_HOST_ANY;
    address.port = PORT;

    server = enet_host_create(&address, 32, 2, 0, 0);
    if (!server) {
        printf("Failed to create server host.\n");
        return 1;
    }
    

    DWORD threadId;
    HANDLE threadHandle;
    CreateThread(NULL, 0, listener, server, 0, &threadId);

    while(1) {
        Sleep(100);
        positionBroadcast sendData;
        sendData.type = BROADCAST_POSITION;
        for (int i=0;i<MAX_PLAYER;i++) {
            if (worldState.clienState[i].peer != NULL) {
                sendData.players[i] = worldState.clienState[i].data;
            } else {
                sendData.players[i].id = 0;
            }
        }
        for (int i=0;i<MAX_PLAYER;i++) {
            if (worldState.clienState[i].peer != NULL) {
                ENetPacket* sendPacket = enet_packet_create(&sendData, sizeof(sendData), ENET_PACKET_FLAG_RELIABLE);
                printf("%d\n", sizeof(positionBroadcast));
                enet_peer_send(worldState.clienState[i].peer, 0, sendPacket);
                enet_host_flush(server);
            }
        }
    }
    
    WaitForSingleObject(threadHandle, INFINITE);
    CloseHandle(threadHandle);
    

    enet_host_destroy(server);
    enet_deinitialize();

    return 0;
}