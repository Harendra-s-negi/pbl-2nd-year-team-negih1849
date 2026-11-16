#ifndef HEAP_H
#define HEAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    char id[16];
    char token[16];
    char name[64];
    int age;
    int urgencyScore;
    int waitMinutes;
    int effectiveScore;
} C_PatientRecord;

typedef struct {
    C_PatientRecord* data;
    int size;
    int capacity;
} C_MaxHeap;

C_MaxHeap* heap_create(int initial_capacity);
bool heap_insert(C_MaxHeap* heap, C_PatientRecord record);
bool heap_extract_max(C_MaxHeap* heap, C_PatientRecord* out_record);
bool heap_peek(const C_MaxHeap* heap, C_PatientRecord* out_record);
void heap_destroy(C_MaxHeap* heap);
void heap_print(const C_MaxHeap* heap);

#ifdef __cplusplus
}
#endif

#endif // HEAP_H

