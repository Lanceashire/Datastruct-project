#ifndef BORROW_H
#define BORROW_H

#include "common.h"

void AddBorrow(BorrowList *L, BookList *bL, Borrow br);
int  ReturnBook(BorrowList *L, BookList *bL, int borrowId, char *retTime);
void SearchBorrowByBook(BorrowList *L, int bookId);
void SearchBorrowByReader(BorrowList *L, int readerId);
void BorrowRank(BookList *bL);
void CheckRemind(BorrowList *L);
void BorrowManage(BookList *bL, ReaderList *rL, BorrowList *brL, int role);

#endif
