//
//  smm_node.c
//  SMMarble
//
//  Created by Suhyeon Lee.
//  ���ӿ� �ʿ��� ��ü(���, ����ī��, ����ī��, ���� �̷�) ���� ���� 

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

//1. ����ü ���� ���� 
typedef struct smmObject {
       char name[MAX_CHARNAME];
       smmObjType_e objType; 
       int type;
       int credit;
       int energy;
       smmObjGrade_e grade;
} smmObject_t;

// SMMObject_t �迭 �� ��� ���� �ʱ�ȭ
static smmObject_t smm_node[MAX_NODE];
static int smmObj_noNode = 0;

//3. ���� �Լ� ���� 
//object generation
//�Է� �Ű������� �޾� ���ο� SMMObject_t ��ü�� �����ϰ� �����͸� ��ȯ
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

//3. ���� �Լ� ���� 
// ��ü �����Ϳ��� �̸��� �����Ͽ� ��ȯ
char* smmObj_getNodeName(void* obj)
{
    smmObject_t* ptr = (smmObject_t*)obj;// �Էµ� ��ü�� SMMObject_t �����ͷ� ��ȯ
    
    return ptr->name;
}

// ��� ���� �������� �Լ�
// ��� ��ȣ�κ��� �ش� ����� ������ ��ȯ
int smmObj_getNodeType(int node_nr)
{
    return smm_node[node_nr].type;
}
// ��� ���� �������� �Լ�
//��� ��ȣ�κ��� �ش� ����� ������ ��ȯ
int smmObj_getNodeCredit(int node_nr)
{
    return smm_node[node_nr].credit;
}
// ��� ������ �������� �Լ�
// ��� ��ȣ�κ��� �ش� ����� �������� ��ȯ
int smmObj_getNodeEnergy(int node_nr)
{
    return smm_node[node_nr].energy;
}

//������Ʈ���� ��ü����ü����, ��ü����ü �ٷ�� �Լ� �����ϱ�
//main���� ������� ������(�����о�°�) ����ü(object�������ǵȰ�)�� �������� database�Լ��� ���� ��ũ�帮��Ʈ�� ���� ->voidŸ����obj������ ������ü�� �ּ�����
//main�� �÷��̾� �����ϴ� �ڵ� ����(�÷��̾� ���� o , ����ü����x)
//main���� �÷��� �帧����
