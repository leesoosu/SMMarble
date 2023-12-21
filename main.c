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

//�����ϵ��� �ʴ� �ڵ� 
#if 0
static int player_energy[MAX_PLAYER];
static int player_position[MAX_PLAYER];
static char player_name[MAX_PLAYER][MAX_CHARNAME];
#endif

//function prototypes(�Լ� ������Ÿ��) 
#if 0
int isGraduated(void); //check if any player is graduated(������ �÷��̾� Ȯ��) 
 //print grade history of the player(�÷��̾� ���� ��� ���) 
void goForward(int player, int step); //make player go "step" steps on the board (check if player is graduated)
void printPlayerStatus(void); //print all player status at the beginning of each turn
float calcAverageGrade(int player); //calculate average grade of the player,
smmGrade_e takeLecture(int player, char *lectureName, int credit); //take the lecture (insert a grade of the player)
void* findGrade(int player, char *lectureName); //find the grade from the player's grade history
void printGrades(int player); //print all the grade history of the player
void generatePlayers(int n, int initEnergy);
#endif

//�÷��̾��� ��� ������ ����ϴ� �Լ� 
void printGrades(int player)
{
     int i;
     void *gradePtr;
     //�ش� �÷��̾��� ���� ��带 ��� �ݺ��ϸ� ��� 
     for (i=0;i<smmdb_len(LISTNO_OFFSET_GRADE + player);i++)
     {
         gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
         printf("%s : %i\n", smmObj_getNodeName(gradePtr), smmObj_getNodeGrade(gradePtr));
     }
}
//��� �÷��̾��� ����(�̸�, ����, ������, ��ġ)�� ����ϴ� �Լ�
void printPlayerStatus(void)
{
     int i;
     //��� �÷��̾��� ���¸� ��� 
     for (i=0;i<player_nr;i++)
     {
         printf("%s : credit %i, energy %i, position %i\n", 
                      cur_player[i].name,
                      cur_player[i].accumCredit,
                      cur_player[i].energy,
                      cur_player[i].position);
     }
}
//���ο� �÷��̾� �����ϴ� �Լ� 
void generatePlayers(int n, int initEnergy) //generate a new player
{
     int i;
     //n time loop��ŭ �÷��̾� ���� 
     for (i=0;i<n;i++)
     {
         //input name
         printf("Input player %i's name:", i); //??? ???? 
         scanf("%s", cur_player[i].name);
         fflush(stdin);
         
         //set position, �ʱ� ��ġ�� 0���� ���� 
         //player_position[i] = 0;
         cur_player[i].position = 0;
         
         //set energy
         //player_energy[i] = initEnergy;
         cur_player[i].energy = initEnergy;     //�ʱ� ������ ���� 
         cur_player[i].accumCredit = 0;         //���� ���� �ʱ�ȭ 
         cur_player[i].flag_graduate = 0;       //���� ���� �ʱ�ȭ 
     }
}

//������ Ű�� �ֻ��� ������ �Լ� 
int rolldie(int player)
{
    char c;
    printf(" Press any key to roll a die (press g to see grade): ");
    c = getchar();
    fflush(stdin);
    
#if 1
    if (c == 'g')                  //g�� ������ ������ ��� 
        printGrades(player);
#endif
    
    return (rand()%MAX_DIE + 1);   //�������� �ֻ��� ���� ��ȯ 
}

//action code when a player stays at a node
//�÷��̾ ��忡 �ӹ����� �� �ൿ�� ó���ϴ� �Լ�
void actionNode(int player)
{
    void *boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position );
    //int type = smmObj_getNodeType( cur_player[player].position );
    int type = smmObj_getNodeType( boardPtr );
  //  char *name = smmObj_getNodeName( boardPtr );
    void *gradePtr;
    
    switch(type) //��� Ÿ�Կ� ���� Ư�� �ൿ�� ���Ѵ�. 
    {
    case 0:
        printf("lecture"); //���� ������ ���� �� ���� 
        break;
    case 1:
        printf("restaurant"); //�������� ������ 
        break;
    case 2:
        printf("laboratory"); //������ ��(���ε�) 
        break;
    case 3:
        printf("home"); //���� ����Ʈ, ���� ����Ʈ, ���� ������ ü�� ���� 
        break;
    case 4:
        printf("experiment"); //����Ƿ� �̵�
        break;
    case 5:
        printf("foodChance"); //�������� ������ �԰� ü�� ����
        break;
    case 6:
        printf("festival"); // ���� -> �������� �̼� ����
        break;
    default:
        break;
    }
}
//�÷��̾ �̵���Ŵ,(���� üũ, ��ġ ������Ʈ) 
void goForward(int player, int step)
{
    void *boardPtr;
    // �÷��̾��� ��ġ�� �־��� ���ܸ�ŭ ������Ŵ
    cur_player[player].position += step;   
    // �÷��̾ �̵��� ���ο� ��ġ�� ���� ���� �����͸� ������
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
    
    //������ �ʱ� ���, ����, ����  
    board_nr = 0;
    food_nr = 0;
    festival_nr = 0;
    
    //���� ���� 
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
        //���� ������ ���� ������ �о�� �� �ʱ� ������ ���� 
           initEnergy = energy;
        board_nr++;
    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", board_nr);

   
    //���� ���� ��� ��� 
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
