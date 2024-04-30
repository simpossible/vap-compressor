//
//  dictionary.c
//  vap-server
//
//  Created by 梁金锋 on 2024/4/27.
//

#include "dictionary.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hashmap.h"
#include <pthread.h>


typedef struct   {
   struct hashmap *map;
    pthread_mutex_t lock;
} __Dictionary;


typedef struct DictionaryNode{
    char *key;
    void *obj;
} DictionaryNode;


static int node_compare(const void *a, const void *b, void *udata) {
    const struct DictionaryNode *ua = a;
    const struct DictionaryNode *ub = b;
    return strcmp(ua->key, ub->key);
}


static uint64_t node_hash(const void *item, uint64_t seed0, uint64_t seed1) {
    const struct DictionaryNode *node = item;
    return hashmap_sip(node->key, strlen(node->key), seed0, seed1);
}

Dictionary *dicCreate(void) {
    __Dictionary *_dic = malloc(sizeof(__Dictionary));
    memset(_dic, 0, sizeof(__Dictionary));
    if (_dic->map == NULL) {
        _dic->map = hashmap_new(sizeof(struct DictionaryNode), 0, 0, 0,
                              node_hash, node_compare, NULL, NULL);
        pthread_mutex_init(&_dic->lock, NULL);
    }
    return (Dictionary *)_dic;
}


void dicSetValue(Dictionary*dic, char *key, void *value) {
    __Dictionary *_dic = (__Dictionary *)dic;
    
    pthread_mutex_lock(&_dic->lock);
    if (_dic->map == NULL) {
        pthread_mutex_unlock(&_dic->lock);
        return;
    }
    struct DictionaryNode *node = malloc(sizeof(struct DictionaryNode));
    char *nodekey = malloc(strlen(key)+1);
    strcpy(nodekey, key);
    node->obj = value;
    node->key = nodekey;
    hashmap_set(_dic->map, node);
    pthread_mutex_unlock(&_dic->lock);
}

void* dicGetValue(Dictionary*dic, char *key) {
    __Dictionary *_dic = (__Dictionary *)dic;
    pthread_mutex_lock(&_dic->lock);
    if (_dic->map == NULL) {
        pthread_mutex_unlock(&_dic->lock);
        return NULL;
    }   
    struct DictionaryNode *node = (DictionaryNode *)hashmap_get(_dic->map, &(struct DictionaryNode){.key=key});
    pthread_mutex_unlock(&_dic->lock);
    if (node == NULL) {
        
        return NULL;
    }
    return node->obj;
}


void dicRemoveValueForKey(Dictionary*dic, char *key) {
    __Dictionary *_dic = (__Dictionary *)dic;
    pthread_mutex_lock(&_dic->lock);
    if (_dic->map == NULL) {
        pthread_mutex_unlock(&_dic->lock);
        return;
    }
    struct DictionaryNode *node = (DictionaryNode *)hashmap_get(_dic->map, &(struct DictionaryNode){.key=key});
    if (node != NULL) {
        hashmap_delete(_dic->map, &(struct DictionaryNode){.key=key});
        if (node->obj == NULL) {
            free(node->obj);
            free(node->key);
        }
        free(node);
        
    }
    pthread_mutex_unlock(&_dic->lock);
   
    
}


