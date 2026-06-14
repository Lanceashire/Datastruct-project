#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileio.h"

/* ============================================================
   fileio.c  —  数据持久化模块
   作者负责人：吴宣慧
   说明：负责程序启动时从 txt 文件加载数据到内存顺序表，
         程序退出时将内存数据写回 txt 文件。
   文件格式：每行一条记录，字段用英文逗号分隔
     book.txt   : bookId,bookName,author,category,stock,borrowCount
     reader.txt : readerId,readerName,gender,phone,role
     borrow.txt : borrowId,bookId,readerId,borrowTime,returnTime,deadline
   ============================================================ */

#define BOOK_FILE   "book.txt"
#define READER_FILE "reader.txt"
#define BORROW_FILE "borrow.txt"

/* 去除字符串末尾的换行符和空格 */
static void trim_end(char *s) {
    int len = (int)strlen(s);
    while (len > 0 && (s[len-1] == '\n' || s[len-1] == '\r' || s[len-1] == ' '))
        s[--len] = '\0';
}

/* ----------------------------------------------------------------
   LoadData：从三个 txt 文件读取数据到内存顺序表
   - 文件不存在时自动创建空文件，不报错
   - 读取成功返回 1
   ---------------------------------------------------------------- */
int LoadData(BookList *bL, ReaderList *rL, BorrowList *brL) {
    FILE *fp;
    char  line[300];
    char  buf[300];
    char *tok;

    /* ---------- 加载图书数据 ---------- */
    fp = fopen(BOOK_FILE, "r");
    if (fp == NULL) {
        /* 文件不存在，自动创建空文件 */
        fp = fopen(BOOK_FILE, "w");
        if (fp) fclose(fp);
    } else {
        while (fgets(line, sizeof(line), fp)) {
            trim_end(line);
            if (strlen(line) == 0) continue;

            /* 用 strtok 按逗号拆分字段，顺序：
               bookId, bookName, author, category, stock, borrowCount */
            Book b;
            strcpy(buf, line);

            tok = strtok(buf, ",");  if (!tok) continue;
            b.bookId = atoi(tok);

            tok = strtok(NULL, ","); if (!tok) continue;
            strncpy(b.bookName, tok, 49);  b.bookName[49] = '\0';

            tok = strtok(NULL, ","); if (!tok) continue;
            strncpy(b.author, tok, 29);    b.author[29] = '\0';

            tok = strtok(NULL, ","); if (!tok) continue;
            strncpy(b.category, tok, 19);  b.category[19] = '\0';

            tok = strtok(NULL, ","); if (!tok) continue;
            b.stock = atoi(tok);

            tok = strtok(NULL, ","); if (!tok) continue;
            b.borrowCount = atoi(tok);

            if (bL->length < MAX_BOOK)
                bL->data[bL->length++] = b;
        }
        fclose(fp);
    }

    /* ---------- 加载读者数据 ---------- */
    fp = fopen(READER_FILE, "r");
    if (fp == NULL) {
        fp = fopen(READER_FILE, "w");
        if (fp) fclose(fp);
    } else {
        while (fgets(line, sizeof(line), fp)) {
            trim_end(line);
            if (strlen(line) == 0) continue;

            /* 顺序：readerId, readerName, gender, phone, role */
            Reader r;
            strcpy(buf, line);

            tok = strtok(buf, ",");  if (!tok) continue;
            r.readerId = atoi(tok);

            tok = strtok(NULL, ","); if (!tok) continue;
            strncpy(r.readerName, tok, 19); r.readerName[19] = '\0';

            tok = strtok(NULL, ","); if (!tok) continue;
            strncpy(r.gender, tok, 4);      r.gender[4] = '\0';

            tok = strtok(NULL, ","); if (!tok) continue;
            strncpy(r.phone, tok, 14);      r.phone[14] = '\0';

            tok = strtok(NULL, ","); if (!tok) continue;
            r.role = atoi(tok);

            if (rL->length < MAX_READER)
                rL->data[rL->length++] = r;
        }
        fclose(fp);
    }

    /* ---------- 加载借阅记录数据 ---------- */
    fp = fopen(BORROW_FILE, "r");
    if (fp == NULL) {
        fp = fopen(BORROW_FILE, "w");
        if (fp) fclose(fp);
    } else {
        while (fgets(line, sizeof(line), fp)) {
            trim_end(line);
            if (strlen(line) == 0) continue;

            /* 顺序：borrowId, bookId, readerId, borrowTime, returnTime, deadline */
            Borrow br;
            strcpy(buf, line);

            tok = strtok(buf, ",");  if (!tok) continue;
            br.borrowId = atoi(tok);

            tok = strtok(NULL, ","); if (!tok) continue;
            br.bookId = atoi(tok);

            tok = strtok(NULL, ","); if (!tok) continue;
            br.readerId = atoi(tok);

            tok = strtok(NULL, ","); if (!tok) continue;
            strncpy(br.borrowTime, tok, 14); br.borrowTime[14] = '\0';

            tok = strtok(NULL, ","); if (!tok) continue;
            strncpy(br.returnTime, tok, 14); br.returnTime[14] = '\0';

            tok = strtok(NULL, ","); if (!tok) continue;
            strncpy(br.deadline, tok, 14);   br.deadline[14] = '\0';

            if (brL->length < MAX_BORROW)
                brL->data[brL->length++] = br;
        }
        fclose(fp);
    }

    return 1;
}

/* ----------------------------------------------------------------
   SaveData：将内存三张顺序表覆盖写入对应 txt 文件
   - 以 "w" 模式打开，每次覆盖全部内容，保证文件与内存同步
   - 保存成功返回 1，打开文件失败返回 0
   ---------------------------------------------------------------- */
int SaveData(BookList *bL, ReaderList *rL, BorrowList *brL) {
    FILE *fp;
    int   i;

    /* ---------- 保存图书数据 ---------- */
    fp = fopen(BOOK_FILE, "w");
    if (!fp) { printf("错误：无法写入 %s\n", BOOK_FILE); return 0; }
    for (i = 0; i < bL->length; i++) {
        Book *b = &bL->data[i];
        fprintf(fp, "%d,%s,%s,%s,%d,%d\n",
                b->bookId, b->bookName, b->author,
                b->category, b->stock, b->borrowCount);
    }
    fclose(fp);

    /* ---------- 保存读者数据 ---------- */
    fp = fopen(READER_FILE, "w");
    if (!fp) { printf("错误：无法写入 %s\n", READER_FILE); return 0; }
    for (i = 0; i < rL->length; i++) {
        Reader *r = &rL->data[i];
        fprintf(fp, "%d,%s,%s,%s,%d\n",
                r->readerId, r->readerName, r->gender, r->phone, r->role);
    }
    fclose(fp);

    /* ---------- 保存借阅记录数据 ---------- */
    fp = fopen(BORROW_FILE, "w");
    if (!fp) { printf("错误：无法写入 %s\n", BORROW_FILE); return 0; }
    for (i = 0; i < brL->length; i++) {
        Borrow *br = &brL->data[i];
        fprintf(fp, "%d,%d,%d,%s,%s,%s\n",
                br->borrowId, br->bookId, br->readerId,
                br->borrowTime, br->returnTime, br->deadline);
    }
    fclose(fp);

    return 1;
}
