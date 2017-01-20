#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define scrnX 36
#define scrnY 18

typedef struct vec2 {
    int x, y;
} vec2;

typedef struct door {
    vec2 pos, parentPos, pretarget;
    struct door *target;
    int keylevel;
} door;

typedef struct attack {
    int dmg, cost, failCost, chance;
} attack;

typedef struct stat {
    int health, maxHealth, maxStamina, stamina, stRegen;
} stat;

enum enemyType { dead, weak, normal, boss };
char enemyLetter[] = { '$', 'e', 'E', 'B' };

typedef struct enemy {
    enum enemyType type;
    vec2 pos;
    int level;
    stat stats;
} enemy;

enum itemType { empty, key };
char itemLetter[] = { '$', 'k' };

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
    int visited;
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
    int level;
    stat stats;
} player;

void initRoom(room *r, int tlX, int tlY, int w, int h) {
    r->topLeft.x = tlX;
    r->topLeft.y = tlY;
    r->dimensions.x = w;
    r->dimensions.y = h;
}

vec2 globalPosFromRoom(vec2 pos, room *r) {
    vec2 result;
    result.x = r->topLeft.x + pos.x;
    result.y = r->topLeft.y + pos.y;
    return result;
}

room* findRoomFromPos(vec2 pos, room *r, int roomCount) {
    for(int i = 0; i < roomCount; i++) {
        vec2 tl = r[i].topLeft;
        vec2 br = tl;
        br.x += r[i].dimensions.x;
        br.y += r[i].dimensions.y;
        if(pos.x >= tl.x && pos.x < br.x && pos.y >= tl.y & pos.y < br.y) {
            return &r[i];
        }
    }
    printf("cant find a room oh no\n");
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

void drawItems(room *rooms, int roomCount, char *screen, char *colors) {
    room *r;
    item *it;
    for(int i = 0; i < roomCount; i++) {
        r = &rooms[i];
        for(int j = 0; j < r->itemCount; j++) {
            it = &r->items[j];
            if(it->type != empty && !it->inInventory) {
                vec2 pos = globalPosFromRoom(it->pos, r);
                if(pos.x >=0 && pos.x < scrnX && pos.y >= 0 && pos.y < scrnY) {
                    screen[pos.y * scrnX + pos.x] = itemLetter[it->type];
                    colors[pos.y * scrnX + pos.x] = it->level;
                } else {
                    printf("item %d in room %d out of bounds!\n", j, i);
                }
            }
        }
    }
}

void drawEnemies(room *rooms, int roomCount, char *screen, char *colors) {
    room *r;
    enemy *en;
    for(int i = 0; i < roomCount; i++) {
        r = &rooms[i];
        for(int j = 0; j < r->enemyCount; j++) {
            en = &r->enemies[j];
            if(en->type != dead) {
                vec2 pos = globalPosFromRoom(en->pos, r);
                if(pos.x >=0 && pos.x < scrnX && pos.y >= 0 && pos.y < scrnY) {
                    screen[pos.y * scrnX + pos.x] = enemyLetter[en->type];
                    colors[pos.y * scrnX + pos.x] = en->level;
                } else {
                    printf("enemy %d in room %d out of bounds!\n", j, i);
                }
            }
        }
    }
}

void pickupItem(player *p, room *r) {
    vec2 pos = p->pos;
    
    for(int j = 0; j < r->itemCount; j++) {
        vec2 ipos = globalPosFromRoom(r->items[j].pos, r);
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
            }
        }
    }
}

void changeColor(int c) {
    switch(c) {
        default:
            printf("\x1b[0m");
            break;            
        case 1:
            printf("\x1b[0m");        
            printf("\x1b[1m");
            break;
        case 2:
            printf("\x1b[1m");
            printf("\x1b[31m");
            break;
        case 3:
            printf("\x1b[1m");
            printf("\x1b[32m");
            break;
        case 4:
            printf("\x1b[1m");
            printf("\x1b[33m");
            break;
        case 5:
            printf("\x1b[1m");
            printf("\x1b[34m");
            break;
        case 6:
            printf("\x1b[1m");
            printf("\x1b[35m");
            break;
        case 7:
            printf("\x1b[1m");
            printf("\x1b[36m");
            break;
    }
}

char screen[scrnY][scrnX];
char colors[scrnY][scrnX];

