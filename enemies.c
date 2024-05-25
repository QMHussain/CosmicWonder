#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "math.h"

typedef struct{
    int x;
    int y;
}position;

typedef enum{
    GIGA_ROBOT,
    DRONE,
    ALIEN,
    ROBOT,
    TURRET,
    BLOB,
}Names;

typedef enum{
    ALL_IN,
    FLYING,
    RANGE,
}Behave;

typedef enum{
    RANGED,
    CC,
}Type;

typedef struct {
    char* name;
    int id;
    position coordinates;
    Type type;
    int range;
    int dmg;
    int weight;
}Weapon;

typedef struct {
    char* name;
    int id;
    position coordinates;
    int weight;
    int shield;
}Armor;

typedef struct {
    char* name;
    int id;
    position coordinates;
    int weight;
    int single_use;
    int stack;
    int max_stack;
    int augmentation;
    int what_augmentation;
    /*
    0 nothing
    1 life
    2 strength
    3 armor
    4 Weapon
    */
}Object;

typedef struct {
    Names name;
    position coordinates;
    int hp;
    int shield;
    int strength;
    int speed;
    int range;
    int orientation;
    Behave type;
    int XP;
}Enemy;

typedef struct {
    int roomdoor[4];
    int ndoor;
    int xmin;
    int xmax;
    int ymin;
    int ymax;
    int nb_enemy;
    Enemy** tab_enemy;
    int nb_object;
    Object* tab_object;
    int nb_armor;
    Armor* tab_armor;
    int nb_blade;
    Weapon* tab_blade;
}Room;

typedef struct{
    position coordinates;
    int room;
    int orientation; //o up 1 right 2 down 3 left
    int life;
    int max_life;
    int level;
    int XP;
    int score;
    int strength;
    int nbr_object_inventory;
    Object* inventory;
    Weapon blade;
    Armor stuff;
} hero;

int max(int a,int b){
    if(a>=b){
        return a;
    }
    return b;
}

int absolute_value(int a){
    if(a<0){
        return -a;
    }
    return a;
}

Enemy* defEnemy(Enemy* a){
    switch (a->name){
        case GIGA_ROBOT:
            a->type = ALL_IN;
            a->hp = 300;
            a->shield = 25;
            a->strength = 50;
            a->speed = 5;
            a->range = 3;
            return a;
        case DRONE:
            a->type = FLYING;
            a->hp = 1;
            a->shield = 1;
            a->strength = 5;
            a->speed = 30;
            a->range = 8;
            return a;
        case ALIEN:
            a->type = ALL_IN;
            a->hp = 80;
            a->shield = 10;
            a->strength = 15;
            a->speed = 20;
            a->range = 2;
            return a;
        case ROBOT:
            a->type = ALL_IN;
            a->hp = 10;
            a->shield = 5;
            a->strength = 25;
            a->speed = 15;
            a->range = 2;
            return a;
        case TURRET:
            a->type = RANGE;
            a->hp = 30;
            a->shield = 5;
            a->strength = 10;
            a->speed = 0;
            a->range = 8;
            return a;
        case BLOB:
            a->type = ALL_IN;
            a->hp = 20;
            a->shield = 1;
            a->strength = 10;
            a->speed = 10;
            a->range = 1;
            return a;
        default:
            exit(404);
    }
}

void enemySpawn(int number, Room* b){
    int x;
    b->tab_enemy = malloc(sizeof(Enemy*)*number);
    if (b->tab_enemy == NULL){
        exit(1);
    }
    for (int i=0;i<number;i++){
        Enemy* a = malloc(sizeof(Enemy)); // Allocate memory for each enemy individually
        if (a == NULL){
            exit(1);
        }
        x = rand()%6+1;
        switch (x){
            case 1:
                a->name = GIGA_ROBOT;
                break;
            case 2:
                a->name = DRONE;
                break;
            case 3:
                a->name = ALIEN;
                break;
            case 4:
                a->name = ROBOT;
                break;
            case 5:
                a->name = TURRET;
                break;
            case 6:
                a->name = BLOB;
                break;
            default:
                exit(5);
        }
        b->tab_enemy[i] = defEnemy(a);
        if (b->xmin < 0 && b->xmax > 0){
            b->tab_enemy[i]->coordinates.x = ((b->xmax - b->xmin) / 2 + b->xmin) + (rand() % 8 - 4);
            b->tab_enemy[i]->coordinates.y = ((b->ymin - b->ymax) / 2 + b->ymax) + (rand() % 8 - 4);
        }
        else if (b->ymin < 0 && b->ymax > 0){
            b->tab_enemy[i]->coordinates.y = ((b->ymax - b->ymin) / 2 + b->ymin) + (rand() % 8 - 4);
            b->tab_enemy[i]->coordinates.x = ((b->xmin - b->xmax) / 2 + b->xmax) + (rand() % 8 - 4);
        }
        else {
            b->tab_enemy[i]->coordinates.x = ((b->xmin - b->xmax) / 2 + b->xmax) + (rand() % 8 - 4);
            b->tab_enemy[i]->coordinates.y = ((b->ymin - b->ymax) / 2 + b->ymax) + (rand() % 8 - 4);
        }
    }
}

