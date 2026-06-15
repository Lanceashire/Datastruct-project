#ifndef COMMON_H
#define COMMON_H

#define MAX_BOOK    200
#define MAX_COPY    1000
#define MAX_READER  200
#define MAX_BORROW  500

#define COPY_AVAILABLE 1
#define COPY_BORROWED  2
#define COPY_DAMAGED   3
#define COPY_LOST      4

/* 一条 Book 表示一种书（书目），ISBN 用于识别相同版本。 */
typedef struct {
    int  catalogId;
    char isbn[20];
    char bookName[50];
    char author[30];
    char publisher[40];
    char category[20];
    int  borrowCount;
} Book;

typedef struct {
    Book data[MAX_BOOK];
    int  length;
} BookList;

/* 一条 BookCopy 表示图书馆里一本可被扫码、借出的实体书。 */
typedef struct {
    int  copyId;
    int  catalogId;
    int  status;
    char location[30];
} BookCopy;

typedef struct {
    BookCopy data[MAX_COPY];
    int      length;
} BookCopyList;

typedef struct {
    int  readerId;
    char readerName[20];
    char gender[5];
    char phone[15];
    int  role;
} Reader;

typedef struct {
    Reader data[MAX_READER];
    int    length;
} ReaderList;

/* 借阅记录关联具体 copyId，而不是只关联书目。 */
typedef struct {
    int  borrowId;
    int  copyId;
    int  readerId;
    char borrowTime[15];
    char returnTime[15];
    char deadline[15];
} Borrow;

typedef struct {
    Borrow data[MAX_BORROW];
    int    length;
} BorrowList;

int LoadData(BookList *bL, BookCopyList *cL, ReaderList *rL, BorrowList *brL);
int SaveData(BookList *bL, BookCopyList *cL, ReaderList *rL, BorrowList *brL);

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

void    AddReader(ReaderList *L, Reader r);
int     DelReader(ReaderList *L, int readerId);
int     ModReader(ReaderList *L, int readerId, Reader newReader);
Reader *SearchReader(ReaderList *L, int readerId);
void    ShowAllReader(ReaderList *L);
void    ReaderManage(ReaderList *L);

void AddBorrow(BorrowList *L, BookList *bL, BookCopyList *cL, Borrow br);
int  ReturnBook(BorrowList *L, BookCopyList *cL, int borrowId, char *retTime);
void SearchBorrowByCatalog(BorrowList *L, BookCopyList *cL, int catalogId);
void SearchBorrowByReader(BorrowList *L, int readerId);
void BorrowRank(BookList *bL, BookCopyList *cL);
void CheckRemind(BorrowList *L);
void BorrowManage(BookList *bL, BookCopyList *cL, ReaderList *rL,
                  BorrowList *brL, int role);

int Login(void);
void MainMenu(int role, BookList *bL, BookCopyList *cL,
              ReaderList *rL, BorrowList *brL);

void FileEditMenu(void);
void CreateFile(char *filename);
void OpenAndShowFile(char *filename);
void InsertLine(char *filename, int lineNo, char *content);
void DeleteLine(char *filename, int lineNo);
void ReplaceLine(char *filename, int lineNo, char *content);
void SearchInFile(char *filename, char *keyword);

#endif
