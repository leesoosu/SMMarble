//
//  main.c
//  SMMarble
//  Player management and realization of the main game flow
//  Create object structs via file I/O and store them in a linked list
//  Player setup
//  Code implementation according to the game flow
//
//  Created by Suhyeon Lee.
//

#include "smm_object.h" // Error occurs in this part
#include "smm_database.h"
#include "smm_common.h"
#include <time.h>
#include <string.h>

// Defining file paths as constants for easy file access
#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestivalConfig.txt"

// Board configuration parameters
static int board_nr;
static int food_nr;
static int festival_nr;

static int player_nr;

typedef struct player {               // Structure containing player status-related variables
        int energy;                   // Player energy
        int position;                 // Player position
        char name[MAX_CHARNAME];      // Player name (maximum character limit)
        int accumCredit;              // Accumulated credits obtained by the player
        int flag_graduate;            // Flag indicating graduation status (whether the player has graduated)
} player_t;

static player_t *cur_player;          // Array pointer for players currently participating in the game
//static player_t cur_player[MAX_PLAYER];

//컴파일되지 않는 코드 
#if 0
static int player_energy[MAX_PLAYER];
static int player_position[MAX_PLAYER];
static char player_name[MAX_PLAYER][MAX_CHARNAME];
#endif

//function prototypes(함수 프로토타입) 
#if 0
int isGraduated(void); //check if any player is graduated(졸업한 플레이어 확인) 
 //print grade history of the player(플레이어 학점 기록 출력) 
void goForward(int player, int step); //make player go "step" steps on the board (check if player is graduated)
void printPlayerStatus(void); //print all player status at the beginning of each turn
float calcAverageGrade(int player); //calculate average grade of the player,
smmGrade_e takeLecture(int player, char *lectureName, int credit); //take the lecture (insert a grade of the player)
void* findGrade(int player, char *lectureName); //find the grade from the player's grade history
void printGrades(int player); //print all the grade history of the player
void generatePlayers(int n, int initEnergy);
#endif

//플레이어의 모든 학점을 출력하는 함수 
void printGrades(int player)
{
     int i;
     void *gradePtr;
     //해당 플레이어의 학점 노드를 모두 반복하며 출력 
     for (i=0;i<smmdb_len(LISTNO_OFFSET_GRADE + player);i++)
     {
         gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
         printf("%s : %i\n", smmObj_getNodeName(gradePtr), smmObj_getNodeGrade(gradePtr));
     }
}
//모든 플레이어의 상태(이름, 학점, 에너지, 위치)를 출력하는 함수
void printPlayerStatus(void)
{
     int i;
     //모든 플레이어의 상태를 출력 
     for (i=0;i<player_nr;i++)
     {
         printf("%s : credit %i, energy %i, position %i\n", 
                      cur_player[i].name,
                      cur_player[i].accumCredit,
                      cur_player[i].energy,
                      cur_player[i].position);
     }
}
//새로운 플레이어 생성하는 함수 
void generatePlayers(int n, int initEnergy) //generate a new player
{
     int i;
     //n time loop만큼 플레이어 생성 
     for (i=0;i<n;i++)
     {
         //input name
         printf("Input player %i's name:", i); //??? ???? 
         scanf("%s", cur_player[i].name);
         fflush(stdin);
         
         //set position, 초기 위치를 0으로 설정 
         //player_position[i] = 0;
         cur_player[i].position = 0;
         
         //set energy
         //player_energy[i] = initEnergy;
         cur_player[i].energy = initEnergy;     //초기 에너지 설정 
         cur_player[i].accumCredit = 0;         //누적 학점 초기화 
         cur_player[i].flag_graduate = 0;       //졸업 여부 초기화 
     }
}

//임의의 키로 주사위 굴리는 함수 
int rolldie(int player)
{
    char c;
    printf(" Press any key to roll a die (press g to see grade): ");
    c = getchar();
    fflush(stdin);
    
#if 1
    if (c == 'g')                  //g를 누르면 학점을 출력 
        printGrades(player);
#endif
    
    return (rand()%MAX_DIE + 1);   //랜덤으로 주사위 값을 반환 
}

