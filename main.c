#include <stdio.h>
#include <stdlib.h>

typedef struct {
    float x, y;
} vec2;

typedef struct {
    vec2 topLeft, dimensions;
} room;

typedef struct {
    vec2 size;
    room *rooms;
    int roomCount;
} level;

void initRoom(room *r, int tlX, int tlY, int w, int h) {
    r->topLeft.x = tlX;
    r->topLeft.y = tlY;
    r->dimensions.x = w;
    r->dimensions.y = h;
}

int main() {
    level hub;
    hub.size.x = 15;
    hub.size.y = 10;
    hub.roomCount = 2;
    hub.rooms = malloc(hub.roomCount * sizeof(room));

    initRoom(&hub.rooms[0], 1, 1, 5, 3);
    initRoom(&hub.rooms[1], 8, 5, 5, 3);

    int x, y, r;
    room *tRoom;
    for(y = 0; y < hub.size.y; y++) {
        for(x = 0; x < hub.size.x; x++) {
            int isRoom = 0;
            for(r = 0; r < hub.roomCount; r++) {
                tRoom = &hub.rooms[r];
                if(y >= tRoom->topLeft.y && y <= tRoom->topLeft.y + tRoom->dimensions.y && x >= tRoom->topLeft.x && x <= tRoom->topLeft.x + tRoom->dimensions.x) {
                    isRoom = 1;
                }
            }
            if(isRoom) {
                printf(" ");
            } else {
                printf("%c", 178);
            }
        }
        printf("\n");
    }


    free(hub.rooms);
    return 0;
}