#include <stdio.h>
#include <string.h>
#include "hash.h"

#define HASH_SIZE 2003
#define KEY_SIZE  32

typedef struct {
    char key[KEY_SIZE];
    int  value;
    int  used;
} HashSlot;

typedef struct {
    HashSlot slots[HASH_SIZE];
} HashTable;

static HashTable catalogIdHash;
static HashTable isbnHash;
static HashTable copyIdHash;
static HashTable readerIdHash;
static HashTable borrowIdHash;

static unsigned long hash_string(const char *text) {
    unsigned long hash = 5381;
    unsigned char c;
    while ((c = (unsigned char)*text++) != 0)
        hash = ((hash << 5) + hash) + c;
    return hash % HASH_SIZE;
}

static void clear_hash(HashTable *table) {
    memset(table, 0, sizeof(*table));
}

static int put_hash(HashTable *table, const char *key, int value) {
    unsigned long start = hash_string(key);
    unsigned long i;
    for (i = 0; i < HASH_SIZE; i++) {
        HashSlot *slot = &table->slots[(start + i) % HASH_SIZE];
        if (!slot->used || strcmp(slot->key, key) == 0) {
            strncpy(slot->key, key, KEY_SIZE - 1);
            slot->key[KEY_SIZE - 1] = '\0';
            slot->value = value;
            slot->used = 1;
            return 1;
        }
    }
    return 0;
}

static int get_hash(HashTable *table, const char *key) {
    unsigned long start = hash_string(key);
    unsigned long i;
    for (i = 0; i < HASH_SIZE; i++) {
        HashSlot *slot = &table->slots[(start + i) % HASH_SIZE];
        if (!slot->used) return -1;
        if (strcmp(slot->key, key) == 0) return slot->value;
    }
    return -1;
}

static void int_key(int value, char key[KEY_SIZE]) {
    snprintf(key, KEY_SIZE, "%d", value);
}

void RebuildBookHash(BookList *bL) {
    int i;
    char key[KEY_SIZE];
    clear_hash(&catalogIdHash);
    clear_hash(&isbnHash);
    for (i = 0; i < bL->length; i++) {
        int_key(bL->data[i].catalogId, key);
        put_hash(&catalogIdHash, key, i);
        put_hash(&isbnHash, bL->data[i].isbn, i);
    }
}

void RebuildCopyHash(BookCopyList *cL) {
    int i;
    char key[KEY_SIZE];
    clear_hash(&copyIdHash);
    for (i = 0; i < cL->length; i++) {
        int_key(cL->data[i].copyId, key);
        put_hash(&copyIdHash, key, i);
    }
}

void RebuildReaderHash(ReaderList *rL) {
    int i;
    char key[KEY_SIZE];
    clear_hash(&readerIdHash);
    for (i = 0; i < rL->length; i++) {
        int_key(rL->data[i].readerId, key);
        put_hash(&readerIdHash, key, i);
    }
}

void RebuildBorrowHash(BorrowList *brL) {
    int i;
    char key[KEY_SIZE];
    clear_hash(&borrowIdHash);
    for (i = 0; i < brL->length; i++) {
        int_key(brL->data[i].borrowId, key);
        put_hash(&borrowIdHash, key, i);
    }
}

int FindCatalogIndexById(int catalogId) {
    char key[KEY_SIZE];
    int_key(catalogId, key);
    return get_hash(&catalogIdHash, key);
}

int FindCatalogIndexByIsbn(const char *isbn) {
    return get_hash(&isbnHash, isbn);
}

int FindCopyIndex(int copyId) {
    char key[KEY_SIZE];
    int_key(copyId, key);
    return get_hash(&copyIdHash, key);
}

int FindReaderIndex(int readerId) {
    char key[KEY_SIZE];
    int_key(readerId, key);
    return get_hash(&readerIdHash, key);
}

int FindBorrowIndex(int borrowId) {
    char key[KEY_SIZE];
    int_key(borrowId, key);
    return get_hash(&borrowIdHash, key);
}
