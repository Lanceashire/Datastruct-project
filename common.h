#ifndef COMMON_H
#define COMMON_H

/* ============================================================
   common.h  —  公共头文件
   作者负责人：吴宣慧
   说明：定义系统全部数据结构和函数声明，所有 .c 文件均 include 此文件
   ============================================================ */

#define MAX_BOOK   200
#define MAX_READER 200
#define MAX_BORROW 300

/* -------------------- 图书结构体 -------------------- */
typedef struct {
    int  bookId;
    char bookName[50];
    char author[30];
    char category[20];
    int  stock;
    int  borrowCount;
} Book;

typedef struct {
    Book data[MAX_BOOK];
    int  length;
} BookList;

/* -------------------- 读者结构体 -------------------- */
typedef struct {
    int  readerId;
    char readerName[20];
    char gender[5];
    char phone[15];
    int  role;    /* 1=管理员  2=教师  3=学生 */
} Reader;

typedef struct {
    Reader data[MAX_READER];
    int    length;
} ReaderList;

/* -------------------- 借阅记录结构体 -------------------- */
typedef struct {
    int  borrowId;
    int  bookId;
    int  readerId;
    char borrowTime[15];  /* 格式 YYYY-MM-DD */
    char returnTime[15];  /* 未归还时填 "未归还" */
    char deadline[15];    /* 格式 YYYY-MM-DD */
} Borrow;

typedef struct {
    Borrow data[MAX_BORROW];
    int    length;
} BorrowList;

/* ============================================================
   函数声明（各模块实现，集中在此声明方便互相调用）
   ============================================================ */

/* ---------- fileio.c ---------- */
int LoadData(BookList *bL, ReaderList *rL, BorrowList *brL);
int SaveData(BookList *bL, ReaderList *rL, BorrowList *brL);

/* ---------- book.c ---------- */
void  AddBook(BookList *L, Book b);
int   DelBook(BookList *L, int bookId);
int   ModBook(BookList *L, int bookId, Book newBook);
Book *SearchBook(BookList *L, int bookId);
void  SearchBookByName(BookList *L, char *name);
void  ShowAllBook(BookList *L);
void  UpdateStock(BookList *L, int bookId, int num);
void  UpdateBorrowCount(BookList *L, int bookId);
void  BookManage(BookList *L);

/* ---------- reader.c ---------- */
void    AddReader(ReaderList *L, Reader r);
int     DelReader(ReaderList *L, int readerId);
int     ModReader(ReaderList *L, int readerId, Reader newReader);
Reader *SearchReader(ReaderList *L, int readerId);
void    ShowAllReader(ReaderList *L);
void    ReaderManage(ReaderList *L);

/* ---------- borrow.c ---------- */
void AddBorrow(BorrowList *L, BookList *bL, Borrow br);
int  ReturnBook(BorrowList *L, BookList *bL, int borrowId, char *retTime);
void SearchBorrowByBook(BorrowList *L, int bookId);
void SearchBorrowByReader(BorrowList *L, int readerId);
void BorrowRank(BookList *bL);
void CheckRemind(BorrowList *L);
void BorrowManage(BookList *bL, ReaderList *rL, BorrowList *brL, int role);

/* ---------- auth.c ---------- */
int Login(void);

/* ---------- menu.c ---------- */
void MainMenu(int role, BookList *bL, ReaderList *rL, BorrowList *brL);

/* ---------- fileedit.c ---------- */
void FileEditMenu(void);
void CreateFile(char *filename);
void OpenAndShowFile(char *filename);
void InsertLine(char *filename, int lineNo, char *content);
void DeleteLine(char *filename, int lineNo);
void ReplaceLine(char *filename, int lineNo, char *content);
void SearchInFile(char *filename, char *keyword);

#endif /* COMMON_H */
