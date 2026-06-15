#ifndef BORROW_H
#define BORROW_H

#include "common.h"

void AddBorrow(BorrowList *L, BookList *bL, BookCopyList *cL, Borrow br);
int  ReturnBook(BorrowList *L, BookCopyList *cL, int borrowId, char *retTime);
void SearchBorrowByCatalog(BorrowList *L, BookCopyList *cL, int catalogId);
void SearchBorrowByReader(BorrowList *L, int readerId);
void BorrowRank(BookList *bL, BookCopyList *cL);
void CheckRemind(BorrowList *L);
void BorrowManage(BookList *bL, BookCopyList *cL, ReaderList *rL,
                  BorrowList *brL, int role);

#endif