Weapon search_blade(int ID,FILE* fichier){
    int verif,test,temp;
    Weapon blade;
    rewind(fichier);
    char c=getc(fichier);
    while(c != EOF){
        if(c=='@'){
            verif=fscanf(fichier,"%d",&test);
            if(verif==EOF){
                printf("error load charging");
                exit(3);
            }
            if(ID==test){
                blade.id=ID;

                blade.name=calloc(21,sizeof(char));
                if(blade.name==NULL){
                    printf("error calloc");
                    exit(3);
                }
                verif=fscanf(fichier,"%s",blade.name);
                if(verif==EOF){
                    printf("error load charging");
                    exit(3);
                }

                verif=fscanf(fichier,"%d",&temp);
                if(verif==EOF){
                    printf("error load charging");
                    exit(3);
                }
                switch(temp){
                    case 1:
                        blade.type= RANGED;
                        break;

                    case 2:
                        blade.type= CC;
                        break;
                }

                verif=fscanf(fichier,"%d",&blade.range);
                if(verif==EOF){
                    printf("error load charging");
                    exit(3);
                }
                verif=fscanf(fichier,"%d",&blade.dmg);
                if(verif==EOF){
                    printf("error load charging");
                    exit(3);
                }
                verif=fscanf(fichier,"%d",&blade.weight);
                if(verif==EOF){
                    printf("error load charging");
                    exit(3);
                }
                return blade;
            }
        }
    }
    printf("formatting error blade.txt");
    exit(12);
}

Armor search_stuff(int ID,FILE* fichier){
    int verif,test;
    char* verif_name=NULL;
    Armor stuff;
    rewind(fichier);
    char c=getc(fichier);
    while(c != EOF){
        if(c=='@'){
            verif=fscanf(fichier,"%d",&test);
            if(verif==EOF){
                printf("error load charging");
                exit(3);
            }
            if(ID==test){
                stuff.id=ID;

                stuff.name=calloc(21,sizeof(char));
                if(stuff.name==NULL){
                    printf("error calloc");
                    exit(3);
                }
                verif=fscanf(fichier,"%s",stuff.name);
                if(verif==EOF){
                    printf("error load charging");
                    exit(3);
                }

                verif=fscanf(fichier,"%d",&stuff.weight);
                if(verif==EOF){
                    printf("error load charging");
                    exit(3);
                }
                verif=fscanf(fichier,"%d",&stuff.shield);
                if(verif==EOF){
                    printf("error load charging");
                    exit(3);
                }
                return stuff;
            }
        }
    }
    printf("formatting error stuff.txt");
    exit(12);
}

Object search_object(int ID,FILE* fichier){
    int verif,test;
    Object think;
    rewind(fichier);
    char c=getc(fichier);
    while(c != EOF){
        if(c=='@'){
            verif=fscanf(fichier,"%d",&test);
            if(verif==EOF){
                printf("error load charging");
                exit(3);
            }
            if(ID==test){
                think.id=ID;

                think.name=calloc(21,sizeof(char));
                if(think.name==NULL){
                    printf("error calloc");
                    exit(3);
                }
                verif=fscanf(fichier,"%s",think.name);
                if(verif==EOF){
                    printf("error load charging");
                    exit(3);
                }
                verif=fscanf(fichier,"%d",&think.weight);
                if(verif==EOF){
                    printf("error load charging");
                    exit(3);
                }
                verif=fscanf(fichier,"%d",&think.single_use);
                if(verif==EOF){
                    printf("error load charging");
                    exit(3);
                }
                verif=fscanf(fichier,"%d",&think.max_stack);
                if(verif==EOF){
                    printf("error load charging");
                    exit(3);
                }
                verif=fscanf(fichier,"%d",&think.augmentation);
                if(verif==EOF){
                    printf("error load charging");
                    exit(3);
                }
                verif=fscanf(fichier,"%d",&think.what_augmentation);
                if(verif==EOF){
                    printf("error load charging");
                    exit(3);
                }
                return think;
            }
        }
    }
    printf("formatting error Object.txt");
    exit(12);
}

