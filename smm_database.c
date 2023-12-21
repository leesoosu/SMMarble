//
//  smm_database.c
//  Sookmyung Marble
//  Database platform code for storing marble board elements
//  Created by Suhyeon Lee.
//  보드의 노드, 음식카드, 축제카드, 각 플레이어의 수강 강의 이력을 저장 

#include "smm_common.h"
#include "smm_database.h"

#define LIST_END            -1
#define MAX_LIST            LISTNO_OFFSET_GRADE+MAX_PLAYER

//node definition for linked list, 연결된 리스크 노드 정의 
typedef struct node{
    int index;               //index of the node
    void* obj;               //object data
    void* next;              //다음 노드를 가리키는 포인터 
    void* prev;              //이전 노드를 가리키는 포인터 
} node_t;

//리스트들을 관리하기 위한 배열과 변수 
static node_t* list_database[MAX_LIST];   //리스트의 시작 노드를 가리키는 포인터 배열 
static node_t* listPtr[MAX_LIST];         //각 리스트의 현재 위치를 가리키는 포인터 배열 
static int list_cnt[MAX_LIST];            //각 리스트의 길이를 나타내는 배열 


//Inner functions (cannot used at the outside of this file)
//새로운 노드를 생성하는 내부 함수 
static node_t* genNode(void)
{
    //allocate memory for creating the node
    node_t* ndPtr = (node_t*)malloc(sizeof(node_t));
    if (ndPtr != NULL)
    {
        //노드 초기화 
        ndPtr->next = NULL;
        ndPtr->obj = NULL;
        ndPtr->prev = NULL;
    }
    
    return ndPtr;
}
// 리스트에서 특정 인덱스의 노드를 찾는 내부 함수
static node_t* smmList(int list_nr, int index)
{
    //리스트에서 해당하는 인덱스의 노드를 찾아 반환 
    node_t* ndPtr = list_database[list_nr];//시작 노드를 가리킴 
    if (listPtr[list_nr] != NULL && listPtr[list_nr]->index <= index)
    {
        ndPtr = listPtr[list_nr];
    }
    
    //return for wrong input
    //잘못된 입력 처리 
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
    //노드가 종료되거나 0이 될 때까지 이동
    //노드를 순회하며 해당하는 인덱스의 노드를 찾습니다.
    while (ndPtr != NULL)
    {
        if (ndPtr->index == index)
            break;
        ndPtr = ndPtr->next;
    }
    
    return ndPtr;
}

//리스트의 노드 인덱스를 업데이트하는 내부 함수
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

//리스트의 끝에 새로운 데이터 객체 추가 
int smmdb_addTail(int list_nr, void* obj)
{
    node_t* ndPtr;
    node_t* newNdPtr;
    
    //parameter checking
    // 입력된 객체가 NULL인지 확인하고, NULL이면 에러를 출력하고 -1을 반환
    if (obj == NULL)
    {
        printf("[ERROR] smmdb_addTail() : Failed to do addTail : input object indicates NULL!\n");
        return -1;
    }
    
    //generate new node
    //새로운 노드를 생성하고, 생성에 실패하면 에러를 출력하고 -1을 반환
    newNdPtr = genNode();
    if (newNdPtr == NULL)
    {
        printf("[ERROR] smmdb_addTail() : Failed to do addTail : Failed to create a node\n");
        return -1; //indicate to the upper function that node addition is failed
    }
    newNdPtr->obj = obj;      //새로운 노드에 데이터를 할당 
    
    //add node to the list tail
    if (list_database[list_nr] == NULL)
    {
        list_database[list_nr] = newNdPtr;// 리스트가 비어있다면 새로운 노드를 리스트의 처음으로 설정
        newNdPtr->index = 0;
    }
    else
    {
        ndPtr = smmList(list_nr, list_cnt[list_nr]-1);           //리스트의 마지막 노드를 찾음 
        ndPtr->next = newNdPtr;                                  //마지막 노드의 다음을 새로운 노드롤 설정                      
        newNdPtr->prev = ndPtr;                                  //새로운 노드의 이전을 마지막 노드로 설정 
        newNdPtr->index = ndPtr->index+1;                         
    }
    
    listPtr[list_nr] = newNdPtr;                                  //리스트의 현재 위치를 새로운 노드로 설정 
    
    list_cnt[list_nr]++;                                          //리스트의 노드 개수를 증가 
    
    return 0;
}


/*
    description : delete data object from the list
    input parameters : index - index'th data to delete
    return value : deletion result (0 - succeeded, -1 - failed)
*/
//리스트에서 특정 인덱스의 데이터 객체를 삭제 
int smmdb_deleteData(int list_nr, int index)
{
    node_t* ndPrevPtr;
    node_t* ndNextPtr;
    node_t* delNdPtr;
    
    //parameter checking
    //입력된 인덱스의 노드를 찾고, 찾지 못하면 에러를 출력하고 -1을 반환
    if ( (delNdPtr = smmList(list_nr, index)) == NULL)
    {
        printf("[ERROR] smmdb_deleteData() : Failed to do deleteData : input index is invalid (%i)!\n", index);
        return -1;
    }
    
    ndPrevPtr = delNdPtr->prev;// 삭제할 노드의 이전 노드를 저장
    if (ndPrevPtr != NULL)
    {
        ndPrevPtr->next = delNdPtr->next;// 삭제할 노드의 이전 노드의 다음 노드를 삭제할 노드의 다음 노드로 설정
    }
    ndNextPtr = delNdPtr->next;// 삭제할 노드의 다음 노드를 저장
    if (ndNextPtr != NULL)
    {
        ndNextPtr->prev = delNdPtr->prev;// 삭제할 노드의 다음 노드의 이전 노드를 삭제할 노드의 이전 노드로 설정
    }
    
    free(delNdPtr->obj);  // 노드에 할당된 객체 메모리를 해제
    free(delNdPtr);       // 노드 메모리를 해제
    
    list_cnt[list_nr]--;  // 리스트의 노드 개수를 감소
    
    // 리스트가 비어있지 않다면 인덱스를 업데이트
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
