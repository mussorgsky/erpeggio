#include <stdio.h>
#include <stdlib.h>

#define scrnX 30
#define scrnY 10

typedef struct vec2 {
    int x, y;
} vec2;

typedef struct door {
    vec2 pos, parentPos, targoo;
    struct door *target;
    int keylevel;
} door;

enum enemyType { dead, normal, boss };
char enemyLetter[] = { '$', 'E', 'B' };

typedef struct enemy {
    enum enemyType type;
    vec2 pos;
} enemy;

enum itemType { empty, key, healthpack };
char itemLetter[] = { '$', 'k', 'h' };

typedef struct item {
    enum itemType type;
    int level;
    int inInventory;
    vec2 pos;
} item;

typedef struct room {
    vec2 topLeft, dimensions;
    int doorCount;
    door *doors;
    int enemyCount;
    enemy *enemies;
    int itemCount;
    item *items;
} room;

typedef struct level {
    vec2 size;
    int roomCount;
    room *rooms;
} level;

typedef struct player {
    vec2 pos;
    int itemCount;
    item *items;
} player;

void initRoom(room *r, int tlX, int tlY, int w, int h) {
    r->topLeft.x = tlX;
    r->topLeft.y = tlY;
    r->dimensions.x = w;
    r->dimensions.y = h;
}

int checkKey(player *p, door *d) {
    item *it;
    for(int i = 0; i < p->itemCount; i++) {
        it = &p->items[i];
        if(it->type == key && it->level >= d->keylevel) {
            return 1;
        }
    }
    return 0;
}

void drawItems(room *rooms, int roomCount, char *screen) {
    room *r;
    item *it;
    for(int i = 0; i < roomCount; i++) {
        r = &rooms[i];
        for(int j = 0; j < r->itemCount; j++) {
            it = &r->items[j];
            if(it->type != empty && !it->inInventory) {
                vec2 pos;
                pos.x = r->topLeft.x + it->pos.x;
                pos.y = r->topLeft.y + it->pos.y;

                //if(testpos.x >= rPos.x && testpos.x < rSize.x && testpos.y >= rPos.y && testpos.y < rSize.y)
                if(pos.x >=0 && pos.x < scrnX && pos.y >= 0 && pos.y < scrnY) {
                    screen[pos.y * scrnX + pos.x] = itemLetter[it->type];
                } else {
                    printf("item %d in room %d out of bounds!\n", j, i);
                }
            }
        }
    }
}

void drawEnemies(room *rooms, int roomCount, char *screen) {
    room *r;
    enemy *en;
    for(int i = 0; i < roomCount; i++) {
        r = &rooms[i];
        for(int j = 0; j < r->enemyCount; j++) {
            en = &r->enemies[j];
            if(en->type != dead) {
                vec2 pos;
                pos.x = r->topLeft.x + en->pos.x;
                pos.y = r->topLeft.y + en->pos.y;

                //if(testpos.x >= rPos.x && testpos.x < rSize.x && testpos.y >= rPos.y && testpos.y < rSize.y)
                if(pos.x >=0 && pos.x < scrnX && pos.y >= 0 && pos.y < scrnY) {
                    screen[pos.y * scrnX + pos.x] = enemyLetter[en->type];
                } else {
                    printf("enemy %d in room %d out of bounds!\n", j, i);
                }
            }
        }
    }
}

void pickupItem(player *p, level *l) {
    vec2 pos = p->pos;
    room *r;
    for(int i = 0; i < l->roomCount; i++) {
        r = &l->rooms[i];
        vec2 ipos;
        for(int j = 0; j < r->itemCount; j++) {
            ipos = r->items[j].pos;
            ipos.x += r->topLeft.x;
            ipos.y += r->topLeft.y;
            if(ipos.x = pos.x && ipos.y == pos.y) {
                int done = 0;
                switch(r->items[j].type) {
                    default:
                        break;
                    case key:
                        for(int k = 0; k < p->itemCount && !done; k++) {
                            if(p->items[k].type == key) {
                                p->items[k].level = r->items[j].level;
                                r->items[j].type = empty;
                                printf("picked up level %d key\n", r->items[j].level);
                                done = 1;
                            }
                        }
                        break;
                    case healthpack:
                        break;
                }
            }
        }
    }
}