void stuffSpawn(Room* a, FILE* Object, FILE* Blade, FILE* Armor){
    if (a->nb_object){
        for (int i=0; i<a->nb_object ; i++){
            a->tab_object[i] = search_object(rand()%15,Object);
            a->tab_object[i].coordinates.x = rand() % (a->xmax - a->xmin) + a->xmin;
            a->tab_object[i].coordinates.y = rand() % (a->ymax - a->ymin) + a->ymin;
        }
    }
    if (a->nb_blade){
        for (int i=0; i<a->nb_blade ; i++){
            a->tab_blade[i] = search_blade(rand()%15,Blade);
            a->tab_blade[i].coordinates.x = rand() % (a->xmax - a->xmin) + a->xmin;
            a->tab_blade[i].coordinates.y = rand() % (a->ymax - a->ymin) + a->ymin;
        }
    }
    if (a->nb_armor){
        for (int i=0; i<a->nb_armor ; i++){
            a->tab_armor[i] = search_stuff(rand()%15,Armor);
            a->tab_armor[i].coordinates.x = rand() % (a->xmax - a->xmin) + a->xmin;
            a->tab_armor[i].coordinates.y = rand() % (a->ymax - a->ymin) + a->ymin;
        }
    }
}



double calculateDistance(position pos1, position pos2) {
    int dx = pos2.x - pos1.x;
    int dy = pos2.y - pos1.y;
    return sqrt(dx*dx + dy*dy);
}

void printEnemy(Enemy** tab,int number){
    for(int i=0; i<number ;i++){
        switch (tab[i]->name) {
            case GIGA_ROBOT:
                printf("Enemy : GIGA_ROBOT\n");
                break;
            case DRONE:
                printf("Enemy : DRONE\n");
                break;
            case ALIEN:
                printf("Enemy : ALIEN\n");
                break;
            case ROBOT:
                printf("Enemy : ROBOT\n");
                break;
            case TURRET:
                printf("Enemy : TURRET\n");
                break;
            case BLOB:
                printf("Enemy : BLOB\n");
                break;
        }
        printf("coordinates: x=%d y=%d\n", tab[i]->coordinates.x, tab[i]->coordinates.y);
    }
}

int checkRange(Enemy* a, hero* player){
    double distance = calculateDistance(a->coordinates, player->coordinates);
    printf("Distance : %f\n", distance);
    switch (a->type) {
        case ALL_IN:
            if (distance >= a->range){
                return 1;
            }
            else {
                return 0;
            }
        case RANGE:
            if (distance >= a->range){
                return 1;
            }
            else {
                return 0;
            }
        case FLYING:
            if (distance != a->range){
                return 0;
            }
            else {
                return 1;
            }
    }
}

void mobOrientation(Enemy* a, hero* player){
    if (player->coordinates.x > a->coordinates.x && player->coordinates.y > a->coordinates.y){
        if (a->coordinates.x - player->coordinates.x == a->coordinates.y - player->coordinates.y){
            a->orientation = rand()%2;
        }
        else if(a->coordinates.x - player->coordinates.x > a->coordinates.y - player->coordinates.y){
            a->orientation = 1;
        } else{
            a->orientation = 0;
        }
    }
    else if (player->coordinates.x > a->coordinates.x && player->coordinates.y < a->coordinates.y){
        if (a->coordinates.x - player->coordinates.x == a->coordinates.y - player->coordinates.y){
            a->orientation = rand()%2+1;
        }
        else if(a->coordinates.x - player->coordinates.x > a->coordinates.y - player->coordinates.y){
            a->orientation = 1;
        } else{
            a->orientation = 2;
        }
    }
    else if (player->coordinates.x < a->coordinates.x && player->coordinates.y < a->coordinates.y){
        if (a->coordinates.x - player->coordinates.x == a->coordinates.y - player->coordinates.y){
            a->orientation = rand()%2+2;
        }
        else if(a->coordinates.x - player->coordinates.x > a->coordinates.y - player->coordinates.y){
            a->orientation = 2;
        } else{
            a->orientation = 3;
        }
    }
    else {
        if (a->coordinates.x - player->coordinates.x == a->coordinates.y - player->coordinates.y){
            if(rand()%2){
                a->orientation = 0;
            } else{
                a->orientation = 3;
            }
        }
        else if(a->coordinates.x - player->coordinates.x > a->coordinates.y - player->coordinates.y){
            a->orientation = 3;
        } else{
            a->orientation = 0;
        }
    }
}

