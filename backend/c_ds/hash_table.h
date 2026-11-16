#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "heap.h"
#include <stdbool.h>

#define TABLE_SIZE 31

typedef struct C_HashEntry {
    char key[16];
    C_PatientRecord value;
    struct C_HashEntry* next;
} C_HashEntry;

typedef struct {
    C_HashEntry* buckets[TABLE_SIZE];
    int count;
} C_HashTable;

C_HashTable* hash_table_create(void);
bool hash_table_put(C_HashTable* table, const char* token, C_PatientRecord record);
bool hash_table_get(const C_HashTable* table, const char* token, C_PatientRecord* out_record);
bool hash_table_remove(C_HashTable* table, const char* token);
void hash_table_destroy(C_HashTable* table);
void hash_table_print(const C_HashTable* table);

#ifdef __cplusplus
}
#endif

#endif // HASH_TABLE_H

