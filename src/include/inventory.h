#ifndef INVENTORY_H
#define INVENTORY_H
#include <vec2.h>

typedef struct inventory {
    int inventoryItem[50];
    int slotCount;
    int isOpen;
    int slotSize;
    vec2 slotScreenPosition[50];
    int hotbarSize;
    int hotbarCount;
    vec2 hotbarScreenPosition[5];
    int hotbarItem[5];
    int selectedHotbar;
} inventory;

int getPressedHotbar(input input, inventory *inventory);

#endif