const int levelDef[] = {
    scrnX, scrnY,
    11,
        15, 1, 5, 4,
            3,
                0, 2, 1, 3, 0,
                4, 2, 0, 1, 0,
                2, 3, 4, 6, 0,
            0,
            0,
        21, 2, 5, 4,
            2,
                0, 1, 0, 0, 1,
                4, 1, 2, 2, 0,
            1,
                1, 1, 1, 1,
            1,
                1, 2, 1, 1,
        27, 2, 7, 4,
            1,
                0, 1, 2, 1, 1,
            1,
                1, 3, 5, 2,
            1,
                1, 5, 1, 1,
        9, 2, 5, 4,
            2,
                4, 1, 1, 0, 0,
                0, 1, 3, 4, 0,
            1,
                1, 2, 2, 2,
            1,
                1, 2, 1, 1,
        1, 2, 7, 4,
            2,
                6, 1, 3, 3, 1,
                1, 3, 6, 5, 0,
            1,
                1, 4, 1, 1,
            1,
                1, 3, 1, 1,
        1, 7, 7, 5,
            1,
                1, 0, 6, 4, 1,
            1,
                1, 7, 5, 3,
            2,
                2, 5, 1, 1,
                2, 3, 3, 1,
        15, 6, 5, 7,
            4,
                2, 0, 4, 0, 2,
                0, 3, 5, 8, 0,
                4, 3, 4, 7, 0,
                2, 6, 7, 9, 0,
            0,
            0,
        21, 7, 5, 5,
            1,
                0, 2, 4, 6, 2,
            1,
                1, 5, 3, 2,
            2,
                2, 2, 0, 1,
                2, 2, 4, 1,
        9, 7, 5, 5,
            1,
                4, 2, 5, 6, 1,
            1,
                1, 6, 1, 2,
            2,
                2, 2, 0, 1,
                2, 2, 4, 1,
        16, 14, 10, 3,
            2,
                1, 0, 7, 6, 3,
                9, 1, 7, 10, 0,
            0,
            2,
                1, 3, 1, 1,
                2, 7, 1, 1,
        27, 7, 7, 10,
            1,
                0, 8, 7, 9, 1,
            0,
            2,
                2, 3, 5, 1,
                3, 3, 2, 1,
    2, 1, 10
};

