#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <enet/enet.h>
#include <string.h>
#include <stdint.h>
#include <packet.h>

#define PORT 55555
#define MAX_WORLD_COUNT 32

typedef struct worldInfo {
    char worldName[50];
    int port;
    uint8_t running;
} worldInfo;

worldInfo worldTable[MAX_WORLD_COUNT];

int find_server_index(char worldName[50]) {
    int port = 6000;
    int index = -1;
    for (int i=0;i<MAX_WORLD_COUNT;i++) {
        if (strcmp(worldTable[i].worldName, worldName) == 0 && worldTable[i].running) {
            return i;
        }
        if (worldTable[i].running == 0) {
            return i;
        }
    }
}

void launch_world_server(const char worldName[50], int port) {
    char cmd[256];
    sprintf(cmd, "./server.exe --world %s --port %d", worldName, port);

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    BOOL success = CreateProcessA(
        NULL,       // application name
        cmd,        // command line
        NULL, NULL, FALSE, 0,
        NULL, NULL, &si, &pi
    );

    if (success) {
        printf("World %s started on port %d\n", worldName, port);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        printf("Failed to start world server\n");
    }
}

int handle_message(ENetPeer* peer, ENetPacket* packet, ENetHost *server) {
    header *messageHeader = (header*)packet->data;
    switch(messageHeader->type) {
        case CONNECT_LOGIN: {
            loginInput *receivedPakcet = (loginInput*)packet->data;
            
            loginOutput sendData;
            strcpy(sendData.playerName, receivedPakcet->playerName);
            sendData.status = 123;
            ENetPacket *sendPacket = enet_packet_create((char*)&sendData, sizeof(sendData), ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(peer, 0, sendPacket);
            enet_host_flush(server);
            break;
        }
        case CONNECT_WORLD: {
            worldRequestPacket *receivedPakcet = (worldRequestPacket*)packet->data;
            int index = find_server_index(receivedPakcet->worldName);
            if (worldTable[index].running == 0) {
                launch_world_server(receivedPakcet->worldName, worldTable[index].port);
                worldTable[index].running = 1;
                strcpy(worldTable[index].worldName, receivedPakcet->worldName);
            }
            worldConnectPacket sendData = {BROADCAST_WORLD, worldTable[index].port};
            ENetPacket *sendPacket = enet_packet_create((char*)&sendData, sizeof(sendData), ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(peer, 0, sendPacket);
            enet_host_flush(server);
            break;
        }
        case WORLD_SHUTDOWN: {
            WorldShutdownNotify *receivedPakcet = (WorldShutdownNotify*)packet->data;
            for (int i=0;i<MAX_WORLD_COUNT;i++) {
                if (worldTable[i].port == receivedPakcet->port) {
                    worldTable[i].running = 0;
                    break;
                }
            }
            break;
        }
    }
    return 1;
}

int main() {
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
    for (int i=0;i<MAX_WORLD_COUNT;i++) {
        worldTable[i].port = 6000 + i;
        worldTable[i].running = 0;
    }
    printf("Server listening on port %d...\n", PORT);
    ENetEvent event;
    while (1) {
        while (enet_host_service(server, &event, 1000) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    break;
                case ENET_EVENT_TYPE_RECEIVE:
                    // header *messageHeader = (header*)event.packet->data;
                    handle_message(event.peer, event.packet, server);
                    enet_packet_destroy(event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    // printf("Client disconnected.\n");
                    break;
                default:
                    break;
            }
        }
    }

    return 0;
}