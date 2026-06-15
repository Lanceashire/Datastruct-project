#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileio.h"
#include "hash.h"

#define BOOK_FILE   "book.txt"
#define COPY_FILE   "copy.txt"
#define READER_FILE "reader.txt"
#define BORROW_FILE "borrow.txt"

static void trim_end(char *s) {
    int len = (int)strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r' || s[len - 1] == ' '))
        s[--len] = '\0';
}

static FILE *open_or_create(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp) return fp;
    fp = fopen(filename, "w");
    if (!fp) return NULL;
    fclose(fp);
    return fopen(filename, "r");
}

static char *next_field(char **cursor) {
    return strtok(*cursor ? *cursor : NULL, ",");
}

int LoadData(BookList *bL, BookCopyList *cL, ReaderList *rL, BorrowList *brL) {
    FILE *fp;
    char line[400], buf[400], *cursor, *tok;

    fp = open_or_create(BOOK_FILE);
    if (!fp) return 0;
    while (fgets(line, sizeof(line), fp) && bL->length < MAX_BOOK) {
        Book b = {0};
        trim_end(line);
        if (!line[0]) continue;
        strcpy(buf, line); cursor = buf;
        tok = next_field(&cursor); if (!tok) continue; b.catalogId = atoi(tok); cursor = NULL;
        tok = next_field(&cursor); if (!tok) continue; strncpy(b.isbn, tok, 19);
        tok = next_field(&cursor); if (!tok) continue; strncpy(b.bookName, tok, 49);
        tok = next_field(&cursor); if (!tok) continue; strncpy(b.author, tok, 29);
        tok = next_field(&cursor); if (!tok) continue; strncpy(b.publisher, tok, 39);
        tok = next_field(&cursor); if (!tok) continue; strncpy(b.category, tok, 19);
        tok = next_field(&cursor); if (!tok) continue; b.borrowCount = atoi(tok);
        bL->data[bL->length++] = b;
    }
    fclose(fp);

    fp = open_or_create(COPY_FILE);
    if (!fp) return 0;
    while (fgets(line, sizeof(line), fp) && cL->length < MAX_COPY) {
        BookCopy copy = {0};
        trim_end(line);
        if (!line[0]) continue;
        strcpy(buf, line); cursor = buf;
        tok = next_field(&cursor); if (!tok) continue; copy.copyId = atoi(tok); cursor = NULL;
        tok = next_field(&cursor); if (!tok) continue; copy.catalogId = atoi(tok);
        tok = next_field(&cursor); if (!tok) continue; copy.status = atoi(tok);
        tok = next_field(&cursor); if (!tok) continue; strncpy(copy.location, tok, 29);
        cL->data[cL->length++] = copy;
    }
    fclose(fp);

    fp = open_or_create(READER_FILE);
    if (!fp) return 0;
    while (fgets(line, sizeof(line), fp) && rL->length < MAX_READER) {
        Reader r = {0};
        trim_end(line);
        if (!line[0]) continue;
        strcpy(buf, line); cursor = buf;
        tok = next_field(&cursor); if (!tok) continue; r.readerId = atoi(tok); cursor = NULL;
        tok = next_field(&cursor); if (!tok) continue; strncpy(r.readerName, tok, 19);
        tok = next_field(&cursor); if (!tok) continue; strncpy(r.gender, tok, 4);
        tok = next_field(&cursor); if (!tok) continue; strncpy(r.phone, tok, 14);
        tok = next_field(&cursor); if (!tok) continue; r.role = atoi(tok);
        rL->data[rL->length++] = r;
    }
    fclose(fp);

    fp = open_or_create(BORROW_FILE);
    if (!fp) return 0;
    while (fgets(line, sizeof(line), fp) && brL->length < MAX_BORROW) {
        Borrow br = {0};
        trim_end(line);
        if (!line[0]) continue;
        strcpy(buf, line); cursor = buf;
        tok = next_field(&cursor); if (!tok) continue; br.borrowId = atoi(tok); cursor = NULL;
        tok = next_field(&cursor); if (!tok) continue; br.copyId = atoi(tok);
        tok = next_field(&cursor); if (!tok) continue; br.readerId = atoi(tok);
        tok = next_field(&cursor); if (!tok) continue; strncpy(br.borrowTime, tok, 14);
        tok = next_field(&cursor); if (!tok) continue; strncpy(br.returnTime, tok, 14);
        tok = next_field(&cursor); if (!tok) continue; strncpy(br.deadline, tok, 14);
        brL->data[brL->length++] = br;
    }
    fclose(fp);

    RebuildBookHash(bL);
    RebuildCopyHash(cL);
    RebuildReaderHash(rL);
    RebuildBorrowHash(brL);
    return 1;
}

int SaveData(BookList *bL, BookCopyList *cL, ReaderList *rL, BorrowList *brL) {
    FILE *fp;
    int i;

    fp = fopen(BOOK_FILE, "w");
    if (!fp) return 0;
    for (i = 0; i < bL->length; i++) {
        Book *b = &bL->data[i];
        fprintf(fp, "%d,%s,%s,%s,%s,%s,%d\n", b->catalogId, b->isbn,
                b->bookName, b->author, b->publisher, b->category, b->borrowCount);
    }
    fclose(fp);

    fp = fopen(COPY_FILE, "w");
    if (!fp) return 0;
    for (i = 0; i < cL->length; i++) {
        BookCopy *copy = &cL->data[i];
        fprintf(fp, "%d,%d,%d,%s\n", copy->copyId, copy->catalogId,
                copy->status, copy->location);
    }
    fclose(fp);

    fp = fopen(READER_FILE, "w");
    if (!fp) return 0;
    for (i = 0; i < rL->length; i++) {
        Reader *r = &rL->data[i];
        fprintf(fp, "%d,%s,%s,%s,%d\n", r->readerId, r->readerName,
                r->gender, r->phone, r->role);
    }
    fclose(fp);

    fp = fopen(BORROW_FILE, "w");
    if (!fp) return 0;
    for (i = 0; i < brL->length; i++) {
        Borrow *br = &brL->data[i];
        fprintf(fp, "%d,%d,%d,%s,%s,%s\n", br->borrowId, br->copyId,
                br->readerId, br->borrowTime, br->returnTime, br->deadline);
    }
    fclose(fp);
    return 1;
}
