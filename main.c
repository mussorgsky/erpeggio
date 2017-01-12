#include <stdio.h>
#include <stdlib.h>

typedef struct vec2 {
    int x, y;
} vec2;

typedef struct door {
    vec2 pos, parentPos;
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

typedef struct player {
    vec2 pos;
} player;

void initRoom(room *r, int tlX, int tlY, int w, int h) {
    r->topLeft.x = tlX;
    r->topLeft.y = tlY;
    r->dimensions.x = w;
    r->dimensions.y = h;
}

#define scrnX 80
#define scrnY 24
char screen[scrnY][scrnX];

int main() {
    level hub;
    hub.size.x = scrnX;
    hub.size.y = scrnY;
    hub.roomCount = 2;
    hub.rooms = malloc(hub.roomCount * sizeof(room));

    initRoom(&hub.rooms[0], 1, 3, 6, 4);
    initRoom(&hub.rooms[1], 10, 3, 6, 4);

    hub.rooms[0].doorCount = 2;
    hub.rooms[1].doorCount = 2;
    hub.rooms[0].doors = malloc(hub.rooms[0].doorCount * sizeof(door));
    hub.rooms[1].doors = malloc(hub.rooms[1].doorCount * sizeof(door));

    hub.rooms[0].doors[0].pos.x = 5;
    hub.rooms[0].doors[0].pos.y = 1;
    hub.rooms[0].doors[0].parentPos = hub.rooms[0].topLeft;
    hub.rooms[0].doors[0].target = &hub.rooms[1].doors[0];

    hub.rooms[0].doors[1].pos.x = 5;
    hub.rooms[0].doors[1].pos.y = 2;
    hub.rooms[0].doors[1].parentPos = hub.rooms[0].topLeft;
    hub.rooms[0].doors[1].target = &hub.rooms[1].doors[1];

    hub.rooms[1].doors[0].pos.x = 0;
    hub.rooms[1].doors[0].pos.y = 1;
    hub.rooms[1].doors[0].parentPos = hub.rooms[1].topLeft;
    hub.rooms[1].doors[0].target = &hub.rooms[0].doors[0];

    hub.rooms[1].doors[1].pos.x = 0;
    hub.rooms[1].doors[1].pos.y = 2;
    hub.rooms[1].doors[1].parentPos = hub.rooms[1].topLeft;
    hub.rooms[1].doors[1].target = &hub.rooms[0].doors[1];

    player dude;
    dude.pos = hub.rooms[0].topLeft;
    dude.pos.x += 1;
    dude.pos.y += 1;

    /*
        Screen displaying starts here
    */
    int exit = 0;
    char input;
    do {
        scanf(" %c", &input);

        switch(input) {
            default:
                break;
            case 'w':
                dude.pos.y--;
                break;
            case 's':
                dude.pos.y++;
                break;
            case 'a':
                dude.pos.x--;
                break;
            case 'd':
                dude.pos.x++;
                break;
            case 'q':
                exit = 1;
                break;
        }

        int m, n;
        int done = 0;
        vec2 newpos = dude.pos;
        for(m = 0; m < hub.roomCount; m++) {
            for(n = 0; n < hub.rooms[m].doorCount; n++) {
                vec2 pos = hub.rooms[m].doors[n].pos;
                pos.x += hub.rooms[m].topLeft.x;
                pos.y += hub.rooms[m].topLeft.y;

                if(dude.pos.x == pos.x && dude.pos.y == pos.y && done != 1) {
                    printf("doors %d %d\n", m, n);
                    newpos = hub.rooms[m].doors[n].target->pos;
                    newpos.x += hub.rooms[m].doors[n].target->parentPos.x;
                    newpos.y += hub.rooms[m].doors[n].target->parentPos.y;
                    done = 1;
                }
            }
        }

        done = 0;
        dude.pos = newpos;

        int x, y, i, j;
        for(y = 0; y < scrnY; y++) {
            for(x = 0; x < scrnX; x++) {
                screen[y][x] = '.';
            }
        }

        room *r;
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

        screen[dude.pos.y][dude.pos.x] = 'X';

        for(y = 0; y < scrnY; y++) {
            for(x = 0; x < scrnX; x++) {
                printf("%c", screen[y][x]);
            }
            printf("\n");
        }
    } while(!exit);


    free(hub.rooms);
    return 0;
}