void initLevel(level *l, player *p, const int *def) {
    int seeker = 0;
    l->size.x = def[seeker++];
    l->size.y = def[seeker++];
    //printf("level size set\n");

    l->roomCount = def[seeker++];
    l->rooms = malloc(l->roomCount * sizeof(room));
    //printf("room array allocated\n");

    for(int i = 0; i < l->roomCount; i++) {
        int x, y, w,h;
        x = def[seeker++];
        y = def[seeker++];
        w = def[seeker++];
        h = def[seeker++];
        initRoom(&l->rooms[i], x, y, w, h);
        l->rooms[i].visited = 0;
        //printf("room %d initialized\n", i);

        l->rooms[i].doorCount = def[seeker++];
        l->rooms[i].doors = malloc(l->rooms[i].doorCount * sizeof(door));
        //printf("\tdoors initialized\n");
        
        for(int j = 0; j < l->rooms[i].doorCount; j++) {
            l->rooms[i].doors[j].pos.x = def[seeker++];
            l->rooms[i].doors[j].pos.y = def[seeker++];
            l->rooms[i].doors[j].keylevel = def[seeker++];
            l->rooms[i].doors[j].pretarget.x = def[seeker++];
            l->rooms[i].doors[j].pretarget.y = def[seeker++];
            l->rooms[i].doors[j].parentPos = l->rooms[i].topLeft;
            //printf("\t\tdoor %d initialized\n", j);
        }

        l->rooms[i].itemCount = def[seeker++];
        l->rooms[i].items = malloc(l->rooms[i].itemCount * sizeof(item));
        //printf("\titems initialized\n");

        for(int j = 0; j < l->rooms[i].itemCount; j++) {
            l->rooms[i].items[j].type = def[seeker++];
            l->rooms[i].items[j].level = def[seeker++];
            l->rooms[i].items[j].pos.x = def[seeker++];
            l->rooms[i].items[j].pos.y = def[seeker++];
            //printf("\t\titem %d initialized\n", j);
        }

        l->rooms[i].enemyCount = def[seeker++];
        l->rooms[i].enemies = malloc(l->rooms[i].enemyCount * sizeof(enemy));
        //printf("\tenemies initialized\n");

        for(int j = 0; j < l->rooms[i].enemyCount; j++) {
            l->rooms[i].enemies[j].type = def[seeker++];
            l->rooms[i].enemies[j].pos.x = def[seeker++];
            l->rooms[i].enemies[j].pos.y = def[seeker++];
            l->rooms[i].enemies[j].level = def[seeker++];

            if(l->rooms[i].enemies[j].type == weak) {
                l->rooms[i].enemies[j].stats.health = 30;
                l->rooms[i].enemies[j].stats.maxHealth = 30;
                l->rooms[i].enemies[j].stats.maxStamina = 20;
                l->rooms[i].enemies[j].stats.stamina = 20;
                l->rooms[i].enemies[j].stats.stRegen = 1;
            } else if(l->rooms[i].enemies[j].type == normal) {
                l->rooms[i].enemies[j].stats.health = 70;
                l->rooms[i].enemies[j].stats.maxHealth = 70;
                l->rooms[i].enemies[j].stats.maxStamina = 60;
                l->rooms[i].enemies[j].stats.stamina = 60;
                l->rooms[i].enemies[j].stats.stRegen = 4;
            }else {
                l->rooms[i].enemies[j].stats.health = 120;
                l->rooms[i].enemies[j].stats.maxHealth = 120;
                l->rooms[i].enemies[j].stats.maxStamina = 120;
                l->rooms[i].enemies[j].stats.stamina = 120;
                l->rooms[i].enemies[j].stats.stRegen = 12;
            }
            //printf("\t\tenemy %d initialized\n", j);
        }
    }

    p->pos = l->rooms[0].topLeft;
    p->pos.x += def[seeker++];
    p->pos.y += def[seeker++];
    //printf("player position set\n\n");
    p->level = def[seeker++];

    for(int i = 0; i < l->roomCount; i++) {
        for(int j = 0; j < l->rooms[i].doorCount; j++) {
            vec2 target = l->rooms[i].doors[j].pretarget;
            l->rooms[i].doors[j].target = &l->rooms[target.x].doors[target.y];
            //printf("door %d %d target set\n", i, j);
        }
    }
}

void cleanScanf() {
    char c;
    while((c = getchar()) != '\n' && c != EOF) { }
}

#define RAND_MAX 100

