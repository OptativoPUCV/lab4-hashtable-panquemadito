#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}


void insertMap(HashMap * map, char * key, void * value) {
  if (map == NULL || key == NULL)  {
    return;
  }

  long position = hash(key,map->capacity) ;
  int rehash_count = 0;
  
  while (map->buckets[position] != NULL && rehash_count < map->capacity) {
    if (is_equal(map->buckets[position]->key, key)) {
      free (map->buckets[position]->key);
      map->buckets[position]->key = strdup(key) ;
      map->buckets[position]->value = value ;
      return;
    }
    position = (position + 1) % map->capacity;
    rehash_count++;
  }
  if (rehash_count == map->capacity) {
    enlarge(map) ;
    insertMap(map,key,value) ;
  } else {

    map->buckets[position] = createPair(strdup(key), value) ;
    map->size++ ;
    map->current = position ;
  }
}

void enlarge(HashMap * map) {
  if (map == NULL) return;
  enlarge_called = 1; //no borrar (testing purposes)
  
  long old_capacity = map->capacity ;
  map->capacity *=2;
  Pair **old_buckets = map->buckets;
  map->buckets = (Pair ** )calloc(map->capacity, sizeof(Pair *));
  if (map->buckets == NULL) {
    map->buckets = old_buckets ;
    map->capacity = old_capacity;
    return;
  }

  for (long i = 0; i < old_capacity; i++) {
    if (old_buckets[i] != NULL && old_buckets[i]->key != NULL) {
      insertMap(map,old_buckets[i]->key, old_buckets[i]->value);
      free(old_buckets[i]) ;
    }
  }
  free(old_buckets) ;
}


HashMap * createMap(long capacity) {
  if (capacity <= 0) {
    return NULL;
  }

  HashMap * map = (HashMap *)malloc(sizeof(HashMap));
  if (map == NULL) {
    return NULL;
  }
  map->capacity = capacity;
  map->size = 0;
  map->current = -1 ;

  map->buckets = (Pair **)calloc(map->capacity, sizeof(Pair *));
  if (map->buckets == NULL) {
    free(map);
    return NULL;
  }
  for (int i = 0 ; i < capacity ; i++) {
    map->buckets[i] = NULL;
  }
  return map;
}

void eraseMap(HashMap * map,  char * key) {    
  if (map == NULL || key == NULL) {
    return;
  }

  long position = hash(key,map->capacity) ;
  while (map->buckets[position] != NULL ) {
    if (is_equal(map->buckets[position]->key,key)) {
      free(map->buckets[position]->key) ;
      map->buckets[position]->key = NULL;
      map->size--;
      map->current = position;
      return;
    }
    position = (position + 1) % map->capacity;
  }
}

Pair * searchMap(HashMap * map,  char * key) { 
  if (map == NULL || key == NULL) {
    return NULL;
  }
  long position = hash(key,map->capacity) ;

  while (map->buckets[position] != NULL ) {
    if (is_equal(map->buckets[position]->key, key)) {
      map->current = position ;

      return map->buckets[position] ;
    }
    position = (position + 1) % map->capacity;
  }
  map->current = position;
    return NULL;
}

Pair * firstMap(HashMap * map) {
  if (map == NULL) return NULL;
  for (long i = 0; i < map->capacity; i++) {
    if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
      map->current = i;
      return map->buckets[i] ;
    }
  }
  map->current = -1;
  return NULL;
}

Pair * nextMap(HashMap * map) {
  if (map == NULL || map->current == -1) return NULL;

  long start = map->current;
  do {
    map->current = (map->current + 1) % map->capacity;
    if(map->buckets[map->current] != NULL && map->buckets[map->current]->key !=NULL) {
       return map->buckets[map->current] ;
    }
  } while (map->current != start); 
  return NULL;
}
