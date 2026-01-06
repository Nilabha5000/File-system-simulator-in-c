#include "obj_map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
unsigned long djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}
struct bucket *create_bucket(const char *key , void *value){
    struct bucket*buk = (struct bucket*)malloc(sizeof(struct bucket));
    if(buk == NULL)
      return NULL;
    buk->key = strdup(key);
    if(buk->key == NULL)
    {
        free(buk);
        return NULL;
    }
    buk->value = value;
    buk->next = NULL;
    return buk;
}
struct bucket **rehash(struct obj_table *obj , int new_size){
    if(obj == NULL) return NULL;
    struct bucket **new_table = (struct bucket**)calloc(new_size, sizeof(struct bucket*));

    if(new_table == NULL) return NULL;
    struct bucket **old_table = obj->table;
    int old_size = obj->size;
    int new_count = 0;
    for(int i = 0; i < old_size; ++i){
        
        struct bucket *node = old_table[i];
        while(node){
            struct bucket *next = node->next;
            unsigned long h = djb2(node->key);
            int index = (int)(h % (unsigned long)new_size);
           
            node->next = new_table[index];
            new_table[index] = node;
            new_count++;

            node = next;

        }
        
    }

    free(old_table);
    obj->size = new_size;
    obj->table = new_table;
    obj->count = new_count;
    return obj->table;
}

struct obj_table *create_table(){
    struct obj_table *obj = (struct obj_table*)malloc(sizeof(struct obj_table));
    if(obj == NULL) return NULL;

    obj->count = 0;
    obj->size = obj->min_cap = 5;
    obj->table = (struct bucket**)calloc(obj->size, sizeof(struct bucket*));

    if(obj->table == NULL){
        free(obj);
        return NULL;
    }

    return obj;
}

void *get_obj(struct obj_table *obj, const char *key){
    if(obj == NULL) return NULL;
    unsigned long h = djb2(key);
    int index = (int)(h % (unsigned long)obj->size);

    struct bucket *node = obj->table[index];
    while(node){
        if(strcmp(node->key,key) == 0)
            return node->value;
        node = node->next;
    }
    return NULL;
}
void destroy_obj_table(struct obj_table *obj){
    if(obj == NULL) return;

    if(obj->table == NULL){
        free(obj);
        return;
    }

    for(int i = 0; i < obj->size; ++i){
        struct bucket *node = obj->table[i];
      
        while(node != NULL){
            struct bucket *next = node->next;
            free(node->key);
            free(node);
            node = next;
        }
    }
    free(obj->table);
    free(obj);
}


int insert_obj(struct obj_table *obj , const char *key, void *value){
    if(obj == NULL) return 0;
    double load_factor = (double)obj->count / (double)obj->size;
    if(load_factor > 0.75){
        if(rehash(obj,obj->size*2) == NULL){
              return 0;
        }
    }
   
    struct bucket *buk = create_bucket(key , value);

    if(buk == NULL) return 0;

    unsigned long h = djb2(buk->key);
    int index = (int)(h % (unsigned long)obj->size);
    if(obj->table[index] == NULL){
        obj->table[index] = buk;
        obj->count++;
        return 1;
    }

    buk->next = obj->table[index];
    obj->table[index] = buk;
    obj->count++; 
    return 1;
}
void* del_obj(struct obj_table *obj, const char *key){
      if(obj == NULL) return NULL;

    unsigned long h = djb2(key);
    int index = (int)(h % (unsigned long)obj->size);

    struct bucket *curr = obj->table[index];
    struct bucket *prev = NULL;

    while(curr != NULL){
        if(strcmp(key, curr->key) == 0)
            break;

        prev = curr;
        curr = curr->next;
    }

    if(curr == NULL)
        return NULL;

    void *result = curr->value;

    // unlink
    if(prev == NULL)
        obj->table[index] = curr->next;
    else
        prev->next = curr->next;

    // free bucket
    free(curr->key);
    free(curr);

    obj->count--;
    // shrink if below threshold
    double load_factor = (double)obj->count / (double)obj->size;

    if((obj->size / 2) >= obj->min_cap && load_factor < 0.25){
       rehash(obj, obj->size / 2);
    }

    return result;
}
