//
//  smm_database.c
//  Sookmyung Marble
//  Database platform code for storing marble board elements
//  Created by Suhyeon Lee.
//  ������ ���, ����ī��, ����ī��, �� �÷��̾��� ���� ���� �̷��� ���� 

#include "smm_common.h"
#include "smm_database.h"

#define LIST_END            -1
#define MAX_LIST            LISTNO_OFFSET_GRADE+MAX_PLAYER

//node definition for linked list, ����� ����ũ ��� ���� 
typedef struct node{
    int index;               //index of the node
    void* obj;               //object data
    void* next;              //���� ��带 ����Ű�� ������ 
    void* prev;              //���� ��带 ����Ű�� ������ 
} node_t;

//����Ʈ���� �����ϱ� ���� �迭�� ���� 
static node_t* list_database[MAX_LIST];   //����Ʈ�� ���� ��带 ����Ű�� ������ �迭 
static node_t* listPtr[MAX_LIST];         //�� ����Ʈ�� ���� ��ġ�� ����Ű�� ������ �迭 
static int list_cnt[MAX_LIST];            //�� ����Ʈ�� ���̸� ��Ÿ���� �迭 


//Inner functions (cannot used at the outside of this file)
//���ο� ��带 �����ϴ� ���� �Լ� 
static node_t* genNode(void)
{
    //allocate memory for creating the node
    node_t* ndPtr = (node_t*)malloc(sizeof(node_t));
    if (ndPtr != NULL)
    {
        //��� �ʱ�ȭ 
        ndPtr->next = NULL;
        ndPtr->obj = NULL;
        ndPtr->prev = NULL;
    }
    
    return ndPtr;
}
// ����Ʈ���� Ư�� �ε����� ��带 ã�� ���� �Լ�
static node_t* smmList(int list_nr, int index)
{
    //����Ʈ���� �ش��ϴ� �ε����� ��带 ã�� ��ȯ 
    node_t* ndPtr = list_database[list_nr];//���� ��带 ����Ŵ 
    if (listPtr[list_nr] != NULL && listPtr[list_nr]->index <= index)
    {
        ndPtr = listPtr[list_nr];
    }
    
    //return for wrong input
    //�߸��� �Է� ó�� 
    if (index <-1)
    {
        printf("[ERROR] smmList() : either list is null or index is wrong! (offset : %i)\n", index);
        return NULL;
    }
    if (index >= list_cnt[list_nr])
    {
        printf("[ERROR] smmList() : index is larger than length (len:%i, index:%i)\n", list_cnt[list_nr], index);
        return NULL;
    }
    
    //repeat travelling until the node is the end or offset becomes 0
    //��尡 ����ǰų� 0�� �� ������ �̵�
    //��带 ��ȸ�ϸ� �ش��ϴ� �ε����� ��带 ã���ϴ�.
    while (ndPtr != NULL)
    {
        if (ndPtr->index == index)
            break;
        ndPtr = ndPtr->next;
    }
    
    return ndPtr;
}

//����Ʈ�� ��� �ε����� ������Ʈ�ϴ� ���� �Լ�
static int updateIndex(int list_nr)
{
    int index=0;
    node_t* ndPtr = list_database[list_nr];
    
    while ( ndPtr != NULL )//travel until it is the end node
    {
        ndPtr->index = index++;
        ndPtr = ndPtr->next; //travel once
    }
    
    return index;
}



//API function
/*
    description : adding a data object to the list end
    input parameters : obj - data object to add to the list
    return value : addition result (0 - succeeded, -1 - failed)
    
    operation : 1. make a new node
                2. find the last node in the list
                3. make the last node's next pointer to point the new node
                4. update the index
*/

