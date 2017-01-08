#define true 1
#define false 0
#include <stdio.h>

typedef struct {
    float health, attack, armor;
} stat;

typedef struct {
    stat stats;
} player;

typedef struct {
    stat stats;
} enemy;

void fight(player *p, enemy *e, int initiative) {
    stat pstat, estat;
    pstat = p->stats;
    estat = e->stats;

    float initiativeBonus = 1.1;

    if(initiative) {
        pstat.attack *= initiativeBonus;
    } else {
        estat.attack *= initiativeBonus;
    }

    int turn = 0;
    int chargeTurns = 3;
    int chargeTurnsElapsed = 0;
    unsigned int plAction = 0;
    float bigHitMult = 100.0;

    printf("A new battle begins!\n");
    
    while(pstat.health > 0 && estat.health > 0) {
        //players turn
        if(plAction == 2 && chargeTurnsElapsed < chargeTurns) {
            printf("You spent your turn charging up the attack\n");
            chargeTurnsElapsed++;
        } else if(plAction == 2 && chargeTurnsElapsed >= chargeTurns) {
            float dmg = pstat.attack * bigHitMult / estat.armor;
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

            printf("You chose to ");
            switch(plAction) {
                case 1:
                printf("attack quickly\n");
                float dmg = pstat.attack / estat.armor;
                estat.health -= dmg;
                printf("You hit for %f damage\nThey have %f hp left\n", dmg, estat.health);
                break;

                case 2:
                printf("charge attack\n");
                chargeTurnsElapsed = 0;
                break;

                case 3:
                printf("block\n");
                break;
            }
        }
        //enemys turn
        if(estat.health > 0) {
            float dmg = estat.attack / pstat.armor;
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
    player pl;
    pl.stats.health = 100;
    pl.stats.attack = 10;
    pl.stats.armor = 1;

    enemy en;
    en.stats.health = 50;
    en.stats.attack = 15;
    en.stats.armor = 1.5;

    fight(&pl, &en, true);

    return 0;
}