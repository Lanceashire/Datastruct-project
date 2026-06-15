#ifndef READER_H
#define READER_H

#include "common.h"

void    AddReader(ReaderList *L, Reader r);
int     DelReader(ReaderList *L, int readerId);
int     ModReader(ReaderList *L, int readerId, Reader newReader);
Reader *SearchReader(ReaderList *L, int readerId);
void    ShowAllReader(ReaderList *L);
void    ReaderManage(ReaderList *L);

#endif