char screen[scrnY][scrnX];

const int levelDef[] = {
    /* level size */ scrnX, scrnY,
    /* rooms */ 3,
        /* room 1 */ 1, 1, 6, 4,
            /* doors in room 1 */ 2,
                /* door 1 */ 5, 1, 0, /* target [room], [door] */ 1, 0,
                /* door 2 */ 5, 2, 1, 1, 1,
            /* items in room 1 */ 0,
                /* no items in room 1 */
            /* enemies in room 1*/ 0,
                /* no enemies in room 1 */
        /* room 2 */ 10, 1, 6, 4,
            /* doors in room 2 */ 2,
                /* door 1 */ 0, 1, 0, 0, 0,
                /* door 2 */ 0, 2, 1, 0, 1,
            /* items in room 2 */ 1,
                /* item 1 */ 1, 10, 2, 2,
            /* enemies in room 2 */ 1,
                /* enemy 1 */ 1, 2, 1,
        /* room 3 */ 1, 5, 2, 2,
            /* doors in room 3 */ 0,
                /* no doors in room 3 */
            /* items */ 0,
                /* no items in room 3 */
            /* enemies */ 0,
                /* no enemies in room 3 */
    /* player position */ 1, 1
};

const int levelDefoo[] = {
    /* level size */ scrnX, scrnY,
    /* rooms */ 1,
        /* room 1 */ 2, 2, 5, 5,
            /* doors in room 1 */ 2,
                /* door 1 */ 0, 0, 1, /* target [room], [door] */ 0, 1,
                /* door 2 */ 4, 4, 1, 0, 0,
            /* items in room 1 */ 1,
                /* item 1*/ 1, 10, 2, 2,
            /* enemies in room 1*/ 1,
                /* enemy 1 */ 1, 3, 3,
    /* player position */ 1, 1
};

void initLevel(level *l, player *p, const int *def) {
    int seeker = 0;
    l->size.x = def[seeker++];
    l->size.y = def[seeker++];
    printf("level size set\n");

    l->roomCount = def[seeker++];
    l->rooms = malloc(l->roomCount * sizeof(room));
    printf("room array allocated\n");

    for(int i = 0; i < l->roomCount; i++) {
        int x, y, w,h;
        x = def[seeker++];
        y = def[seeker++];
        w = def[seeker++];
        h = def[seeker++];
        initRoom(&l->rooms[i], x, y, w, h);
        printf("room %d initialized\n", i);

        l->rooms[i].doorCount = def[seeker++];
        l->rooms[i].doors = malloc(l->rooms[i].doorCount * sizeof(door));
        printf("\tdoors initialized\n");
        
        for(int j = 0; j < l->rooms[i].doorCount; j++) {
            l->rooms[i].doors[j].pos.x = def[seeker++];
            l->rooms[i].doors[j].pos.y = def[seeker++];
            l->rooms[i].doors[j].keylevel = def[seeker++];
            l->rooms[i].doors[j].targoo.x = def[seeker++];
            l->rooms[i].doors[j].targoo.y = def[seeker++];
            l->rooms[i].doors[j].parentPos = l->rooms[i].topLeft;
            printf("\t\tdoor %d initialized\n", j);
        }

        l->rooms[i].itemCount = def[seeker++];
        l->rooms[i].items = malloc(l->rooms[i].itemCount * sizeof(item));
        printf("\titems initialized\n");

        for(int j = 0; j < l->rooms[i].itemCount; j++) {
            l->rooms[i].items[j].type = def[seeker++];
            l->rooms[i].items[j].level = def[seeker++];
            l->rooms[i].items[j].pos.x = def[seeker++];
            l->rooms[i].items[j].pos.y = def[seeker++];
            printf("\t\titem %d initialized\n", j);
        }

        l->rooms[i].enemyCount = def[seeker++];
        l->rooms[i].enemies = malloc(l->rooms[i].enemyCount * sizeof(enemy));
        printf("\tenemies initialized\n");

        for(int j = 0; j < l->rooms[i].enemyCount; j++) {
            l->rooms[i].enemies[j].type = def[seeker++];
            l->rooms[i].enemies[j].pos.x = def[seeker++];
            l->rooms[i].enemies[j].pos.y = def[seeker++];
            printf("\t\tenemy %d initialized\n", j);
        }
    }

    p->pos = l->rooms[0].topLeft;
    p->pos.x += def[seeker++];
    p->pos.y += def[seeker++];
    printf("player position set\n\n");

    for(int i = 0; i < l->roomCount; i++) {
        for(int j = 0; j < l->rooms[i].doorCount; j++) {
            vec2 target = l->rooms[i].doors[j].targoo;
            l->rooms[i].doors[j].target = &l->rooms[target.x].doors[target.y];
            printf("door %d %d target set\n", i, j);
        }
    }
}

