//
//  smm_node.c
//  SMMarble
//
//  Created by Suhyeon Lee.
//  게임에 필요한 객체(노드, 음식카드, 축제카드, 수강 이력) 정보 저장 

#include <string.h>

#include "smm_common.h"
#include "smm_object.h"


#define MAX_NODETYPE    7
#define MAX_GRADE       9
#define MAX_NODE        100


static char smmNodeName[MAX_NODETYPE][MAX_CHARNAME] = {
    "lecture",
    "restaurant",
    "laboratory",
    "home",
    "experiment",
    "foodChance",
    "festival"
};

char* smmObj_getTypeName(int type)
{
      return (char*)smmNodeName[type];
}



typedef enum smmObjGrade {
    smmObjGrade_Ap = 0,
    smmObjGrade_A0,
    smmObjGrade_Am,
    smmObjGrade_Bp,
    smmObjGrade_B0,
    smmObjGrade_Bm,
    smmObjGrade_Cp,
    smmObjGrade_C0,
    smmObjGrade_Cm
} smmObjGrade_e;

//1. 구조체 형식 정의 
typedef struct smmObject {
       char name[MAX_CHARNAME];
       smmObjType_e objType; 
       int type;
       int credit;
       int energy;
       smmObjGrade_e grade;
} smmObject_t;

// SMMObject_t 배열 및 노드 개수 초기화
static smmObject_t smm_node[MAX_NODE];
static int smmObj_noNode = 0;

//3. 관련 함수 변경 
//object generation
//입력 매개변수를 받아 새로운 SMMObject_t 객체를 생성하고 포인터를 반환
void* smmObj_genObject(char* name, smmObjType_e objType, int type, int credit, int energy, smmObjGrade_e grade)
{    
    smmObject_t* ptr;
    
    ptr = (smmObject_t*)malloc(sizeof(smmObject_t));
    
    strcpy(ptr->name, name);
    ptr->objType = objType;
    ptr->type = type;
    ptr->credit = credit;
    ptr->energy = energy;
    ptr->grade = grade;
    
    return ptr;
}

//3. 관련 함수 변경 
// 객체 포인터에서 이름을 추출하여 반환
char* smmObj_getNodeName(void* obj)
{
    smmObject_t* ptr = (smmObject_t*)obj;// 입력된 객체를 SMMObject_t 포인터로 변환
    
    return ptr->name;
}

// 노드 유형 가져오는 함수
// 노드 번호로부터 해당 노드의 유형을 반환
int smmObj_getNodeType(int node_nr)
{
    return smm_node[node_nr].type;
}
// 노드 학점 가져오는 함수
//노드 번호로부터 해당 노드의 학점을 반환
int smmObj_getNodeCredit(int node_nr)
{
    return smm_node[node_nr].credit;
}
// 노드 에너지 가져오는 함수
// 노드 번호로부터 해당 노드의 에너지를 반환
int smmObj_getNodeEnergy(int node_nr)
{
    return smm_node[node_nr].energy;
}

//오브젝트에서 객체구조체정의, 객체구조체 다루는 함수 구현하기
//main에서 입출력을 받은걸(파일읽어온것) 구조체(object에서정의된거)에 넣은다음 database함수를 통해 링크드리스트에 저장 ->void타입의obj변수에 각구조체의 주소저장
//main에 플레이어 설정하는 코드 구현(플레이어 생성 o , 구조체수정x)
//main에서 플레이 흐름구현