void moveMob(Enemy* a, hero* player, Room* b){ //
    int x = 0;
    int y = 0;
    printf("hero position : %d %d\n", player->coordinates.x, player->coordinates.y);
    switch (a->type) {
        case ALL_IN:
            while (checkRange(a, player)) {
                if (player->coordinates.x + 1 > (a->coordinates.x + a->range)) {
                    if (a->coordinates.x < b->xmax){
                        a->coordinates.x += 1;
                    }
                } else if (player->coordinates.x + 1 < (a->coordinates.x - a->range)) {
                    if (a->coordinates.x > b->xmin){
                        a->coordinates.x -= 1;
                    }
                }
                if (player->coordinates.y + 1 > (a->coordinates.y + a->range)) {
                    if (a->coordinates.y < b->ymax){
                        a->coordinates.y += 1;
                    }
                } else if (player->coordinates.y + 1 < (a->coordinates.y - a->range)) {
                    if (a->coordinates.y > b->ymin){
                        a->coordinates.y -= 1;
                    }
                }
                printf("enemy position : %d %d\n", a->coordinates.x, a->coordinates.y);
            }
        case FLYING:
            while (checkRange(a, player)) {
                if ((a->coordinates.x + a->range) < player->coordinates.x + 1 && a->coordinates.x < b->xmax) {
                    a->coordinates.x += 1;
                } else if ((a->coordinates.x - a->range) > player->coordinates.x + 1 && a->coordinates.x > b->xmin) {
                    a->coordinates.x -= 1;
                }
                if ((a->coordinates.y + a->range) < player->coordinates.y + 1 && a->coordinates.y < b->ymax) {
                    a->coordinates.y += 1;
                } else if ((a->coordinates.y - a->range) > player->coordinates.y + 1 && a->coordinates.y > b->ymin) {
                    a->coordinates.y -= 1;
                }
                printf("enemy position : %d %d\n", a->coordinates.x, a->coordinates.y);
            }
        case RANGE:
            break;
    }
}

void killEnemy(Enemy* a, Room* b, hero* player){
    if (a->hp == 0){
        player->XP += a->XP;
        a = b->tab_enemy[b->nb_enemy];
        b->nb_enemy--;
    }
}

void playerHit(hero* player, Enemy* a, Room* b){
    double distance = calculateDistance(a->coordinates, player->coordinates);
    switch (player->orientation) {
        case 0:
            if (distance <= player->blade.range && a->coordinates.y >= player->coordinates.y){
                //attack animation (missing)
                a->hp -= player->strength + player->blade.dmg - a->shield;
            }
        case 1:
            if (distance <= player->blade.range && a->coordinates.x >= player->coordinates.x){
                //attack animation (missing)
                a->hp -= player->strength + player->blade.dmg - a->shield;
            }
        case 2:
            if (distance <= player->blade.range && a->coordinates.y <= player->coordinates.y){
                //attack animation (missing)
                a->hp -= player->strength + player->blade.dmg - a->shield;
            }
        case 3:
            if (distance <= player->blade.range && a->coordinates.x <= player->coordinates.y){
                //attack animation (missing)
                a->hp -= player->strength + player->blade.dmg - a->shield;
            }
    }
}

void mobHit(Enemy* a, hero* player){
    if (checkRange(a, player)){
        //attack animation (missing)
        player->life -= a->strength - player->stuff.shield;
    }
}



int main(){
    srand(367738);
    hero* player = NULL;
    player = malloc(sizeof(hero));
    if (player == NULL){
        exit (1);
    }
    Room *a = NULL;
    a = malloc(sizeof(Room));
    if (a == NULL){
        exit (1);
    }
    a->nb_enemy = 8;

    a->xmin = 5;
    a->xmax = 20;
    a->ymin = -20;
    a->ymax = -5;

    player->coordinates.x = 16;
    player->coordinates.y = -12;

    enemySpawn(8,a);
    printEnemy(a->tab_enemy,8);
    moveMob(a->tab_enemy[4], player, a);
    free(a);
    free(player);
    return 0;
}