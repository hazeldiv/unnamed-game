#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vec2.h>
#include <enet/enet.h>
#include <packet.h>
#include <stdint.h>
#include <windows.h>
#include <SDL3/SDL.h>

#define PORT 55555
world worldState;
uint8_t playerCount;
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
            worldState.clienState[index].data.position_a = worldState.clienState[index].data.position_b;
            worldState.clienState[index].data.position_b = receivedPakcet->position;
            worldState.clienState[index].data.timeStamp_a = worldState.clienState[index].data.timeStamp_b;
            worldState.clienState[index].data.timeStamp_b = (uint64_t)SDL_GetTicks();
            break;
        }
        case TYPE_TILE: {
            tileUpdate *receivedPakcet = (tileUpdate*)packet->data;
            worldState.tile[receivedPakcet->position.y][receivedPakcet->position.x].block = receivedPakcet->tile.block;
            worldState.tile[receivedPakcet->position.y][receivedPakcet->position.x].background = receivedPakcet->tile.background;
            int index = get_client_index(worldState.clienState, peer);
            tileUpdate sendData = {BROADCAST_TILE, receivedPakcet->tile, receivedPakcet->position};
            ENetPacket *sendPacket = enet_packet_create((char*)&sendData, sizeof(sendData), ENET_PACKET_FLAG_RELIABLE);
            for (int i=0;i<MAX_PLAYER;i++) {
                if (worldState.clienState[i].peer != NULL && worldState.clienState[i].peer != peer && worldState.clienState[i].peer->state == ENET_PEER_STATE_CONNECTED) {
                    enet_peer_send(worldState.clienState[i].peer, 0, sendPacket);
                }
            }
            enet_host_flush(server);
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
    playerCount--;
}

DWORD WINAPI listener(LPVOID lpParam) {
    printf("Server listening on port %d...\n", PORT);
    ENetHost *server = (ENetHost*)lpParam;
    ENetEvent event;
    while (1) {
        while (enet_host_service(server, &event, 1000) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    playerCount++;
                    printf("A new client connected. Player count : %d\n", playerCount);
                    break;
                case ENET_EVENT_TYPE_RECEIVE:
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
            if (worldState.clienState[i].peer != NULL && worldState.clienState[i].peer->state == ENET_PEER_STATE_CONNECTED) {
                ENetPacket* sendPacket = enet_packet_create(&sendData, sizeof(sendData), ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);
                enet_peer_send(worldState.clienState[i].peer, 0, sendPacket);
            }
        }
        enet_host_flush(server);
    }
    
    WaitForSingleObject(threadHandle, INFINITE);
    CloseHandle(threadHandle);

    enet_host_destroy(server);
    enet_deinitialize();
    return 0;
}