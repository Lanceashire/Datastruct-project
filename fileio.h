#ifndef FILEIO_H
#define FILEIO_H

#include "common.h"

int LoadData(BookList *bL, ReaderList *rL, BorrowList *brL);
int SaveData(BookList *bL, ReaderList *rL, BorrowList *brL);

#endif /* FILEIO_H */
