#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ncurses.h>
#include <locale.h>

//define define new color
#define COLOR_PINK 8
#define COLOR_BROWN 9
#define COLOR_BEIGE 10
#define COLOR_BEIGEclair 11

#define MAXROOM 100

typedef struct{
    int x;
    int y;
} position;
    
typedef struct {
    char* name;
    int id;
    position coos;
    int weight;
    int single_use;
    int stack;
    int max_stack;
    int upgrade;
    int wupgrade;
    /*
    0 nothing 
    1 life 
    2 strength
    3 armor
    4 Weapon
    */
}Object;

//for Weapon
typedef enum{
    RANGED,
    CC,
    MANA,
}Type;
//
typedef struct {
    char* name;
    int id;
    position coos;
    Type type;
    int range;
    int dmg;
    int weight;
}Weapon;

typedef struct {
    char* name;
    int id;
    position coos;
    int weight;
    int shield;
}Armor;

//for ennemy
typedef enum{
    GIGA_ROBOT,
    DRONE,
    ALIEN,
    ROBOT,
    GOLLUM,
    TURRET,
    BLOB,
}Names;

typedef enum{
    ALL_IN,
    FLYING,
    PRECIOUS,
    RANGE,
}Behave;
//
typedef struct {
    Names name;
    position coos;
    int life;
    int armor;
    int strength;
    int speed;
    Behave type;
    Weapon weapon;
    int XP;
}Enemy;

typedef struct {
  int x;
  int y;
  int roomn1;
  int roomn2;
  int direction;
} door;

typedef struct {
  int roomdoor[4];
  int ndoor;
  int xmin;
  int xmax;
  int ymin;
  int ymax;
  int nb_enemy;
  Enemy* tab_enemy;
  int nb_object;
  Object* tab_object;
  int nb_armor;
  Armor* tab_armor;
  int nb_blade;
  Weapon* tab_blade;
}room;

typedef struct{
    position coos;
    int room;
    int orientation; //o up 1 right 2 down 3 left
    int life;
    int max_life;
    int lvl;
    int XP;
    int strength;
    Object* inv;
    Weapon blade;
    Armor stuff;
} hero;

