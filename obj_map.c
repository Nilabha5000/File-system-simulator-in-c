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
void destroy_bucket(struct bucket *buk){
    free(buk->key);
    free(buk);
}
struct bucket_list *create_bucket_list(){
    struct bucket_list *buck_list = (struct bucket_list*)malloc(sizeof(struct bucket_list));
    if(buck_list == NULL){
       return NULL;
    }
    buck_list->next = NULL;
    buck_list->start = NULL;
    buck_list->prev = NULL;
    
    return buck_list;
}
struct bucket_list **rehash(struct obj_table *obj , int new_size){
    if(obj == NULL) return NULL;
    struct bucket_list **new_table = (struct bucket_list**)calloc(new_size, sizeof(struct bucket_list*));

    if(new_table == NULL) return NULL;
    struct bucket_list **old_table = obj->table;
    int new_count = 0;
    struct bucket_list *new_begin = NULL;
    struct bucket_list *new_end = NULL;
    struct bucket_list *head = obj->begin;
    while(head != NULL){
        
        struct bucket *node = head->start;
        while(node){
            struct bucket *next = node->next;
            unsigned long h = djb2(node->key);
            int index = (int)(h % (unsigned long)new_size);
            if(new_table[index] == NULL){
                new_table[index] = create_bucket_list();
                if(new_begin == NULL){
                    new_begin = new_end = new_table[index];
                }
                else{
                   new_end->next = new_table[index];
                   new_table[index]->prev = new_end;
                   new_end = new_table[index];
                }
            }
        
            node->next = new_table[index]->start;
            new_table[index]->start = node;
            new_count++;
            
            node = next;

        }

        if(head->next == NULL){
            free(head);
            head = NULL;
        }
        else{
            head = head->next;
            free(head->prev);
        }
        
    }

    free(old_table);
    obj->size = new_size;
    obj->table = new_table;
    obj->count = new_count;
    obj->begin = new_begin;
    obj->end = new_end;
    return obj->table;
}

struct obj_table *create_table(){
    struct obj_table *obj = (struct obj_table*)malloc(sizeof(struct obj_table));
    if(obj == NULL) return NULL;

    obj->count = 0;
    obj->size = obj->min_cap = 5;
    obj->table = (struct bucket_list**)calloc(obj->size, sizeof(struct bucket_list*));
    obj->begin = obj->end = NULL;
    if(obj->table == NULL){
        free(obj);
        return NULL;
    }

    return obj;
}
int empty(struct obj_table *obj){
    
    return (obj->begin == NULL);
}
void *get_obj(struct obj_table *obj, const char *key){
    if(obj == NULL) return NULL;
    unsigned long h = djb2(key);
    int index = (int)(h % (unsigned long)obj->size);
    if(obj->table[index] == NULL)
        return NULL;
    struct bucket *node = obj->table[index]->start;
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
    
    struct bucket_list *head = obj->begin;
    while(head != NULL){
        struct bucket *node = head->start;
      
        while(node != NULL){
            struct bucket *next = node->next;
            destroy_bucket(node);
            node = next;
        }
        
        if(head->next == NULL)
        {
            free(head);
            head = NULL;
        }
        else{
            head = head->next;
            free(head->prev);
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
        struct bucket_list* buk_list = create_bucket_list();
        if(buk_list == NULL){
            destroy_bucket(buk);
            return 0;
        }
        if(empty(obj)){
             obj->begin = obj->end = buk_list;
        } 
        else{
            obj->end->next = buk_list;
            buk_list->prev = obj->end;
            obj->end = buk_list;
        }
        buk_list->start = buk;
        obj->table[index] = buk_list;

        obj->count++;
        return 1;
    }
    
    buk->next = obj->table[index]->start;
    obj->table[index]->start = buk;
    obj->count++; 
    return 1;
}
void* del_obj(struct obj_table *obj, const char *key){
      if(obj == NULL) return NULL;

    unsigned long h = djb2(key);
    int index = (int)(h % (unsigned long)obj->size);
    if(obj->table[index] == NULL){
         return NULL;
    }
    struct bucket *curr = obj->table[index]->start;
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
        obj->table[index]->start = curr->next;
    else
        prev->next = curr->next;

    // free bucket
    destroy_bucket(curr);
    // check if all the buckets of the buket_list are deleted then free that bucket_list.
    if(obj->table[index]->start == NULL){

        struct bucket_list *buk_list = obj->table[index];
        //checking buck_list node is not the first node.
        if(buk_list->prev){
          buk_list->prev->next = buk_list->next;
        }
        else{
           obj->begin = buk_list->next;
        }

       if(buk_list->next){
          buk_list->next->prev = buk_list->prev;
       }
       else{
        obj->end = buk_list->prev;
       }
        //freeing the bucket list.
        free(buk_list);
        obj->table[index] = NULL;
    }
    obj->count--;
    // shrink if below threshold
    double load_factor = (double)obj->count / (double)obj->size;

    if((obj->size / 2) >= obj->min_cap && load_factor < 0.25){
       rehash(obj, obj->size / 2);
    }

    return result;
}