//action code when a player stays at a node
//플레이어가 노드에 머물렀을 때 행동을 처리하는 함수
void actionNode(int player)
{
    void *boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position );
    //int type = smmObj_getNodeType( cur_player[player].position );
    int type = smmObj_getNodeType( boardPtr );
  //  char *name = smmObj_getNodeName( boardPtr );
    void *gradePtr;
    
    switch(type) //노드 타입에 따라 특정 행동을 취한다. 
    {
    case 0:
        printf("lecture"); //전공 과목을 들을 수 있음 
        break;
    case 1:
        printf("restaurant"); //에너지를 보충함 
        break;
    case 2:
        printf("laboratory"); //실험을 함(무인도) 
        break;
    case 3:
        printf("home"); //시작 포인트, 종료 포인트, 지날 때마다 체력 보충 
        break;
    case 4:
        printf("experiment"); //실험실로 이동
        break;
    case 5:
        printf("foodChance"); //랜덤으로 음식을 먹고 체력 보충
        break;
    case 6:
        printf("festival"); // 축제 -> 랜덤으로 미션 수행
        break;
    default:
        break;
    }
}
//플레이어를 이동시킴,(졸업 체크, 위치 업데이트) 
void goForward(int player, int step)
{
    void *boardPtr;
    // 플레이어의 위치를 주어진 스텝만큼 증가시킴
    cur_player[player].position += step;   
    // 플레이어가 이동한 새로운 위치의 게임 보드 데이터를 가져옴
    boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
    
    printf("%s goes to node %i (name: %s)\n", 
           cur_player[player].name, cur_player[player].position,
           smmObj_getNodeName(boardPtr));
}



int main(int argc, const char * argv[]) {
    
    FILE* fp;
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
    int i;
    int initEnergy;
    int turn=0;
    
    //게임의 초기 노드, 음식, 축제  
    board_nr = 0;
    food_nr = 0;
    festival_nr = 0;
    
    //난수 설정 
    srand(time(NULL));
    
    
    //1. import parameters ---------------------------------------------------------------------------------
    //1-1. boardConfig 
    if ((fp = fopen(BOARDFILEPATH, "r")) == NULL) {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", BOARDFILEPATH);
        getchar();
        return -1;
    }

    printf("Reading board component......\n");
    while (fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy) == 4) {
        //void *boardObj = smmObj_genObject(name, SMMNODE_TYPE_MAX, type, credit, energy, 0); 
        //smmdb_addTail(LISTNO_NODE, boardObj);
        
        //if (type == SMMNODE_TYPE_HOME)
        //게임 보드의 설정 파일을 읽어온 후 초기 에너지 설정 
           initEnergy = energy;
        board_nr++;
    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", board_nr);

   
    //게임 보드 노드 출력 
    for (i = 0;i<board_nr;i++)
    {
        void *boardObj = smmdb_getData(LISTNO_NODE, i);
        
        printf("node %i : %s, %i(%s), credit %i, energy %i\n", 
                     i, smmObj_getNodeName(boardObj), 
                     smmObj_getNodeType(boardObj), smmObj_getTypeName(smmObj_getNodeType(boardObj)),
                     smmObj_getNodeCredit(boardObj), smmObj_getNodeEnergy(boardObj));
    }
    //printf("(%s)", smmObj_getTypeName(SMMNODE_TYPE_LECTURE));
    
    #if 0
    //2. food card config 
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
        return -1;
    }
    
    printf("\n\nReading food card component......\n");
    while () //read a food parameter set
    {
        //store the parameter set
    }
    fclose(fp);
    printf("Total number of food cards : %i\n", food_nr);
    
    
    
    //3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\n\nReading festival card component......\n");
    while () //read a festival card string
    {
        //store the parameter set
    }
    fclose(fp);
    printf("Total number of festival cards : %i\n", festival_nr);
    #endif
    
    
    //2. Player configuration ---------------------------------------------------------------------------------
    
    do
    {
        //input player number to player_nr
        printf("input player no.:");
        scanf("%d", &player_nr);
        fflush(stdin);
    }
    while (player_nr < 0 || player_nr >  MAX_PLAYER);
    
    cur_player = (player_t*)malloc(player_nr*sizeof(player_t));
    generatePlayers(player_nr, initEnergy);
    
    
    //3. SM Marble game starts ---------------------------------------------------------------------------------
    while (1) //is anybody graduated?
    {
        int die_result;
        
        
        //4-1. initial printing
        printPlayerStatus();
        
        //4-2. die rolling (if not in experiment)        
        die_result = rolldie(turn);
        
        //4-3. go forward
        goForward(turn, die_result);

		//4-4. take action at the destination node of the board
        actionNode(turn);
        
        //4-5. next turn
        turn = (turn + 1)%player_nr;
    }
    
    
    free(cur_player);
    system("PAUSE");
    return 0;
}
