#include "table.h"
#include <stdio.h>
#include <string.h>
#include "hash-private.h"
#include "mymalloc.h"

// public implementations
bool HashTable_Init(HashTable *table, size_t size, bool takeOwnership) {
    if(table==NULL)return false;
    if( table != NULL && size != 0 ) {
        size_t count = sizeof( HashTableNode* ) * size;
        table->buckets = myMalloc( count );
        if(table->buckets == NULL)
        {
            return false;
        }
        table->size = size;
        table->count = 0;
        table->take_ownership = takeOwnership;
        memset( table->buckets, 0, count );
        return true;
    }
    return false;
}

void HashTable_Destruct(HashTable *table) {
    if(table==NULL)return;
    for(size_t i=0;i<table->size;i++){
        HashTableNode * uk=table->buckets[i];
        while(uk!=NULL){
            if(table->take_ownership){
                Data_Destruct(uk->key);
                Data_Destruct(uk->value);
                HashTableNode *mazany=uk;
                uk=uk->next;
                myFree(mazany);
            }
        }
    }
    myFree(table->buckets);
    table->size =table->count=0;

}

bool HashTable_Insert(HashTable *table, Data_t *key, Data_t *value) {
    if(table==NULL)return false;
    if (table && key) {
        unsigned int i = hash(table, key);

        if (!HashTable_Find(table, key)) {
            printf("hash: Key: ");
            puts(Data_To_String(key, NULL));
            printf(" -> Hash index: %u\n", i);
            HashTableNode *node = myMalloc(sizeof(HashTableNode));
            if (node == NULL) {
                return false;
            }
            node->value = value;
            node->key = key;
            node->next = NULL;
            //vlozeni prvku na zacatek pole
            node->next = table->buckets[i];
            table->buckets[i] = node;
            table->count++;
            return true;
        }
    }
    return false;
}

bool HashTable_Replace(HashTable *table, Data_t *key, Data_t *value) {
 if(table==NULL) return false;
    int index=Data_Hash(key);
HashTableNode *uk = table->buckets[index];
while(uk!=NULL){
    if(Data_Cmp(uk->key,key)==0){
        if(table->take_ownership){
            Data_Destruct(uk->value);
        }
        uk->value=value;
        return true;
    }
    uk=uk->next;
}
return false;

}

bool HashTable_Delete(HashTable *table, Data_t *key) {
 if(table==NULL)return false;
 int index=Data_Hash(key);
 HashTableNode *uk = table->buckets[index],*ukpred=NULL;
 while(uk!=NULL){
  if(Data_Cmp(uk->key,key)==0){
      if(ukpred!=NULL){
          ukpred->next = uk->next;
      }else{
          table->buckets[index]=uk->next;
      }
      if(table->take_ownership){
          Data_Destruct(uk->key);
          Data_Destruct(uk->value);
      }
      myFree(uk);
      table->count--;
      return true;
  }
 ukpred=uk;
 uk=uk->next;


 }
  return false;   
}
Data_t *HashTable_Find(HashTable *table, Data_t *key) {
    if (table == NULL) {
        return NULL;
    }
    unsigned int index = hash(table, key);
    //ulozeni prvniho prvku lin liaru
    HashTableNode *node = table->buckets[index];

    //pruchod linseznamu
    while (node) {
        if (Data_Cmp(node->key, key) == 0) {
            return node->value;
        }
        node = node->next;
    }
    //null if prvek no ex
    return NULL;
}

size_t HashTable_Get_Count(HashTable *table) {
 if (table==NULL) return 0;
   else return table->count;
}

void HashTable_Clear(HashTable *table) {
    if(table==NULL)return;
    for(size_t i=0;i<table->size;i++){
        HashTableNode *uk=table->buckets[i];
        while(uk!=NULL){
            if(table->take_ownership){
                Data_Destruct(uk->key);
                Data_Destruct(uk->value);
                HashTableNode *mazany=uk;
                uk=uk->next;
                myFree(mazany);
            }
        }
        table->buckets[i]=NULL;
        table->count=0;
    }
}

void HashTable_Process(HashTable *table, TableNodeProc proc) {
  if(table==NULL)return;
  for(size_t i=0;i<table->size;i++){
      HashTableNode *uk = table->buckets[i];
      while(uk!=NULL){
          proc(uk->key,uk->value);
          uk=uk->next;
      }
  }
}