int main() {
    level hub;
    player dude;
    initLevel(&hub, &dude, &levelDef[0]);

    dude.itemCount = 10;
    dude.items = malloc(dude.itemCount * sizeof(item));

    dude.items[0].type = key;
    dude.items[0].level = 0;
    dude.items[0].inInventory = 1;
    dude.items[0].pos.x = 0;
    dude.items[0].pos.y = 0;

    dude.items[1].type = healthpack;
    dude.items[1].level = 0;
    dude.items[1].inInventory = 1;
    dude.items[1].pos.x = 0;
    dude.items[1].pos.y = 0;

    for(int i = 2; i < dude.itemCount; i++) {
        item *it = &dude.items[i];
        it->type = empty;
        it->level = 0;
        it->inInventory = 1;
        it->pos.x = 0;
        it->pos.y = 0;
    }

    /*
        Screen displaying starts here
    */
    int exit = 0;
    char input[140];
    do {
        scanf(" %s", &input);
        vec2 walk;
        walk.x = 0;
        walk.y = 0;
        switch(input[0]) {
            default:
                break;
            case 'w':
                walk.y--;
                break;
            case 's':
                walk.y++;
                break;
            case 'a':
                walk.x--;
                break;
            case 'd':
                walk.x++;
                break;
            case 'e':
                pickupItem(&dude, &hub);
                break;
            case 'q':
                exit = 1;
                break;
        }

        vec2 testpos = dude.pos;
        testpos.x += walk.x;
        testpos.y += walk.y;
        if(testpos.x >= 0 && testpos.y >= 0 && testpos.x < scrnX && testpos.y < scrnY) {
            for(int i = 0; i < hub.roomCount; i++) {
                vec2 rPos = hub.rooms[i].topLeft;
                vec2 rSize = hub.rooms[i].dimensions;
                rSize.x += rPos.x;
                rSize.y += rPos.y;
                if(testpos.x >= rPos.x && testpos.x < rSize.x && testpos.y >= rPos.y && testpos.y < rSize.y) {
                    dude.pos.x += walk.x;
                    dude.pos.y += walk.y;
                    break;
                }
            }
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
                    if(checkKey(&dude, &hub.rooms[m].doors[n])) {
                        printf("doors %d %d\n", m, n);
                        newpos = hub.rooms[m].doors[n].target->pos;
                        newpos.x += hub.rooms[m].doors[n].target->parentPos.x;
                        newpos.y += hub.rooms[m].doors[n].target->parentPos.y;
                        done = 1;
                    } else {
                        printf("this door requires a level %d key\n", hub.rooms[m].doors[n].keylevel);
                    }
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

        drawItems(hub.rooms, hub.roomCount, &screen[0][0]);
        drawEnemies(hub.rooms, hub.roomCount, &screen[0][0]);

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