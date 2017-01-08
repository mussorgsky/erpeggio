#define true 1
#define false 0
#include <stdio.h>

typedef struct {
    float health, armor;
} stat;

typedef struct {
    float capacity, regen, current;
} stamina;

typedef struct {
    float cost, dmg;
} attack;

typedef struct {
    stat stats;
    stamina stam;
    attack atFast, atHard;
} player;

typedef struct {
    stat stats;
    stamina stam;
    attack atFast;
} enemy;

void fight(player *p, enemy *e, int initiative) {
    stat pstat, estat;
    stamina pstam, estam;
    pstat = p->stats;
    estat = e->stats;

    float initiativeBonus = 1.0;

    if(initiative) {
        //pstat.attack *= initiativeBonus;
    } else {
        //estat.attack *= initiativeBonus;
    }

    int turn = 0;
    int chargeTurns = 3;
    int chargeTurnsElapsed = 0;
    unsigned int plAction = 0;

    printf("A new battle begins!\n");
    
    while(pstat.health > 0 && estat.health > 0) {
        //players turn
        if(plAction == 2 && chargeTurnsElapsed < chargeTurns) {

            printf("You spent your turn charging up the attack\n");
            chargeTurnsElapsed++;

        } else if(plAction == 2 && chargeTurnsElapsed >= chargeTurns) {

            float dmg = p->atHard.dmg / estat.armor;
            estat.health -= dmg;
            if(estat.health < 0) estat.health = 0;
            printf("You charged up your attack\nYou hit for %f damage\nThey have %f hp left\n", dmg, estat.health);
            if(estat.health <= 0) {
                printf("YOU KILLED THAT SONOFABITCH\n");
            }
            plAction = 0;

        } else {

            printf("It\'s your turn, what do you do?\n(1) Quick attack\t(2) Charge attack\t(3) Block\n");
            scanf("%d", &plAction);

            if(plAction > 3) plAction = 3;
            if(plAction < 1) plAction = 1;

            p->stam.current = (p->stam.current + p->stam.regen >= p->stam.capacity) ? p->stam.capacity : p->stam.current + p->stam.regen;

            printf("You chose to ");
            switch(plAction) {
                case 1:
                printf("attack quickly\n");
                float dmg = p->atFast.dmg / estat.armor;
                estat.health -= dmg;
                p->stam.current -= p->atFast.cost;
                printf("You hit for %f damage\nThey have %f hp left\n", dmg, estat.health);
                break;

                case 2:
                printf("charge attack\n");
                chargeTurnsElapsed = 0;
                p->stam.current -= p->atHard.cost;
                break;

                case 3:
                printf("block\n");
                break;
            }

        }
        //enemys turn
        if(estat.health > 0) {
            
            e->stam.current = (e->stam.current + e->stam.regen >= e->stam.capacity) ? e->stam.capacity : e->stam.current + e->stam.regen;
            float dmg = e->atFast.dmg / pstat.armor;
            pstat.health -= dmg;
            printf("\tThe enemy hit you for %f damage\n\tYou have %f hp left\n", dmg, pstat.health);

        }
    }

    if(pstat.health > 0) {
        printf("\nYOU WIN\n\n");
    } else {
        printf("\nYOU LOSE\n\n");
    }

    p->stats = pstat;
    e->stats = estat;
}

int main() {
    attack fast;
    fast.cost = 15;
    fast.dmg = 10;

    attack hard;
    hard.cost = 25;
    hard.dmg = 30;

    player pl;
    pl.stats.health = 100;
    pl.stats.armor = 1;
    pl.stam.capacity = 100;
    pl.stam.regen = 10;
    pl.stam.current = pl.stam.capacity;
    pl.atFast = fast;
    pl.atHard = hard;

    enemy en;
    en.stats.health = 50;
    en.stats.armor = 1.5;
    en.stam.capacity = 100;
    en.stam.regen = 10;
    en.stam.current = en.stam.capacity;
    en.atFast = fast;

    fight(&pl, &en, true);

    return 0;
}