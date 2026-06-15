#ifndef BOOK_H
#define BOOK_H

#include "common.h"

void  AddBook(BookList *L, Book b);
int   DelBook(BookList *L, int bookId);
int   ModBook(BookList *L, int bookId, Book newBook);
Book *SearchBook(BookList *L, int bookId);
void  SearchBookByName(BookList *L, char *name);
void  ShowAllBook(BookList *L);
void  UpdateStock(BookList *L, int bookId, int num);
void  UpdateBorrowCount(BookList *L, int bookId);
void  BookManage(BookList *L);

#endif
