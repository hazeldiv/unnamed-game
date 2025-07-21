#include <stdio.h>
#include <game.h>
#include <tile.h>
#include <stdlib.h>
#include <string.h>
#include <enet/enet.h>
#include <client.h>

// #define SERVER_HOST "29.ip.gl.ply.gg"
// #define SERVER_PORT 33009

// #define SERVER_HOST "voice-bracelets.gl.at.ply.gg"
// #define SERVER_PORT 33640

// localhost
// #define SERVER_HOST "127.0.0.1"
// #define SERVER_PORT 55555

#define SERVER_HOST "152.42.223.96"
#define SERVER_PORT 55555

static ENetHost *client;
ENetPeer *peer;

void client_iterate(game *gameState) {
    ENetEvent event;
    
    while(1) {
        while (enet_host_service(client, &event, 1000) > 0) {
            
            if (event.type == ENET_EVENT_TYPE_RECEIVE) {
                clientHeader *messageHeader = (clientHeader*)event.packet->data;
                switch (messageHeader->type) {
                    case BROADCAST_POSITION: {
                        positionBroadcast *receivedData = (positionBroadcast*)event.packet->data;
                        for (int i=0;i<32;i++) {
                            if (receivedData->players[i].id != gameState->entityState.id && receivedData->players[i].id != 0) {
                                if (gameState->surroundingPlayer[i].id == 0) {
                                    gameState->surroundingPlayer[i].position = (vec2) {50.5f, 50.6f};
                                }
                                
                                gameState->surroundingPlayer[i].id = receivedData->players[i].id;
                                if (!vec2_compare(gameState->surroundingPlayer[i].targetPos, gameState->surroundingPlayer[i].startPos) &&
                                    vec2_compare(gameState->surroundingPlayer[i].targetPos, receivedData->players[i].position_b) &&
                                    vec2_compare(gameState->surroundingPlayer[i].startPos, receivedData->players[i].position_a)) continue;
                                gameState->surroundingPlayer[i].targetPos = receivedData->players[i].position_b;
                                gameState->surroundingPlayer[i].startPos = gameState->surroundingPlayer[i].position;
                                gameState->surroundingPlayer[i].prevTimeStamp = receivedData->players[i].timeStamp_a - gameState->serverOffsetTime;
                                gameState->surroundingPlayer[i].timeStamp = receivedData->players[i].timeStamp_b - gameState->serverOffsetTime;
                                strcpy(gameState->surroundingPlayer[i].name, receivedData->players[i].name);
                                gameState->surroundingPlayer[i].currentTimeStamp = SDL_GetTicks();
                            } else gameState->surroundingPlayer[i].id = 0;
                        }
                        break;
                    }
                    case BROADCAST_TILE: {
                        tileUpdate *receivedData = (tileUpdate*)event.packet->data;
                        gameState->world[receivedData->position.y][receivedData->position.x].block = receivedData->tile.block;
                        gameState->world[receivedData->position.y][receivedData->position.x].background = receivedData->tile.background;
                        break;
                    }
                    case BROADCAST_CHAT: {

                        break;
                    }
                }
                enet_packet_destroy(event.packet);
            }
        }
    } 
}

void client_init(game *gameState) {
    gameState->isClientBusy = 1;

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
                    gameState->world[y][x].coords.x = x;
                    gameState->world[y][x].coords.y = y;
                }
            }
            enet_packet_destroy(event.packet);
            gameState->currentScene = 1;
        } else {
            printf("Connection failed.\n");
            enet_peer_reset(peer);
            gameState->currentScene = 2;
        }
    } else {
        printf("Connection failed.\n");
        enet_peer_reset(peer);
        gameState->currentScene = 2;
    }
    
    gameState->isClientBusy = 0;
    SDL_Thread *thread = SDL_CreateThread((SDL_ThreadFunction)client_iterate, "listen_tile_update", gameState);
    SDL_DetachThread(thread);
}


void update_position(game *gameState) {
    gameState->isClientBusy = 1;
    vec2 originalPosition = gameState->entityState.position;
    positionUpdate sendPacket = {TYPE_POSITION, gameState->entityState.id, originalPosition};
    ENetPacket *packet = enet_packet_create((char*)&sendPacket, sizeof(sendPacket), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
    enet_host_flush(client);
    
    gameState->isClientBusy = 0;
}

void tile_update_thread(tile *tile) {
    tileUpdate sendData;
    sendData.tile = (simpleTile){tile->block, tile->background};
    sendData.position = (vec2_int){(int)tile->coords.x, (int)tile->coords.y};
    sendData.type = TYPE_TILE;
    ENetPacket *packet = enet_packet_create((char*)&sendData, sizeof(sendData), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
    enet_host_flush(client);
}



void tile_update(tile *tile) {
    SDL_Thread *thread = SDL_CreateThread((SDL_ThreadFunction)tile_update_thread, "tile_update", tile);
    SDL_DetachThread(thread);
}

void client_quit() {
    enet_host_destroy(client);
    enet_deinitialize();
}