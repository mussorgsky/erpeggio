#include <stdio.h>
#include <stdlib.h>

typedef struct vec2 {
    int x, y;
} vec2;

typedef struct door {
    vec2 pos;
    struct door *target;
} door;

typedef struct room {
    vec2 topLeft, dimensions;
    int doorCount;
    door *doors;
} room;

typedef struct level {
    vec2 size;
    int roomCount;
    room *rooms;
} level;

void initRoom(room *r, int tlX, int tlY, int w, int h) {
    r->topLeft.x = tlX;
    r->topLeft.y = tlY;
    r->dimensions.x = w;
    r->dimensions.y = h;
}

#define scrnX 15
#define scrnY 10
char screen[scrnY][scrnX];

int main() {
    level hub;
    hub.size.x = scrnX;
    hub.size.y = scrnY;
    hub.roomCount = 2;
    hub.rooms = malloc(hub.roomCount * sizeof(room));

    initRoom(&hub.rooms[0], 1, 3, 6, 4);
    initRoom(&hub.rooms[1], 8, 3, 6, 4);

    hub.rooms[0].doorCount = 2;
    hub.rooms[1].doorCount = 2;
    hub.rooms[0].doors = malloc(hub.rooms[0].doorCount * sizeof(door));
    hub.rooms[1].doors = malloc(hub.rooms[1].doorCount * sizeof(door));

    hub.rooms[0].doors[0].pos.x = 5;
    hub.rooms[0].doors[0].pos.y = 1;

    hub.rooms[0].doors[1].pos.x = 5;
    hub.rooms[0].doors[1].pos.y = 2;

    hub.rooms[1].doors[0].pos.x = 0;
    hub.rooms[1].doors[0].pos.y = 0;

    hub.rooms[1].doors[1].pos.x = 0;
    hub.rooms[1].doors[1].pos.y = 3;

    /*
        Screen displaying starts here
    */
    int x, y, i, j;
    for(y = 0; y < scrnY; y++) {
        for(x = 0; x < scrnX; x++) {
            screen[y][x] = '.';
        }
    }

    room *r;
    door *d;
    for(i = 0; i < hub.roomCount; i++) {
        r = &hub.rooms[i];
        for(y = r->topLeft.y; y < r->topLeft.y + r->dimensions.y; y++) {
            for(x = r->topLeft.x; x < r->topLeft.x + r->dimensions.x; x++) {
                screen[y][x] = ' ';
            }
        }

        for(j = 0; j < r->doorCount; j++) {
            screen[r->topLeft.y + r->doors[j].pos.y][r->topLeft.x + r->doors[j].pos.x] = 'O';
        }
    }

    for(y = 0; y < scrnY; y++) {
        for(x = 0; x < scrnX; x++) {
            printf("%c", screen[y][x]);
        }
        printf("\n");
    }


    free(hub.rooms);
    return 0;
}