//����Ʈ�� ���� ���ο� ������ ��ü �߰� 
int smmdb_addTail(int list_nr, void* obj)
{
    node_t* ndPtr;
    node_t* newNdPtr;
    
    //parameter checking
    // �Էµ� ��ü�� NULL���� Ȯ���ϰ�, NULL�̸� ������ ����ϰ� -1�� ��ȯ
    if (obj == NULL)
    {
        printf("[ERROR] smmdb_addTail() : Failed to do addTail : input object indicates NULL!\n");
        return -1;
    }
    
    //generate new node
    //���ο� ��带 �����ϰ�, ������ �����ϸ� ������ ����ϰ� -1�� ��ȯ
    newNdPtr = genNode();
    if (newNdPtr == NULL)
    {
        printf("[ERROR] smmdb_addTail() : Failed to do addTail : Failed to create a node\n");
        return -1; //indicate to the upper function that node addition is failed
    }
    newNdPtr->obj = obj;      //���ο� ��忡 �����͸� �Ҵ� 
    
    //add node to the list tail
    if (list_database[list_nr] == NULL)
    {
        list_database[list_nr] = newNdPtr;// ����Ʈ�� ����ִٸ� ���ο� ��带 ����Ʈ�� ó������ ����
        newNdPtr->index = 0;
    }
    else
    {
        ndPtr = smmList(list_nr, list_cnt[list_nr]-1);           //����Ʈ�� ������ ��带 ã�� 
        ndPtr->next = newNdPtr;                                  //������ ����� ������ ���ο� ���� ����                      
        newNdPtr->prev = ndPtr;                                  //���ο� ����� ������ ������ ���� ���� 
        newNdPtr->index = ndPtr->index+1;                         
    }
    
    listPtr[list_nr] = newNdPtr;                                  //����Ʈ�� ���� ��ġ�� ���ο� ���� ���� 
    
    list_cnt[list_nr]++;                                          //����Ʈ�� ��� ������ ���� 
    
    return 0;
}


/*
    description : delete data object from the list
    input parameters : index - index'th data to delete
    return value : deletion result (0 - succeeded, -1 - failed)
*/
//����Ʈ���� Ư�� �ε����� ������ ��ü�� ���� 
int smmdb_deleteData(int list_nr, int index)
{
    node_t* ndPrevPtr;
    node_t* ndNextPtr;
    node_t* delNdPtr;
    
    //parameter checking
    //�Էµ� �ε����� ��带 ã��, ã�� ���ϸ� ������ ����ϰ� -1�� ��ȯ
    if ( (delNdPtr = smmList(list_nr, index)) == NULL)
    {
        printf("[ERROR] smmdb_deleteData() : Failed to do deleteData : input index is invalid (%i)!\n", index);
        return -1;
    }
    
    ndPrevPtr = delNdPtr->prev;// ������ ����� ���� ��带 ����
    if (ndPrevPtr != NULL)
    {
        ndPrevPtr->next = delNdPtr->next;// ������ ����� ���� ����� ���� ��带 ������ ����� ���� ���� ����
    }
    ndNextPtr = delNdPtr->next;// ������ ����� ���� ��带 ����
    if (ndNextPtr != NULL)
    {
        ndNextPtr->prev = delNdPtr->prev;// ������ ����� ���� ����� ���� ��带 ������ ����� ���� ���� ����
    }
    
    free(delNdPtr->obj);  // ��忡 �Ҵ�� ��ü �޸𸮸� ����
    free(delNdPtr);       // ��� �޸𸮸� ����
    
    list_cnt[list_nr]--;  // ����Ʈ�� ��� ������ ����
    
    // ����Ʈ�� ������� �ʴٸ� �ε����� ������Ʈ
    if (list_cnt[list_nr] == 0)
    {
        list_database[list_nr] = NULL;
    }
    else
    {
        updateIndex(list_nr);
    }
    
    return 0;
}


//functions for list observation -----------------------------

/*
    description : return the number of data objects in the list
    return value : length
*/
int smmdb_len(int list_nr)
{
    return list_cnt[list_nr];
}


/*
    description : get the object data
    input parameters : index
    return value : object pointer
*/
void* smmdb_getData(int list_nr, int index)
{
    void* obj = NULL;
    node_t* ndPtr;
    
    //parameter checking
    if ((ndPtr = smmList(list_nr, index)) != NULL)
    {
        obj = (void*)ndPtr->obj;
        listPtr[list_nr] = ndPtr;
    }
    
    if (obj == NULL)
        printf("[ERROR] smmdb_getData() : there is no data of index %i\n", index);
    
    return obj;
}
