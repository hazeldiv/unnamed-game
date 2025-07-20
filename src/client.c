#include <stdio.h>
// #include <winsock2.h>
// #include <ws2tcpip.h>
#include <game.h>
#include <tile.h>
#include <stdlib.h>
#include <string.h>
#include <enet/enet.h>
#include <client.h>

// #pragma comment(lib, "Ws2_32.lib")

// #define SERVER_HOST "29.ip.gl.ply.gg"
// #define SERVER_PORT 33009

// #define SERVER_HOST "voice-bracelets.gl.at.ply.gg"
// #define SERVER_PORT 33640

// localhost
#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT 25565

// SOCKET sock;

// int instruction;



// struct update_world {
//     vec2_int position;
//     struct world currentTile;
// };



// int recv_all(char* buffer, int length) {
//     int total = 0;
//     while (total < length) {
//         int byte = recv(sock, buffer+total, length-total, 0);
//         if (byte <= 0) return -1;
//         total += byte;
//     }
//     return total;
// }
static ENetHost *client;
ENetPeer *peer;

// void listen_tile_update(game *gameState) {
//     ENetEvent event;
//     while(1) {
//         while (enet_host_service(client, &event, 1000) > 0) {
            
//         }
//     }
// }

void client_iterate(game *gameState) {
    ENetEvent event;
    SDL_Mutex* enet_mutex = SDL_CreateMutex();
    // enet_mutex = SDL_CreateMutex();
    
    while(1) {
        // SDL_LockMutex(enet_mutex);
        while (enet_host_service(client, &event, 1000) > 0) {
            // printf("test %d\n", event.type);
            if (event.type == ENET_EVENT_TYPE_RECEIVE) {
            //     clientHeader *messageHeader = (clientHeader*)event.packet->data;
            //     // printf("message %d\n", messageHeader->type);
            //     switch (messageHeader->type) {
            //         case BROADCAST_POSITION: {
            //             // positionBroadcast *receivedData = (positionBroadcast*)event.packet->data;
            //             // for (int i=0;i<32;i++) {
            //             //     if (receivedData->players[i].id != gameState->entityState.id && receivedData->players[i].id != 0) {
            //             //         if (gameState->surroundingPlayer[i].id == 0) {
            //             //             gameState->surroundingPlayer[i].position = (vec2) {50.5f, 50.6f};
            //             //         }
                                
            //             //         gameState->surroundingPlayer[i].id = receivedData->players[i].id;
            //             //         if (!vec2_compare(gameState->surroundingPlayer[i].targetPos, gameState->surroundingPlayer[i].startPos) &&
            //             //             vec2_compare(gameState->surroundingPlayer[i].targetPos, receivedData->players[i].position_b) &&
            //             //             vec2_compare(gameState->surroundingPlayer[i].startPos, receivedData->players[i].position_a)) continue;
            //             //         gameState->surroundingPlayer[i].targetPos = receivedData->players[i].position_b;
            //             //         gameState->surroundingPlayer[i].startPos = gameState->surroundingPlayer[i].position;
            //             //         gameState->surroundingPlayer[i].prevTimeStamp = receivedData->players[i].timeStamp_a - gameState->serverOffsetTime;
            //             //         gameState->surroundingPlayer[i].timeStamp = receivedData->players[i].timeStamp_b - gameState->serverOffsetTime;
            //             //         strcpy(gameState->surroundingPlayer[i].name, receivedData->players[i].name);
            //             //         gameState->surroundingPlayer[i].currentTimeStamp = SDL_GetTicks();
            //             //     } else gameState->surroundingPlayer[i].id = 0;
            //             // }
            //             break;
            //         }
            //         case BROADCAST_TILE: {
            //             printf("change tile\n");
            //             tileUpdate *receivedData = (tileUpdate*)event.packet->data;
            //             gameState->world[receivedData->position.x][receivedData->position.y].block = receivedData->tile.block;
            //             gameState->world[receivedData->position.x][receivedData->position.y].background = receivedData->tile.background;
            //             break;
            //         }
            //         case BROADCAST_CHAT: {

            //             break;
            //         }
            //     }
                enet_packet_destroy(event.packet);
            // //     printf("bb\n");
            }
        }
        // enet_host_flush(client);
        
        // enet_packet_destroy(event.packet);
        // SDL_UnlockMutex(enet_mutex);
    } 
}

void client_init(game *gameState) {
    gameState->isClientBusy = 1;
    // WSADATA wsaData;
    // struct addrinfo hints, *res;
    // int result;

    // WSAStartup(MAKEWORD(2, 2), &wsaData);

    // // Setup hints
    // ZeroMemory(&hints, sizeof(hints));
    // hints.ai_family = AF_INET;
    // hints.ai_socktype = SOCK_STREAM;
    // hints.ai_protocol = IPPROTO_TCP;

    // // Resolve server address and port
    // result = getaddrinfo(SERVER_HOST, SERVER_PORT, &hints, &res);
    // if (result != 0) {
    //     printf("getaddrinfo failed: %d\n", result);
    //     WSACleanup();
    // }

    // // Create socket
    // sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    // if (sock == INVALID_SOCKET) {
    //     printf("socket() failed\n");
    //     freeaddrinfo(res);
    //     WSACleanup();
    // }

    // // Connect
    // result = connect(sock, res->ai_addr, (int)res->ai_addrlen);
    // if (result == SOCKET_ERROR) {
    //     printf("Connection failed\n");
    //     closesocket(sock);
    //     freeaddrinfo(res);
    //     WSACleanup();
    // }

    // //printf("Connected to server!\n");

    // // Send message
    // instruction = 1;
    // struct initPacket packet;
    // Uint64 t0 = SDL_GetTicks();
    // send(sock, (char*)&instruction, sizeof(instruction), 0);
    // send(sock, (char*)gameState->entityState.name, sizeof(gameState->entityState.name), 0);
    // // recv(sock, (char*)&gameState->entityState.id, sizeof(gameState->entityState.id), 0);
    // recv(sock, (char*)&packet, sizeof(struct initPacket), 0);
    // Uint64 t1 = SDL_GetTicks();
    // gameState->serverOffsetTime = receivedPacket->timeStamp - ((t1 - t0)/2 + t0);
    // gameState->entityState.id = receivedPacket->id;
    
    
    //printf("%d %d %d %d\n", init_payload.id,init_payload.world[0][0].block, init_payload.world[51][50].block, sizeof(struct world_payload));
    
    // Receive reply
    // char buffer[1024] = {0};
    // recv(sock, buffer, sizeof(buffer), 0);
    // printf("Received: %s\n", buffer);

    if (enet_initialize() != 0) {
        printf("ENet failed to initialize.\n");
    }

    client = enet_host_create(NULL, 1, 2, 0, 0);
    if (!client) {
        printf("Failed to create client host.\n");
    }

    ENetAddress address;
    

    enet_address_set_host(&address, SERVER_HOST);
    address.port = SERVER_PORT;

    peer = enet_host_connect(client, &address, 2, 0);
    if (!peer) {
        printf("Failed to initiate connection.\n");
    }
    ENetEvent event;
    if (enet_host_service(client, &event, 10000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
        clientInit sendPacket;
        sendPacket.type = TYPE_INIT;
        strcpy(sendPacket.name, gameState->entityState.name);

        Uint64 t0 = SDL_GetTicks();
        ENetPacket *packet = enet_packet_create((char*)&sendPacket, sizeof(sendPacket), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, packet);
        enet_host_flush(client);
        if (enet_host_service(client, &event, 10000) > 0 &&
            event.type == ENET_EVENT_TYPE_RECEIVE) {
            serverInit *receivedPacket = (serverInit*)event.packet->data;
            
            Uint64 t1 = SDL_GetTicks();
            gameState->serverOffsetTime = receivedPacket->timeStamp - ((t1 - t0)/2 + t0);
            gameState->entityState.id = receivedPacket->id;
            for (int y=0;y<WORLD_HEIGHT;y++) {
                for (int x=0;x<WORLD_WIDTH;x++) {
                    gameState->world[y][x].block = receivedPacket->tile[y][x].block;
                    gameState->world[y][x].background = receivedPacket->tile[y][x].background;
                }
            }
            enet_packet_destroy(event.packet);
            gameState->currentScene = 1;
        } else {
            printf("Connection failed.\n");
            enet_peer_reset(peer);
            gameState->currentScene = 2;
            // return 0;
        }
    } else {
        printf("Connection failed.\n");
        enet_peer_reset(peer);
        gameState->currentScene = 2;
        // return 0;
    }
    
    gameState->isClientBusy = 0;
    // return 1;
    SDL_Thread *thread = SDL_CreateThread((SDL_ThreadFunction)client_iterate, "listen_tile_update", gameState);
    SDL_DetachThread(thread);
}

// struct world world_inbound[20][20];
// struct player players[32];
// int count = 0;


void update_position(game *gameState) {
    gameState->isClientBusy = 1;
    // struct packet packet;
    // instruction = 2;
    // send(sock, (char*)&instruction, sizeof(instruction), 0);
    // vec2 originalPosition = gameState->entityState.position;
    // struct player send_packet;
    // send_receivedPacket->id = gameState->entityState.id;
    // strcpy(send_receivedPacket->name, gameState->entityState.name);
    // send_receivedPacket->position_a = originalPosition;
    // send(sock, (char*)&send_packet, sizeof(struct player), 0);
    // int byte = recv_all((char*)&packet, sizeof(packet));
    // if (byte != sizeof(packet)) return;
    // if (receivedPacket->status != 200) return;
    // printf("test\n");
    // enet_host_flush(client);
    vec2 originalPosition = gameState->entityState.position;
    // positionUpdate sendPacket = {TYPE_POSITION, gameState->entityState.id, originalPosition};
    // ENetPacket *packet = enet_packet_create((char*)&sendPacket, sizeof(sendPacket), ENET_PACKET_FLAG_RELIABLE);
    // enet_peer_send(peer, 0, packet);
    // enet_host_flush(client);
    
    // serverUpdate *receivedPacket;
    // ENetEvent event;
    // if (enet_host_service(client, &event, 5000) > 0 &&
    //     event.type == ENET_EVENT_TYPE_RECEIVE) {
    //     receivedPacket = (serverUpdate*)event.packet->data;
    //     enet_packet_destroy(event.packet);
    // }
    // } else {
    //     enet_peer_reset(peer);
    //     return;
    // }
    // printf("test 2\n");
    
    // for (int y=0;y<MAXWORLD;y++) {
    //     int tempY = originalPosition.y-MAXWORLD/2 + y;
    //     for (int x=0;x<MAXWORLD;x++) {
    //         int tempX = originalPosition.x-MAXWORLD/2 + x;
    //         gameState->world[tempY][tempX].block = receivedPacket->world[y][x].block;
    //         gameState->world[tempY][tempX].background = receivedPacket->world[y][x].background;
    //         gameState->world[tempY][tempX].coords = (vec2){tempX, tempY};
    //     }
    // }
    
    gameState->isClientBusy = 0;
    // printf("test 3\n");
}

// DWORD WINAPI change_tile_thread(LPVOID lpParam) {
//     tile *currentTile = (tile*)lpParam;
//     int status;
//     instruction = 3;
//     // printf("change tile on %f %f\n", currentTile->coords.x, currentTile->coords.y);
//     send(sock, (char*)&instruction, sizeof(instruction), 0);
//     send(sock, (char*)&(struct update_world){(vec2_int){currentTile->coords.x, currentTile->coords.y}, (struct world){currentTile->block, currentTile->background}}, sizeof(struct update_world), 0);
    
//     // recv(sock, (char*)&status, sizeof(status), 0);
//     return 0;
// }

void tile_update_thread(tile *tile) {
    tileUpdate sendData;
    sendData.tile = (simpleTile){tile->block, tile->background};
    sendData.position = (vec2_int){(int)tile->coords.x, (int)tile->coords.y};
    ENetPacket *packet = enet_packet_create((char*)&sendData, sizeof(sendData), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
    enet_host_flush(client);
}



void tile_update(tile *tile) {
    // SDL_Thread *thread = SDL_CreateThread((SDL_ThreadFunction)tile_update_thread, "listen_tile_update", tile);
    // SDL_DetachThread(thread);
}

void client_quit() {
    // printf("quit\n");
    // closesocket(sock);
    // WSACleanup();
    enet_host_destroy(client);
    enet_deinitialize();
}