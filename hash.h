#ifndef HASH_H
#define HASH_H

#include "common.h"

void RebuildBookHash(BookList *bL);
void RebuildCopyHash(BookCopyList *cL);
void RebuildReaderHash(ReaderList *rL);
void RebuildBorrowHash(BorrowList *brL);

int FindCatalogIndexById(int catalogId);
int FindCatalogIndexByIsbn(const char *isbn);
int FindCopyIndex(int copyId);
int FindReaderIndex(int readerId);
int FindBorrowIndex(int borrowId);

#endif
