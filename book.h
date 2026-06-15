#ifndef BOOK_H
#define BOOK_H

#include "common.h"

void  AddBook(BookList *L, Book b);
int   DelBook(BookList *L, BookCopyList *cL, int catalogId);
int   ModBook(BookList *L, int catalogId, Book newBook);
Book *SearchBook(BookList *L, int catalogId);
Book *SearchBookByIsbn(BookList *L, const char *isbn);
void  SearchBookByName(BookList *L, BookCopyList *cL, char *name);
void  ShowAllBook(BookList *L, BookCopyList *cL);
void  AddCopy(BookCopyList *cL, BookList *bL, BookCopy copy);
int   DelCopy(BookCopyList *cL, int copyId);
int   SetCopyStatus(BookCopyList *cL, int copyId, int status);
BookCopy *SearchCopy(BookCopyList *cL, int copyId);
void  ShowCopiesByCatalog(BookCopyList *cL, int catalogId);
int   CountCopies(BookCopyList *cL, int catalogId, int status);
void  UpdateBorrowCount(BookList *L, int catalogId);
void  BookManage(BookList *bL, BookCopyList *cL);

#endif