int min(int a,int b){
    if(a<=b){
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

hero build_hero(){
    hero player;
    player.coordinates.x=0;
    player.coordinates.y=0;
    player.room=0;
    player.orientation=0;
    player.life=100;
    player.max_life=100;
    player.level=0;
    player.XP=0;
    player.strength=1;
    player.inventory=calloc(15, sizeof(Object));
    if(player.inventory == NULL){
        printf("error calloc inventory");
        exit(2);
        }
    return player;
    }
    
void display_game(int screen_length,int screen_width,hero player,WINDOW* background, time_t timer,int time_pause,int time_jouer,int time_to_play);  

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//fonction hero
void use_object(hero* player,int position_stuff){
    //effect
    switch (player->inventory[position_stuff].what_augmentation){
        case 1:
            //life
            player->strength+=player->inventory[position_stuff].augmentation;
            if(player->life>player->max_life){
                player->life=player->max_life;
                }
            break;
        case 2:
            //strength
            player->strength+=player->inventory[position_stuff].augmentation;
            break;
        case 3:
            //armor
            player->stuff.shield+=player->inventory[position_stuff].augmentation;
            break;
        case 4:
            //armor
            player->blade.dmg+=player->inventory[position_stuff].augmentation;
            break;
        }

    if (player->inventory[position_stuff].single_use){
        player->inventory[position_stuff].max_stack--;
        }
    if(player->inventory[position_stuff].max_stack == 0){
        //delete stuff
        player->inventory[position_stuff].id=0;
        }
    }

void collect_object(hero* player,room* a){
    room* tab=a+player->room;
    
    for(int i=0;i<tab->nb_object; i++){
        if(player->coordinates.x == tab->tab_object[i].coordinates.x && player->coordinates.y == tab->tab_object[i].coordinates.y){
            for(int j=0;j<15;j++){
                if(player->inventory[j].id == 0){
                    player->inventory[j]=tab->tab_object[i];
                    tab->tab_object[i]=tab->tab_object[tab->nb_object];
                    tab->tab_object[tab->nb_object].id=0;
                    tab->nb_object--;
                    }
                }
            }
        }
    }

void collect_armor(hero* player,room* a,int screen_length, int screen_width,WINDOW* background,time_t timer,int time_pause,int time_jouer,int time_to_play){
    room* tab=a+player->room;
    
    for(int i=0;i<tab->nb_armor; i++){
        if(player->coordinates.x == tab->tab_armor[i].coordinates.x && player->coordinates.y == tab->tab_armor[i].coordinates.y){
            int test1=getch();
            do{
                //refresh the timer
                //display game
                display_game(screen_length,screen_width,*player,background,timer,time_pause,time_jouer,time_to_play);

                //verfication
                attron(COLOR_PAIR(8));
                mvprintw(7,4,"are you sure you want to change your stuff?");
                mvprintw(8,4,"y/n");

                //name objet use
                mvprintw(9,6,"name: %s",player->stuff.name);
                mvprintw(11,6,"shield: %d",player->stuff.shield);
                mvprintw(13,6,"weight: %d",player->stuff.weight);
                
                //name objet
                mvprintw(9,20,"name:%s",tab->tab_armor[i].name);
                mvprintw(11,20,"shield: %d",tab->tab_armor[i].shield);
                mvprintw(13,20,"weight: %d",tab->tab_armor[i].weight);
                refresh();

                test1=getch();
                //yes
                if(test1==121){
                    player->stuff=tab->tab_armor[i];
                    tab->tab_armor[i]=tab->tab_armor[tab->nb_armor - 1];
                    tab->nb_armor--;
                    }
                //no
                else if(test1==110){
                    tab->tab_armor[i]=tab->tab_armor[tab->nb_armor - 1];
                    tab->nb_armor--;
                    }
            }while(test1!=121 && test1!=110);
            attron(COLOR_PAIR(3));
            }
        }
    }

void collect_blade(hero* player,room* a,int screen_length,int screen_width,WINDOW* background,time_t timer,int time_pause,int time_jouer,int time_to_play){
    room* tab=a+player->room;
    
    for(int i=0;i<tab->nb_blade; i++){
        if(player->coordinates.x == tab->tab_blade[i].coordinates.x && player->coordinates.y == tab->tab_blade[i].coordinates.y){
            int test1=getch();
            do{
                //refresh the timer
                //display game
                display_game(screen_length,screen_width,*player,background,timer,time_pause,time_jouer,time_to_play);

                //verfication
                attron(COLOR_PAIR(8));
                mvprintw(7,4,"are you sure you want to change your blade?");
                mvprintw(8,4,"y/n");
                refresh();

                //name objet use
                mvprintw(9,6,"name: %s",player->blade.name);
                mvprintw(11,6,"range: %d",player->blade.range);
                mvprintw(13,6,"damage: %d",player->blade.dmg);
                mvprintw(15,6,"weight: %d",player->blade.weight);
                
                //name objet
                mvprintw(9,20,"name: %s",tab->tab_blade[i].name);
                mvprintw(11,20,"range: %d",tab->tab_blade[i].range);
                mvprintw(13,20,"damage: %d",tab->tab_blade[i].dmg);
                mvprintw(15,20,"weight: %d",tab->tab_blade[i].weight);


                test1=getch();
                //yes
                if(test1==121){
                    player->blade=tab->tab_blade[i];
                    tab->tab_blade[i]=tab->tab_blade[tab->nb_blade - 1];
                    tab->nb_blade--;
                    }
                //no
                else if(test1==110){
                    tab->tab_blade[i]=tab->tab_blade[tab->nb_blade - 1];
                    tab->nb_blade--;
                    }
            }while(test1!=121 && test1!=110);
            attron(COLOR_PAIR(3));
            }
        }
    }

void you_hit(hero* player,room* a){
    room* tab=a+player->room;
    for(int i=0;i<tab->nb_enemy; i++){
        //search is as enemy in the front of the player
        switch(player->orientation){
            //up
            case 0:
                if(player->coordinates.y < tab->tab_enemy[i].coordinates.y && player->coordinates.y + player->blade.range > tab->tab_enemy[i].coordinates.y && player->coordinates.x == tab->tab_enemy[i].coordinates.x){
                    tab->tab_enemy[i].life-=player->strength+player->blade.dmg;
                    }
                break;
            //right
            case 1:
                if(player->coordinates.x < tab->tab_enemy[i].coordinates.x && player->coordinates.x + player->blade.range > tab->tab_enemy[i].coordinates.x && player->coordinates.y == tab->tab_enemy[i].coordinates.y){
                    tab->tab_enemy[i].life-=player->strength+player->blade.dmg;
                    }
                break;
            //down
            case 2:
                if(player->coordinates.y > tab->tab_enemy[i].coordinates.y && player->coordinates.y + player->blade.range < tab->tab_enemy[i].coordinates.y && player->coordinates.x == tab->tab_enemy[i].coordinates.x){
                    tab->tab_enemy[i].life-=player->strength+player->blade.dmg;
                    }
                break;
            //left
            case 3:
                if(player->coordinates.x > tab->tab_enemy[i].coordinates.x && player->coordinates.x + player->blade.range < tab->tab_enemy[i].coordinates.x && player->coordinates.y == tab->tab_enemy[i].coordinates.y){
                    tab->tab_enemy[i].life-=player->strength+player->blade.dmg;
                    }
                break;
            }
            
        //Enemy dead
        if(tab->tab_enemy[i].life==0){
            //win xp
            player->XP+=tab->tab_enemy[i].XP;
            //sup Enemy
            tab->tab_enemy[i]=tab->tab_enemy[tab->nb_enemy];
            tab->nb_enemy--;
            }
        }
    }

void be_hit(hero* player,room* a){
    room* tab=a+player->room;

    for(int i=0;i<tab->nb_enemy; i++){
        if(player->coordinates.y == tab->tab_enemy[i].coordinates.y && (player->coordinates.x - tab->tab_enemy[i].coordinates.x <= tab->tab_enemy[i].weapon.range || player->coordinates.x - tab->tab_enemy[i].coordinates.x <= -tab->tab_enemy[i].weapon.range)){
            int total_damage=tab->tab_enemy[i].strength+tab->tab_enemy[i].weapon.dmg-player->stuff.shield;
            if(total_damage>0){
                player->life-=total_damage;
                }
            }
        else if(player->coordinates.x == tab->tab_enemy[i].coordinates.x && (player->coordinates.y - tab->tab_enemy[i].coordinates.y <= tab->tab_enemy[i].weapon.range || player->coordinates.y - tab->tab_enemy[i].coordinates.y <= -tab->tab_enemy[i].weapon.range)){
            int total_damage=tab->tab_enemy[i].strength+tab->tab_enemy[i].weapon.dmg-player->stuff.shield;
            if(total_damage>0){
                player->life-=total_damage;
                }
            }
        }
    }

void win_level(hero* player){
    while(player->XP >= 1000){
        player->XP -= 1000;
        player->level++;
        
        //buff for XP
         player->strength++;
         player->max_life+=2;
         player->life+=2;       
        }
    }
    
void death(hero* player){
    player->coordinates.x=0;
    player->coordinates.y=0;
    player->life=100;
    }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//room fonction
room createmainroom() {
  room froom;
  for (int j = 0; j < 4; j++) {
    froom.roomdoor[j] = 1;
  }
  froom.ndoor = 4;
  froom.xmin = 1 * -4;
  froom.xmax = 1 * 5;
  froom.ymin = 1 * -4;
  froom.ymax = 1 * 5;
  // froom.stratumnum = 0;

  return froom;
}
  
void createmaindoor(door *tabdoor) {
  tabdoor[0].x = 0;
  tabdoor[0].y = 6;
  tabdoor[0].roomn1 = 0;
  tabdoor[0].roomn2 = -1;
  tabdoor[0].direction = 0;
  tabdoor[1].x = 6;
  tabdoor[1].y = 0;
  tabdoor[1].roomn1 = 0;
  tabdoor[1].roomn2 = -1;
  tabdoor[1].direction = 1;
  tabdoor[2].x = 0;
  tabdoor[2].y = -5;
  tabdoor[2].roomn1 = 0;
  tabdoor[2].roomn2 = -1;
  tabdoor[2].direction = 2;
  tabdoor[3].x = -5;
  tabdoor[3].y = 0;
  tabdoor[3].roomn1 = 0;
  tabdoor[3].roomn2 = -1;
  tabdoor[3].direction = 3;
}

void defin0(room tabroom[], int maxroom) {
  for (int i = 0; i < maxroom; i++) {
    for (int j = 0; j < 12; j++) {

    }
    tabroom[i].ndoor = 0;
    tabroom[i].xmin = 0;
    tabroom[i].xmax = 0;
    tabroom[i].ymin = 0;
    tabroom[i].ymax = 0;
  }
}

int searchneardoor(door* tabdoor,door testdoor, int doorcount){
  for(int i=0;i<doorcount;i++){
    switch (testdoor.direction){
      case 0:
          if(tabdoor[i].x>=(testdoor.x)-8 && tabdoor[i].x<=(testdoor.x)+8 && tabdoor[i].y>=(testdoor.y)+1 && tabdoor[i].y<=(testdoor.y)+13){
            return 1;
          }
          break;
      case 1:
          if(tabdoor[i].y>=(testdoor.y)-8 && tabdoor[i].y<=(testdoor.y)+8 && tabdoor[i].x>=(testdoor.x)+1 && tabdoor[i].x<=(testdoor.x)+13){
            return 1;
          }
          break;
      case 2:
          if(tabdoor[i].x>=(testdoor.x)-8 && tabdoor[i].x<=(testdoor.x)+8 && tabdoor[i].y<=(testdoor.y)-1 && tabdoor[i].y>=(testdoor.y)-13){
            return 1;
          }
          break;
      case 3:
          if(tabdoor[i].y>=(testdoor.y)-8 && tabdoor[i].y<=(testdoor.y)+8 && tabdoor[i].x<=(testdoor.x)-1 && tabdoor[i].x>=(testdoor.x)-13){
            return 1;
          }
          break;
    }
  }
  return 0;
}

int searchreallyneardoor(door possibledoor, door testdoor){
  switch (testdoor.direction){
    case 0:
        if(possibledoor.x>=(testdoor.x)-4 && possibledoor.x<=(testdoor.x)+4 && possibledoor.y>=(testdoor.y)+1 && possibledoor.y<=(testdoor.y)+7){
          return 1;
        }
    case 1:
        if(possibledoor.y>=(testdoor.y)-4 && possibledoor.y<=(testdoor.y)+4 && possibledoor.x>=(testdoor.x)+1 && possibledoor.x<=(testdoor.x)+7){
          return 1;
        }
    case 2:
        if(possibledoor.x>=(testdoor.x)-4 && possibledoor.x<=(testdoor.x)+4 && possibledoor.y<=(testdoor.y)-1 && possibledoor.y>=(testdoor.y)-7){
          return 1;
        }
    case 3:
        if(possibledoor.y>=(testdoor.y)-4 && possibledoor.y<=(testdoor.y)+4 && possibledoor.x<=(testdoor.x)-1 && possibledoor.x>=(testdoor.x)-7){
          return 1;
        }
  }
  return 0;
}

int confirmoverlap(room *roomsizetab, room roomsize, int roomcount) {
  for (int i=0; i<roomcount; i++) {
    if ((roomsize.ymin >= roomsizetab[i].ymin &&roomsize.ymin <= roomsizetab[i].ymax) ||(roomsize.xmin >= roomsizetab[i].xmin &&roomsize.xmin <= roomsizetab[i].xmax) || (roomsize.ymax >= roomsizetab[i].ymin && roomsize.ymax <= roomsizetab[i].ymax) || (roomsize.xmax >= roomsizetab[i].xmin && roomsize.xmax <= roomsizetab[i].xmax)) {
      return 0; //overlap
    }
  }
  return 1; //no overlap
}

int confirmdoorloc(door* tabdoor, door testdoor, int doorcount){
  for(int i=0;i<doorcount;i++){
    if(testdoor.x <= tabdoor[i].x+4 && testdoor.x >=tabdoor[i].x-4 && testdoor.y <= tabdoor[i].y+4 && testdoor.y >=tabdoor[i].y-4){
      return 0; //not allowed
    }
  }
  return 1; //allowed
}

int adaptnextsizeroomfuse(room *newroom, int roomcount, door *tabdoor, door testdoor, int doorcount) {
  int countdoor;
  for (int i = 0; i <= doorcount; i++) {
    if (tabdoor[i].direction != testdoor.direction && searchreallyneardoor(tabdoor[i], testdoor)>0) {
      switch (testdoor.direction) {
        case 0:
            switch (tabdoor[i].direction) {
              case 1:
                newroom->xmin = tabdoor[i].x + 1;
                tabdoor[i].roomn2 = roomcount;
                countdoor++;
                newroom->roomdoor[((tabdoor[i].direction)+2)%4]=1;
                break;
              case 2:
                newroom->ymax = tabdoor[i].y - 1;
                tabdoor[i].roomn2 = roomcount;
                countdoor++;
                newroom->roomdoor[((tabdoor[i].direction)+2)%4]=1;
                break;
              case 3:
                newroom->xmax = tabdoor[i].x - 1;
                tabdoor[i].roomn2 = roomcount;
                countdoor++;
                newroom->roomdoor[((tabdoor[i].direction)+2)%4]=1;
                break;
            }
            break;
        case 1:
            switch(tabdoor[i].direction) {
              case 0:
                newroom->ymin= tabdoor[i].y+1;
                tabdoor[i].roomn2 = roomcount;
                countdoor++;
                newroom->roomdoor[((tabdoor[i].direction)+2)%4]=1;
                break;
              case 2:
                newroom->ymax = tabdoor[i].y - 1;
                tabdoor[i].roomn2 = roomcount;
                countdoor++;
                newroom->roomdoor[((tabdoor[i].direction)+2)%4]=1;
                break;
              case 3:
                newroom->xmax = tabdoor[i].x - 1;
                tabdoor[i].roomn2 = roomcount;
                countdoor++;
                newroom->roomdoor[((tabdoor[i].direction)+2)%4]=1;
                break;
            }
            break;
        case 2:
            switch(tabdoor[i].direction) {
              case 0:
                newroom->ymin= tabdoor[i].y+1;
                tabdoor[i].roomn2 = roomcount;
                countdoor++;
                newroom->roomdoor[((tabdoor[i].direction)+2)%4]=1;
                break;
              case 1:
                newroom->xmin = tabdoor[i].x + 1;
                tabdoor[i].roomn2 = roomcount;
                countdoor++;
                newroom->roomdoor[((tabdoor[i].direction)+2)%4]=1;
                break;
              case 3:
                newroom->xmax = tabdoor[i].x - 1;
                tabdoor[i].roomn2 = roomcount;
                countdoor++;
                newroom->roomdoor[((tabdoor[i].direction)+2)%4]=1;
                break;
            }
            break;
        case 3:
            switch(tabdoor[i].direction) {
              case 0:
                newroom->ymin= tabdoor[i].y+1;
                tabdoor[i].roomn2 = roomcount;
                countdoor++;
                newroom->roomdoor[((tabdoor[i].direction)+2)%4]=1;
                break;
              case 1:
                newroom->xmin = tabdoor[i].x + 1;
                tabdoor[i].roomn2 = roomcount;
                countdoor++;
                newroom->roomdoor[((tabdoor[i].direction)+2)%4]=1;
                break;
              case 2:
                newroom->ymax = tabdoor[i].y - 1;
                tabdoor[i].roomn2 = roomcount;
                countdoor++;
                newroom->roomdoor[((tabdoor[i].direction)+2)%4]=1;
                break;
            }
            break;
      }
    }
  }
  return countdoor;
}

room generateroom(int seed, int *maxroom,room *tabroom, door* prevdoor, int *roomcount, door *tabdoor, int *doorcount) {
  room newroom;
  int doorpull, randtamp, doornum=*doorcount, directionindex;
  prevdoor->roomn2=*roomcount;
  for(int i=0;i<4;i++){
    newroom.roomdoor[i] =-1;
  }
  newroom.roomdoor[(prevdoor->direction+2)%4]=1;
  newroom.ndoor=1;
  
  for(int i=0; i<*doorcount; i++){
    if(searchreallyneardoor(tabdoor[i], *prevdoor)>0){
      newroom.ndoor++;
    }
  }
  if (newroom.ndoor > 1){
    newroom.xmax=0;
    newroom.xmin=0;
    newroom.ymax=0;
    newroom.ymin=0;
    adaptnextsizeroomfuse(&newroom, *roomcount, tabdoor, *prevdoor, *doorcount);
    do{
      switch (prevdoor->direction){
        case 0:
          newroom.ymin=prevdoor->y+1;
          if(newroom.xmin==0){
            if(rand()%2==0){
              newroom.xmin=prevdoor->x+2;
            }
            else{
              newroom.xmin=prevdoor->x-2;
            }
          }
          if(newroom.xmax==0){
            newroom.xmax=newroom.xmin+4+(rand()%5);
          }
          if(newroom.ymax==0){
            newroom.ymax=newroom.ymin+4+(rand()%5);
          }
          break;
        case 1:
          newroom.xmin=prevdoor->x+1;
          if(newroom.xmax==0){
            newroom.xmax=newroom.xmin+4+(rand()%5);
          }
          if(newroom.ymin==0){
            if(rand()%2==0){
              newroom.ymin=prevdoor->y+2;
            }
            else{
              newroom.ymin=prevdoor->y-2;
            }
          }
          if(newroom.ymax==0){
            newroom.ymax=newroom.ymin+4+(rand()%5);
          }
          break;
        case 2:
          newroom.ymax=prevdoor->y-1;
          if(newroom.xmin==0){
            if(rand()%2==0){
              newroom.xmin=prevdoor->x+2;
            }
            else{
              newroom.xmin=prevdoor->x-2;
            }
          }
          if(newroom.xmax==0){
            newroom.xmax=newroom.xmin+4+(rand()%5);
          }
          if(newroom.ymin==0){
            newroom.ymin=newroom.ymax-4-(rand()%5);
          }
          break;
        case 3:
          newroom.xmax=prevdoor->x-1;
          if(newroom.xmin==0){
            newroom.xmin=newroom.xmax-4-(rand()%5);
          }
          if(newroom.ymin==0){
            if(rand()%2==0){
              newroom.ymin=prevdoor->y+2;
            }
            else{
              newroom.ymin=prevdoor->y-2;
            }
          }
          if(newroom.ymax==0){
            newroom.ymax=newroom.ymin+4+(rand()%5);
          }
          break;
      } 
          
    }while(confirmoverlap(tabroom, newroom, *roomcount)==0);
  } //connecting rooms
  else if(searchneardoor(tabdoor, *prevdoor, *doorcount)>0){
    newroom.ndoor= rand() % 3 + 1;
    doorpull= newroom.ndoor;
    directionindex=0;
    do{
      randtamp=((prevdoor->direction+2)%4)+1+rand()%3;
      if(newroom.roomdoor[randtamp]!=1){
        newroom.roomdoor[randtamp]=1;
        doorpull--;
      }
    }while(doorpull>0);
    do{
      switch (prevdoor->direction){
        case 0:
          if(rand()%2==0){
            newroom.xmin=prevdoor->x+1;
          }
          else{
            newroom.xmin=prevdoor->x-1;
          }
          newroom.xmax=newroom.xmin+3+(rand()%4);
          newroom.ymin=prevdoor->y+1;
          newroom.ymax=newroom.ymin+3+(rand()%4);
          break;
        case 1:
          newroom.xmin=prevdoor->x+1;
          newroom.xmax=newroom.xmin+3+(rand()%4);
          if(rand()%2==0){
            newroom.ymin=prevdoor->y+1;
          }
          else{
            newroom.ymin=prevdoor->y-1;
          }
          newroom.ymax=newroom.ymin+3+(rand()%4);
          break;
        case 2:
          if(rand()%2==0){
            newroom.xmin=prevdoor->x+1;
          }
          else{
            newroom.xmin=prevdoor->x-1;
          }
          newroom.xmin=newroom.xmax+3+(rand()%4);
          newroom.ymax=prevdoor->y-1;
          newroom.ymin=newroom.ymax-3-(rand()%4);
          break;
        case 3:
          newroom.xmax=prevdoor->x-1;
          newroom.xmin=newroom.xmax-3-(rand()%4);
          if(rand()%2==0){
            newroom.ymin=prevdoor->y+1;
          }
          else{
            newroom.ymin=prevdoor->y-1;
          }
          newroom.ymax=newroom.ymin+3+(rand()%4);
          break;
      }
    }while(confirmoverlap(tabroom, newroom, *roomcount)==0);
    for(int i=doornum; i<doornum+newroom.ndoor; i++){
      tabdoor[i].roomn1=*roomcount;
      tabdoor[i].roomn2=-1;
      tabdoor[i].x=0;
      tabdoor[i].y=6;
      do{
        directionindex++;
      }while(newroom.roomdoor[directionindex]!=1 && directionindex==((prevdoor->direction)+2)%4);
      tabdoor[i].direction=directionindex;
      do{
        switch(tabdoor[i].direction){
        case 0:
          tabdoor[i].x=newroom.xmin+(rand()%(newroom.xmax-newroom.xmin+1));
          tabdoor[i].y=newroom.ymax+1;
          break;
        case 1:
          tabdoor[i].x=newroom.xmax+1;
          tabdoor[i].y=newroom.ymin+(rand()%(newroom.ymax-newroom.ymin+1));
          break;
        case 2:
          tabdoor[i].x=newroom.xmin+(rand()%(newroom.xmax-newroom.xmin+1));
          tabdoor[i].y=newroom.ymin-1;
          break;
        case 3:
          tabdoor[i].x=newroom.xmin+1;
          tabdoor[i].y=newroom.ymin+(rand()%(newroom.ymax-newroom.ymin+1));
          break;
        }
      }while(confirmdoorloc(tabdoor, tabdoor[i], *doorcount)==0);
      (*doorcount)++;
    }
  }//near rooms
  else{
    newroom.ndoor= rand() % 3 + 2;
    doorpull= newroom.ndoor;
    do{
      randtamp=((prevdoor->direction+2)%4)+1+rand()%3;
      if(newroom.roomdoor[randtamp]!=1){
        newroom.roomdoor[randtamp]=1;
        doorpull--;
      }
    }while(doorpull>0);
    switch (prevdoor->direction){
      case 0:
        if(rand()%2==0){
          newroom.xmin=prevdoor->x+2;
        }
        else{
          newroom.xmin=prevdoor->x-2;
        }
        newroom.xmax=newroom.xmin+4+(rand()%5);
        newroom.ymin=prevdoor->y+1;
        newroom.ymax=newroom.ymin+4+(rand()%5);
        break;
      case 1:
        newroom.xmin=prevdoor->x+1;
        newroom.xmax=newroom.xmin+4+(rand()%5);
        if(rand()%2==0){
          newroom.ymin=prevdoor->y+2;
        }
        else{
          newroom.ymin=prevdoor->y-2;
        }
        newroom.ymax=newroom.ymin+4+(rand()%5);
        break;
      case 2:
        if(rand()%2==0){
          newroom.xmin=prevdoor->x+2;
        }
        else{
          newroom.xmin=prevdoor->x-2;
        }
        newroom.xmin=newroom.xmax+4+(rand()%5);
        newroom.ymax=prevdoor->y-1;
        newroom.ymin=newroom.ymax-4-(rand()%5);
        break;
      case 3:
        newroom.xmax=prevdoor->x-1;
        newroom.xmin=newroom.xmax-4-(rand()%5);
        if(rand()%2==0){
          newroom.ymin=prevdoor->y+2;
        }
        else{
          newroom.ymin=prevdoor->y-2;
        }
        newroom.ymax=newroom.ymin+4+(rand()%5);
        break;
    }while(confirmoverlap(tabroom, newroom, *roomcount)==0);
    for(int i=doornum; i<doornum+newroom.ndoor; i++){
      tabdoor[i].roomn1=*roomcount;
      tabdoor[i].roomn2=-1;
      tabdoor[i].x=0;
      tabdoor[i].y=6;
      do{
        directionindex++;
      }while(newroom.roomdoor[directionindex]!=1 && directionindex==((prevdoor->direction)+2)%4);
      tabdoor[i].direction=directionindex;
      do{
        switch(tabdoor[i].direction){
        case 0:
          tabdoor[i].x=newroom.xmin+(rand()%(newroom.xmax-newroom.xmin+1));
          tabdoor[i].y=newroom.ymax+1;
          break;
        case 1:
          tabdoor[i].x=newroom.xmax+1;
          tabdoor[i].y=newroom.ymin+(rand()%(newroom.ymax-newroom.ymin+1));
          break;
        case 2:
          tabdoor[i].x=newroom.xmin+(rand()%(newroom.xmax-newroom.xmin+1));
          tabdoor[i].y=newroom.ymin-1;
          break;
        case 3:
          tabdoor[i].x=newroom.xmin+1;
          tabdoor[i].y=newroom.ymin+(rand()%(newroom.ymax-newroom.ymin+1));
          break;
        }
      }while(confirmdoorloc(tabdoor, tabdoor[i], *doorcount)==0);
      (*doorcount)++;
    }
  }// rooms
  (*roomcount)++;
  return newroom;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//display fonction
void display_menu(int screen_length,int screen_width,int menu_cursor,WINDOW* background){
    clear();
    refresh();
    background=newwin(screen_width,screen_length,0,0);
    wbkgd(background,COLOR_PAIR(1));  
    wrefresh(background);

    attron(COLOR_PAIR(1));
    mvprintw(2,screen_length/2-6,"   in menu   ");

    mvprintw(4,screen_length/2-6,"    start    ");
    mvprintw(6,screen_length/2-6,"continue game");
    mvprintw(8,screen_length/2-6,"   setting   ");
    mvprintw(screen_width-2,3,"exit");

    attron(COLOR_PAIR(2));
    switch(menu_cursor){
        case 0:
            mvprintw(4,screen_length/2-6,"    start    ");
            break;
        case 1:
            mvprintw(6,screen_length/2-6,"continue game");
            break;
        case 2:
            mvprintw(8,screen_length/2-6,"   setting   ");
            break;
        case 3:
            mvprintw(screen_width-2,3,"exit");
            break;
        }
    attron(COLOR_PAIR(1));
    move(screen_width+10,screen_length+10);
    refresh();
    }

void display_name_menu(int screen_length,int screen_width,WINDOW* background,char* name){
    refresh();
    background=newwin(screen_width-4,screen_length,4,0);
    wbkgd(background,COLOR_PAIR(1));  
    wrefresh(background);

    mvprintw(4,screen_length/2-11,"enter your initials");
    move(6,screen_length/2-11);
    for(int i=0;i<3;i++){
        if (name[i]==0){
            printw("_");
            }
        else{
            printw("%c",name[i]);
            }
        printw(" ");
        }
    attron(COLOR_PAIR(1));
    move(screen_width+10,screen_length+10);
    refresh();
    }

int search_save(char* name,FILE* fichier){
	rewind(fichier);
    char c=getc(fichier);
	while(c != EOF){
		if(c=='@'){
            int name_pareil=1;
            for(int i=0;i<3;i++){
                c=getc(fichier);
                if(c != name[i]){
                    i=3;
                    name_pareil=0;
                    }
                }
            if(name_pareil){
                return 1;
                }
            }
		c=getc(fichier);
		}
    return 0;
    }

void display_seed_menu(int screen_length,int screen_width,WINDOW* background,int seed,int count){
    mvprintw(10+2*count,10,"%d",seed);
    refresh();
    }

void display_name_load_menu(int screen_length,int screen_width,WINDOW* background,char* name,int load_menu_cursor){
    display_name_menu(screen_length,screen_width,background,name);

    //browse the different backups and display their names
    /*
    
    */
    }

void display_game(int screen_length,int screen_width,hero player,WINDOW* background, time_t timer,int time_pause,int time_jouer,int time_to_play){
    clear();
    refresh();
    background=newwin(screen_width,screen_length,0,0);
    wbkgd(background,COLOR_PAIR(3));  
    wrefresh(background);

    time_t time_now=time(NULL);
    int time_total=time_now-timer-time_pause+time_jouer;

    attron(COLOR_PAIR(3));
    mvprintw(2,1,"Life: %03d   ",player.life);
    mvprintw(4,1,"Time: %02d:%02d ",time_to_play-(time_total/60)-1,59-(time_total%60));

    if(screen_length>25){
        //life 
        attron(COLOR_PAIR(4));
        move(2,13);
        for(int i=0;i<screen_length-14 && i<50;i++){
            if(i>(player.life/(player.max_life*1.0) * min(screen_length-14,50))){
                attron(COLOR_PAIR(6));
                }
            printw(" ");
            }

        //time
        if(time_total+2>time_to_play*60){
            attron(COLOR_PAIR(7));
            mvprintw(4,7,"00:00");
            }

        attron(COLOR_PAIR(5));
        move(4,13);

        for(int i=0;i<screen_length-14 && i<50;i++){
            if(i>(1-time_total/(time_to_play*60.0))*min(screen_length-14,50)-1){
                attron(COLOR_PAIR(6));
                }
            printw(" ");
            }
        }
    attron(COLOR_PAIR(3));
    move(screen_width+10,screen_length+10);
    refresh();
    }

void display_donjon(int screen_length,int screen_width,hero player,int doorcount,door* tabdoor,int roomcount,room* tabroom, WINDOW* donjon){
    //refresh();
    donjon=newwin(screen_width-8,screen_length-6,6,3);
    wbkgd(donjon,COLOR_PAIR(6));
    wrefresh(donjon);
    
    //display hero
    attron(COLOR_PAIR(10));
    mvprintw(6+(screen_width-8)/2,3+(screen_length-6)/2,"&");
    
    //display room
    for(int i=0;i<roomcount;i++){
        //display room background
        for(int j=0;j<absolute_value(tabroom[i].ymax-tabroom[i].ymin);j++){
            for(int k=0;k<absolute_value(tabroom[i].xmax-tabroom[i].xmin);k++){
                int local_x= player.coordinates.x - tabroom[i].xmin + k;
                int local_y= player.coordinates.y - tabroom[i].ymin + j;
                if(absolute_value(local_x) <= (screen_length-6)/2 && absolute_value(local_y) <= (screen_width-8)/2){
                    attron(10);
                    mvprintw(local_y,local_x," ");
                    }
                }
            }
        //display enemy
        for(int l=0;l<tabroom[i].nb_enemy; l++){
            int local_x= player.coordinates.x - tabroom[i].tab_enemy[l].coordinates.x;
            int local_y= player.coordinates.y - tabroom[i].tab_enemy[l].coordinates.y;
            if(absolute_value(local_x) <= (screen_length-6)/2 && absolute_value(local_y) <= (screen_width-8)/2){
                attron(11);
                mvprintw(local_y,local_x," ");
                }
            }
        //display objetc
        for(int l=0;l<tabroom[i].nb_object; l++){
            int local_x= player.coordinates.x - tabroom[i].tab_object[l].coordinates.x;
            int local_y= player.coordinates.y - tabroom[i].tab_object[l].coordinates.y;
            if(absolute_value(local_x) <= (screen_length-6)/2 && absolute_value(local_y) <= (screen_width-8)/2){
                attron(12);
                mvprintw(local_y,local_x," ");
                }
            }
        //display armor
        for(int l=0;l<tabroom[i].nb_armor; l++){
            int local_x= player.coordinates.x - tabroom[i].tab_armor[l].coordinates.x;
            int local_y= player.coordinates.y - tabroom[i].tab_armor[l].coordinates.y;
            if(absolute_value(local_x) <= (screen_length-6)/2 && absolute_value(local_y) <= (screen_width-8)/2){
                attron(13);
                mvprintw(local_y,local_x," ");
                }
            }
        //display blade
        for(int l=0;l<tabroom[i].nb_blade; l++){
            int local_x= player.coordinates.x - tabroom[i].tab_blade[l].coordinates.x;
            int local_y= player.coordinates.y - tabroom[i].tab_blade[l].coordinates.y;
            if(absolute_value(local_x) <= (screen_length-6)/2 && absolute_value(local_y) <= (screen_width-8)/2){
                attron(14);
                mvprintw(local_y,local_x," ");
                }
            }
        }
    refresh();
    }

void display_inventory_menu(int screen_length,int screen_width,hero player,WINDOW* donjon,int inventory_cursor){
    //window start in 6 3
    donjon=newwin(screen_width-8,screen_length-6,6,3);
    wbkgd(donjon,COLOR_PAIR(8));
    wrefresh(donjon);

    attron(COLOR_PAIR(8));
    mvprintw(7,4,"inventory");
    for (int i=0;i<3;i++){
        for(int j=0;j<5;j++){

            for(int k=0;k<7;k++){
                if(inventory_cursor==j+5*i){
                    attron(COLOR_PAIR(9));
                    }
                else if(player.inventory[j + 5 * i].id == 0){
                    attron(COLOR_PAIR(6));
                    }
                else{
                    attron(COLOR_PAIR(3));
                    }
                //x=start_window+2+nbr_object_raw*lenght_object
                //y=start_window+2+nbr_object_col*width_object
                mvprintw(9+k+(i*9),6+(22*j),"                    ");

                attron(COLOR_PAIR(8));
                printw("  ");
                }

            if(player.inventory[j + 5 * i].id != 0){
                if(inventory_cursor==j+5*i){
                    attron(COLOR_PAIR(9));
                    }
                else{
                    attron(COLOR_PAIR(3));
                    }
                //name objet
                mvprintw(9+0+(i*9),6+(22*j),"%s",player.inventory[j + 5 * i].name);
                mvprintw(9+2+(i*9),6+(22*j), "%d / %d", player.inventory[j + 5 * i].stack, player.inventory[j + 5 * i].max_stack);
                switch (player.inventory[j + 5 * i].what_augmentation){
                    case 0:
                        mvprintw(9+4+(i*9),6+(22*j),"no buff");
                        break;
                    case 1:
                        mvprintw(9+4+(i*9),6+(22*j),"+%d life",player.inventory[j + 5 * i].augmentation);
                        break;
                    case 2:
                        mvprintw(9+4+(i*9),6+(22*j),"+%d strength",player.inventory[j + 5 * i].augmentation);
                        break;
                    case 3:
                        mvprintw(9+4+(i*9),6+(22*j),"+%d armor",player.inventory[j + 5 * i].augmentation);
                        break;
                    }
                }
            }
        }
//select object
    attron(COLOR_PAIR(9));
    refresh();
    }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(void){
    initscr();
    noecho();
    halfdelay(10);
    //cbreak();
    //nodelay(stdscr,True);
    //timeout(delay);

    //setlocale(LC_CTYPE, "");
    
    keypad(stdscr, TRUE);
    
    //create size screen
    int screen_length=130; //130
    int screen_width=40;  //40
    if (screen_length<25 || screen_width<25){
        printf("error screen size");
        exit(1);
        }
    
    if(has_colors()==FALSE && can_change_color()==FALSE){
        printf("terminal doesn't support colors");
        exit(1);
        }

    WINDOW *background,*donjon;
    FILE* fichier=NULL;
    fichier=fopen("save.txt","r+");
    if(fichier==NULL){
        printf("error txt doesn't exist");
        exit(1);
    }
    int verif=1;
 
    //init loop to play
    int in_menu=1;
    int in_game=0;
    int in_pause=0;
    int in_inventory=0;

    //create color
    start_color();
    //init_color(COLOR_BlaCK, 700, 1000,1000);
    init_color(COLOR_RED, 700, 0, 0);
    init_color(COLOR_BLUE, 200, 200, 750);
    init_color(COLOR_GREEN, 200, 500, 200);
    init_color(COLOR_YELLOW, 750, 750, 50);

    init_color(COLOR_PINK, 1000, 375, 500);
    init_color(COLOR_BROWN, 300, 200, 0);
    init_color(COLOR_BEIGE,250, 200, 100);
    init_color(COLOR_BEIGEclair,300, 200, 100);
    
    //background menu
    init_pair(1,COLOR_WHITE,COLOR_BLUE);
    //select menu
    init_pair(2,COLOR_WHITE,COLOR_RED);
    //background game and baground item
    init_pair(3,COLOR_WHITE,COLOR_BEIGE);
    //life
    init_pair(4,COLOR_WHITE,COLOR_GREEN);
    //time
    init_pair(5,COLOR_WHITE,COLOR_YELLOW);
    //fond
    init_pair(6,COLOR_WHITE,COLOR_BLACK);
    //time out
    init_pair(7,COLOR_RED,COLOR_BEIGE);
    //inventory background
    init_pair(8,COLOR_WHITE,COLOR_BEIGEclair);
    //inventory select
    init_pair(9,COLOR_WHITE,COLOR_RED);

//change color
    //room background
    init_pair(10,COLOR_WHITE,COLOR_BEIGEclair);
    //ennemy
    init_pair(11,COLOR_WHITE,COLOR_RED);
    //object
    init_pair(12,COLOR_WHITE,COLOR_GREEN);
    //armor
    init_pair(13,COLOR_WHITE,COLOR_GREEN);
    //blade
    init_pair(14,COLOR_WHITE,COLOR_GREEN);

    //menu
    int menu_cursor=0;
    
    //create player
    hero player;
    int inventory_cursor=0;
    //init name
    char name[3];
    int seed;
    
    //key player
    int key_exit=27;
    int key_enter=10;
    int key_inventory='0';
    int key_delete='*';
    int key_sup_text=263;
    int moove_up=KEY_UP;
    int moove_down=KEY_DOWN;
    int moove_left=KEY_LEFT;
    int moove_right=KEY_RIGHT;
    int key_fight='a';
    // ...
    
    //init room
    int maxroom= MAXROOM;
    int roomcount= 0;
    int doorcount=0;
    room *tabroom;
    door *tabdoor;
    tabdoor = malloc(sizeof(door) * MAXROOM * 4);
    if (tabdoor == NULL) {
        exit(1);
        }
    tabroom = malloc(MAXROOM * sizeof(room));
    if (tabroom == NULL) {
        exit(1);
        }
    
    //init timer
    time_t timer=0;
    int time_pause=0;
    int time_jouer=0;
    time_t time_start_pause;
    int time_to_play=1;
    
    while(in_menu){
        //display menu
        display_menu(screen_length,screen_width,menu_cursor,background);
        
        int test1=getch();
        //choose menu button
        //press enter
        if(test1==key_enter){
            int name_complet=0;
            int Exit=0;

            //init name
            name[0]=0;
            name[1]=0;
            name[2]=0;

            int avancement=0;
            int load_menu_cursor=0;
            //start the game
            switch(menu_cursor){
                case 0:
                    name_complet=0;
                    Exit=0;
                    avancement=0;
                    
                    while(!name_complet && !Exit){
                        display_name_menu(screen_length,screen_width,background,name);
                        
                        test1=getch();
                        if (test1==key_exit){
                            //key echap
                            Exit=1;
                            }
                        else if (test1==key_enter && avancement>=2){
                            name_complet=1;
                            }
                        else if (test1==key_sup_text){
                            avancement--;
                            name[avancement]=0;
                            }
                        //recover first name letter
                        else if ((48<=test1 && test1<=59) || (97<=test1 && test1<=122) || (65<=test1 && test1<=90)){
                            name[avancement]=test1;
                            avancement++;
                            }
                        }
                        
                    if (name_complet){
                        int count=0;
                        do{
                        scanf("%d",&seed);
                        display_seed_menu(screen_length,screen_width,background,seed,count);
                        count++;
                        }while(seed>=10000000000 || seed<0);
                        srand(seed);
                        
                        in_game=1;
                        player=build_hero();
                        
                        int roomcount= 1;
                        int doorcount=4;
                        createmaindoor(tabdoor);
                        defin0(tabroom,maxroom);
                        tabroom[0] = createmainroom();
                        
                        timer=time(NULL);
                        time_pause=0;
                        time_jouer=0;
                        }
                    break;
                case 1:
                    name_complet=0;
                    Exit=0;
                    avancement=0;
                    
                    while(!name_complet && !Exit){
                        display_name_load_menu(screen_length,screen_width,background,name,load_menu_cursor);
                        
                        test1=getch();
                        if (test1==key_exit){
                            //key echap
                            Exit=1;
                            }
                        else if (test1==key_enter && avancement>=2){
                            name_complet=1;
                            }
                        else if (test1==key_sup_text){
                            avancement--;
                            name[avancement]=0;
                            }
                        //recover first name letter
                        else if ((48<=test1 && test1<=59) || (97<=test1 && test1<=122) || (65<=test1 && test1<=90)){
                            name[avancement]=test1;
                            avancement++;
                            }
                        }
                        
                    if (name_complet){
                        //search game
                        if(search_save(name,fichier)){
                            //Load information
                            verif=fscanf(fichier,"%d",&player.coordinates.x);
                            if(verif==EOF){
                                printf("error load charging");
                                exit(3);
                                }                            
                            verif=fscanf(fichier,"%d",&player.coordinates.y);
                            if(verif==EOF){
                                printf("error load charging");
                                exit(3);
                                }
                            verif=fscanf(fichier,"%d",&player.life);
                            if(verif==EOF){
                                printf("error load charging");
                                exit(3);
                                }

                            verif=fscanf(fichier,"%d",&player.strength);
                            if(verif==EOF){
                                printf("error load charging");
                                exit(3);
                                }
//recup inventory
                            //player.inventory=calloc(15,sizeof(object));

//complet other information
                            /*
                            roomcount
                            doorcount
                            tabroom
                                //enemy armor blade
                            tabdoor
                            */
                        
                            verif=fscanf(fichier,"%d",&time_jouer);
                            if(verif==EOF){
                                printf("error load charging");
                                exit(3);
                                }
                            timer=time(NULL);
                            time_pause=0;
                            in_game=1;
                            }
                        }
                    break;
                case 2:
//setting menu
//sound
//change key
//language
//screen size
                    printf("la suite au prochain numéro 2 \n");
                    break;
                case 3:
                    in_menu=0;
                    break;
                }
            }
        //select button
        else if (test1==key_exit){
            //key echap
            in_menu=0;
            }
        //key up or key left
        else if (test1==moove_up || test1==moove_left){
            menu_cursor--;
            }
        //key down
        else if (test1==moove_down || test1==moove_right){
            menu_cursor++;
            }
        //keeps the menu cursor on a button
        menu_cursor=(menu_cursor+4)%4;
        
        while(in_game){
            if(!in_pause && !in_inventory){
                //display game
                display_game(screen_length,screen_width,player,background,timer,time_pause,time_jouer,time_to_play);
                //display donjon
                display_donjon(screen_length,screen_width,player,doorcount,tabdoor,roomcount,tabroom,donjon);

                test1=getch();
                //select button
                if (test1==key_exit){
                    //key echap
                    in_pause=1;
                    time_start_pause=time(NULL);

                    //display pause menu
                    display_game(screen_length,screen_width,player,background,timer,time_pause,time_jouer,time_to_play);

                    move(screen_width/2,screen_length/2);
                    printw("pause");
                    move(screen_width+10,screen_length+10);
                    refresh();
                    }

                //key up
                else if (test1==moove_up){
                    player.orientation=0;
                    //moove into door
                    if(player.coordinates.y == tabroom[player.room].ymax){
                        for(int i=0;i<doorcount-1;i++){
                            if(tabdoor[i].x==player.coordinates.x && tabdoor[i].y - 1 == player.coordinates.y){
                                player.coordinates.y+=2;
                                if(tabdoor[i].roomn2 == -1){
                                    //générer room
                                    generateroom(seed,&maxroom,tabroom,&tabdoor[i],&roomcount,tabdoor,&doorcount);
//générer mobs armor blade
                                }
                                if(player.room==tabdoor[i].roomn1){
                                    player.room=tabdoor[i].roomn2;
                                    }
                                else{
                                    player.room=tabdoor[i].roomn1;
                                    }
                                }
                            }
                        }
                    else{
                        player.coordinates.y++;
                        }
                    }
                //key down
                else if (test1==moove_down){
                    player.orientation=2;
                    //moove into door
                    if(player.coordinates.y == tabroom[player.room].ymin){
                        for(int i=0;i<doorcount-1;i++){
                            if(tabdoor[i].x==player.coordinates.x && tabdoor[i].y + 1 == player.coordinates.y){
                                player.coordinates.y-=2;
                                if(tabdoor[i].roomn2 == -1){
                                    //générer room
                                    generateroom(seed,&maxroom,tabroom,&tabdoor[i],&roomcount,tabdoor,&doorcount);
//générer mobs armor blade
                                    }
                                if(player.room==tabdoor[i].roomn1){
                                    player.room=tabdoor[i].roomn2;
                                    }
                                else{
                                    player.room=tabdoor[i].roomn1;
                                    }
                                }
                            }
                        }
                    else{
                        player.coordinates.y--;
                        }
                    }
                //key left
                else if (test1==moove_left){
                    player.orientation=3;
                    //moove into door
                    if(player.coordinates.x == tabroom[player.room].xmin){
                        for(int i=0;i<doorcount-1;i++){
                            if(tabdoor[i].y==player.coordinates.y && tabdoor[i].x + 1 == player.coordinates.y){
                                player.coordinates.x-=2;
                                if(tabdoor[i].roomn2 == -1){
                                    //générer room
                                    generateroom(seed,&maxroom,tabroom,&tabdoor[i],&roomcount,tabdoor,&doorcount);
//générer mobs armor blade
                                    }
                                if(player.room==tabdoor[i].roomn1){
                                    player.room=tabdoor[i].roomn2;
                                    }
                                else{
                                    player.room=tabdoor[i].roomn1;
                                    }
                                }
                            }
                        }
                    else{
                        player.coordinates.x--;
                        }
                    }
                //key right
                else if (test1==moove_right){
                    player.orientation=1;
                    //moove into door
                    if(player.coordinates.x == tabroom[player.room].xmax){
                        for(int i=0;i<doorcount-1;i++){
                            if(tabdoor[i].y==player.coordinates.y && tabdoor[i].x - 1 == player.coordinates.x){
                                player.coordinates.x+=2;
                                if(tabdoor[i].roomn2 == -1){
                                    //générer room
                                    generateroom(seed,&maxroom,tabroom,&tabdoor[i],&roomcount,tabdoor,&doorcount);
//générer mobs armor blade
                                    }
                                if(player.room==tabdoor[i].roomn1){
                                    player.room=tabdoor[i].roomn2;
                                    }
                                else{
                                    player.room=tabdoor[i].roomn1;
                                    }
                                }
                            }
                        }
                    else{
                        player.coordinates.x++;
                        }
                    }

                //press 0
                else if(test1==key_inventory){
                    //open inventory
                    in_inventory=1;
                    inventory_cursor=0;
                    }

                //press enter
                else if(test1==key_enter){
                    //collect thing
                    //a=tab_room
                    collect_object(&player,tabroom);
                    collect_armor(&player,tabroom,screen_length,screen_width,background,timer,time_pause,time_jouer,time_to_play);
                    collect_blade(&player,tabroom,screen_length,screen_width,background,timer,time_pause,time_jouer,time_to_play);
                    }
                    
                else if(test1==key_fight){
                    //attack
                    you_hit(&player,tabroom);

                    printw("you hit nothing\n");
                    refresh();
                    }
                }
            else if(in_pause){
                test1=getch();
                if (test1==key_exit){
                    //key echap
                    in_game=0;
                    in_pause=0;
                    menu_cursor=0;
                    time_pause+=time(NULL)-time_start_pause;
                    time_jouer+=(time(NULL)-timer)-time_pause;
                    
                    //save
                    int verif2=EOF;
                    if(!search_save(name,fichier)){
                        printf("%s",name);
                        exit(12);
//pb avec nom
                        verif2=fprintf(fichier,"@%s",name);
                        if(verif2==EOF){
                            printf("error load charging");
                            exit(5);
                            }  
                        }
                    verif2=fprintf(fichier,"\n");
                    if(verif2==EOF){
                        printf("error load charging");
                        exit(5);
                        }      
                    verif2=fprintf(fichier,"%d\n",player.coordinates.x);
                    if(verif2==EOF){
                        printf("error load charging");
                        exit(5);
                        }                            
                    verif2=fprintf(fichier,"%d\n",player.coordinates.y);
                    if(verif2==EOF){
                        printf("error load charging");
                        exit(5);
                        }
                    verif2=fprintf(fichier,"%d\n",player.life);
                    if(verif2==EOF){
                        printf("error load charging");
                        exit(5);
                        }
                    verif2=fprintf(fichier,"%d\n",player.strength);
                    if(verif2==EOF){
                        printf("error load charging");
                        exit(5);
                        }
//recup inventory
                            //player.inventory=calloc(15,sizeof(object));

//complet other information
                            /*
                            roomcount
                            doorcount
                            tabroom
                                //enemy armor blade
                            tabdoor
                            */

                    verif2=fprintf(fichier,"%d\n",time_jouer);
                    if(verif2==EOF){
                        printf("error load charging");
                        exit(5);
                        }
                    }
                else if(test1!=-1){
                    //continue
                    in_pause=0;
                    time_pause+=time(NULL)-time_start_pause;
                    }
                }

            else if(in_inventory){
                //display game
                display_game(screen_length,screen_width,player,background,timer,time_pause,time_jouer,time_to_play);
                //display inventory menu
                display_inventory_menu(screen_length,screen_width,player,donjon,inventory_cursor);

                test1=getch();
                if (test1==key_exit || test1==key_inventory){
                    //close inventory
                    in_inventory=0;
                    }
                //key up
                else if (test1==moove_up && inventory_cursor-5>=0){
                    inventory_cursor-=5;
                    }
                //key down
                else if (test1==moove_down && inventory_cursor+5<=14){ //inventory size-1
                    inventory_cursor+=5;
                    }
                //key left
                else if (test1==moove_left && inventory_cursor-1>=0){
                    inventory_cursor--;
                    }
                //key right
                else if (test1==moove_right && inventory_cursor+1<=14){//inventory size-1
                    inventory_cursor++;
                    }
                //press *
                else if(test1==key_delete){
                    do{
                        //refresh the timer
                        //display game
                        display_game(screen_length,screen_width,player,background,timer,time_pause,time_jouer,time_to_play);
                        //display inventory menu
                        display_inventory_menu(screen_length,screen_width,player,donjon,inventory_cursor);
                        //verfication
                        attron(COLOR_PAIR(8));
                        mvprintw(screen_width-4,4,"are you sure you want to delete this item %s?",player.inventory[inventory_cursor].name);
                        mvprintw(screen_width-3,4,"y/n");
                        refresh();

                        test1=getch();
                        //yes
                        if(test1==121){
                            player.inventory[inventory_cursor].id=0;
                            }
                    }while(test1!=121 && test1!=110);
                    }
                //press enter
                else if(test1==key_enter){
                    //use item
                    use_object(&player,inventory_cursor);
                    }
                }
            
            //ennemy enters or hits player
            be_hit(&player,tabroom);
            
            win_level(&player);
            
            if(!in_pause && player.life<=0){
                death(&player);
                }
            }
        }
    free(player.inventory);
    delwin(background);
    endwin();
    return 0;
    }