int main() {
    int turns = 0;

    time_t t;
    srand((unsigned) time(&t));
    level hub;
    player dude;
    initLevel(&hub, &dude, &levelDef[0]);

    attack atWeak;
    atWeak.dmg = 5;
    atWeak.cost = 6;
    atWeak.failCost = 5;
    atWeak.chance = 10;

    attack atNormal;
    atNormal.dmg = 10;
    atNormal.cost = 10;
    atNormal.failCost = 5;
    atNormal.chance = 25;

    attack atHard;
    atHard.dmg = 30;
    atHard.cost = 25;
    atHard.failCost = 15;
    atHard.chance = 80;

    dude.itemCount = 10;
    dude.items = malloc(dude.itemCount * sizeof(item));

    dude.items[0].type = key;
    dude.items[0].level = 0;
    dude.items[0].inInventory = 1;
    dude.items[0].pos.x = 0;
    dude.items[0].pos.y = 0;

    dude.stats.health = 100;
    dude.stats.maxHealth = 100;
    dude.stats.maxStamina = 100;
    dude.stats.stamina = 100;
    dude.stats.stRegen = 10;

    for(int i = 1; i < dude.itemCount; i++) {
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
    int fight = 0;
    char input;

    enemy *fighter;

    printf("\n\n\n\n\n\n\n\n\n\n\nWelcome to the game\n\n");
    printf("You are the X and your mission is to kill the Big Bad Boss (B)\n");
    printf("You have to gather keys (k) to go to different rooms through doors (O)\n");
    printf("On your way you will fight different enemies (w, e, E)\n");
    printf("\nThe controls are:\n\tmovement\tw s a d\n\tpick up key\te\n\tshow key level\tk\n\tshow turn\tt\n\tquit game\tq\n");
    printf("\nIn fight mode:\n\tattack\t\tw n h\n");
    printf("\nAll commands must be confirmed with the return key\n");
    printf("\n\tinput any command to continue\n");

    do {
        turns++;
        if(!fight) {
            scanf(" %c", &input);
            if(turns == 1) {
               input = 'x';
            }
            cleanScanf();
            vec2 walk;
            walk.x = 0;
            walk.y = 0;
            switch(input) {
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
                    pickupItem(&dude, findRoomFromPos(dude.pos, &hub.rooms[0], hub.roomCount));
                    break;
                case 'q':
                    exit = 1;
                    break;
                case 'k':
                    changeColor(dude.items[0].level);
                    printf("you have a level %d key\n", dude.items[0].level);
                    changeColor(0);
                    break;
                case 't':
                    printf("you are currently on turn %d\n", turns);
                    break;
            }

            vec2 testpos = dude.pos;
            testpos.x += walk.x;
            testpos.y += walk.y;
            //printf("testing rooms for wall collisions\n");
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
            //printf("\tdone\n");

            int m, n;
            int done = 0;
            vec2 newpos = dude.pos;
            //printf("testing rooms for doors\n");
            for(m = 0; m < hub.roomCount; m++) {
                for(n = 0; n < hub.rooms[m].doorCount; n++) {
                    vec2 pos = hub.rooms[m].doors[n].pos;
                    pos.x += hub.rooms[m].topLeft.x;
                    pos.y += hub.rooms[m].topLeft.y;

                    if(dude.pos.x == pos.x && dude.pos.y == pos.y && done != 1) {
                        if(checkKey(&dude, &hub.rooms[m].doors[n])) {
                            //printf("doors %d %d\n", m, n);
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
            //printf("\tdone\n");

            done = 0;
            dude.pos = newpos;
            findRoomFromPos(dude.pos, &hub.rooms[0], hub.roomCount)->visited = 1;

            //printf("updating enemies\n");
            // update enemies in the room occupied by the player
            room *enroom = findRoomFromPos(dude.pos, hub.rooms, hub.roomCount);
            //room *enroom = &hub.rooms[2];
            for(int i = 0; i < enroom->enemyCount; i++) {
                enemy *en = &enroom->enemies[i];
                if(en->type != dead) {
                    //int chasing = en->level >= dude.level;
                    int chasing = 1;
                    vec2 globPos = globalPosFromRoom(en->pos, enroom);
                    vec2 newEnPos = en->pos;
                    if(chasing) {
                        if(globPos.x < dude.pos.x) {
                            newEnPos.x++;
                        } else if(globPos.x > dude.pos.x) {
                            newEnPos.x--;
                        } else if(globPos.y < dude.pos.y) {
                            newEnPos.y++;
                        } else if(globPos.y > dude.pos.y) {
                            newEnPos.y--;
                        }

                        vec2 dims = enroom->dimensions;
                        if(newEnPos.x >= 0 && newEnPos.x < dims.x && newEnPos.y >= 0 && newEnPos.y < dims.y) {
                            en->pos = newEnPos;
                            globPos = globalPosFromRoom(en->pos, enroom);
                        }
                    }
                    if(globPos.x == dude.pos.x && globPos.y == dude.pos.y) {
                        fighter = en;
                        fight = 1;
                    }
                }
            }
            //printf("\tdone\n");

            int x, y, i, j;
            for(y = 0; y < scrnY; y++) {
                for(x = 0; x < scrnX; x++) {
                    colors[y][x] = 0;
                    screen[y][x] = '.';
                }
            }

            room *r;
            for(i = 0; i < hub.roomCount; i++) {
                r = &hub.rooms[i];
                if(r->visited) {
                    for(y = r->topLeft.y; y < r->topLeft.y + r->dimensions.y; y++) {
                        for(x = r->topLeft.x; x < r->topLeft.x + r->dimensions.x; x++) {
                            colors[y][x] = 0;
                            screen[y][x] = ' ';
                        }
                    }

                    for(j = 0; j < r->doorCount; j++) {
                        screen[r->topLeft.y + r->doors[j].pos.y][r->topLeft.x + r->doors[j].pos.x] = 'O';
                        colors[r->topLeft.y + r->doors[j].pos.y][r->topLeft.x + r->doors[j].pos.x] = r->doors[j].keylevel;
                    }
                }
            }

            drawItems(hub.rooms, hub.roomCount, &screen[0][0], &colors[0][0]);
            drawEnemies(hub.rooms, hub.roomCount, &screen[0][0], &colors[0][0]);

            screen[dude.pos.y][dude.pos.x] = 'X';
            int lastColor = 0;
            for(y = 0; y < scrnY; y++) {
                for(x = 0; x < scrnX; x++) {
                    if(colors[y][x] != lastColor) {
                        lastColor = colors[y][x];
                        changeColor(lastColor);
                    }
                    printf("%c", screen[y][x]);
                }
                printf("\n");
            }
        } else {
            char ctrl;
            attack *at;
            printf("a fight is starting, you have 3 attacks to choose from:\n(w)eak, (n)ormal, (h)ard\neach has a different stamina cost, damage and chance of success\nchoose wisely\n");
            while(dude.stats.health > 0 && fighter->stats.health > 0) {
                //players turn
                turns++;
                printf("it\'s your turn\n");
                printf("hp: %d\t stam: %d", dude.stats.health, dude.stats.stamina);
                printf("\t\t\thp: %d\t stam: %d\n", fighter->stats.health, fighter->stats.stamina);
                scanf(" %c", &ctrl);
                cleanScanf();
                switch(ctrl) {
                    default:
                        at = &atNormal;
                        break;
                    case 'w':
                        at = &atWeak;
                        break;
                    case 'n':
                        at = &atNormal;
                        break;
                    case 'h':
                        at = &atHard;
                        break;
                    case 'q':
                        at = &atNormal;
                        dude.stats.health = 0;
                        break;
                }
                if(dude.stats.stamina >= at->cost) {
                    int chance = rand() % 100;
                    if(chance >= at->chance) {
                        dude.stats.stamina -= at->cost;
                        if(chance >= 95) {
                            printf("CRITICAL HIT\n");
                            fighter->stats.health -= at->dmg * 3;
                        } else {
                            printf("your attack success\n");
                            fighter->stats.health -= at->dmg;
                        }
                    } else {
                        dude.stats.stamina -= at->failCost;
                        printf("your attack failed\n");
                    }
                } else {
                    printf("you not enough stamina\n");
                }
                dude.stats.stamina += dude.stats.stRegen;
                if(dude.stats.stamina > dude.stats.maxStamina) {
                    dude.stats.stamina = dude.stats.maxStamina;
                }
                
                //enemies turn
                switch(fighter->type) {
                    default:
                        at = &atNormal;
                        break;
                    case weak:
                        at = &atWeak;
                        break;
                    case normal:
                        at = &atNormal;
                        break;
                    case boss:
                        at = &atHard;
                        break;
                }
                if(fighter->stats.stamina >= at->cost) {
                    int chance = rand() % 100;
                    if(chance >= at->chance) {
                        fighter->stats.stamina -= at->cost;
                        if(chance >= 95) {
                            printf("CRITICAL HIT\n");
                            dude.stats.health -= at->dmg * 3;
                        } else {
                            printf("enemys attack success\n");
                            dude.stats.health -= at->dmg;
                        }
                    } else {
                        fighter->stats.stamina -= at->failCost;
                        printf("enemys attack failed\n");
                    }
                } else {
                    printf("enemy not enough stamina\n");
                }
                fighter->stats.stamina += fighter->stats.stRegen;
                if(fighter->stats.stamina > fighter->stats.maxStamina) {
                    fighter->stats.stamina = fighter->stats.maxStamina;
                }
            }
            printf("hp: %d\t stam: %d", dude.stats.health, dude.stats.stamina);
            printf("\t\t\thp: %d\t stam: %d\n", fighter->stats.health, fighter->stats.stamina);
            if(dude.stats.health > 0) {
                if(fighter->type == boss) {
                    printf("~~~~CONGRATULATIONS!~~~~\n~~~~YOU WON THE GAME~~~~\nit took you %d turns\n", turns);
                    exit = 1;
                } else {
                    printf("you won the battle\n");
                    fighter->type = dead;
                    dude.stats.health = dude.stats.maxHealth;
                    dude.stats.stamina = dude.stats.maxStamina;
                }
            } else {
                printf("you lost\n");
                exit = 1;
            }
            fight = 0;
        }
    } while(!exit);

    return 0;
}