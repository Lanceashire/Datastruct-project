#include <stdio.h>
#include <string.h>
#include "book.h"

static void print_book(const Book *b) {
    printf("%-8d %-24s %-16s %-12s %-8d %-8d\n",
           b->bookId, b->bookName, b->author, b->category,
           b->stock, b->borrowCount);
}

static void print_header(void) {
    printf("%-8s %-24s %-16s %-12s %-8s %-8s\n",
           "编号", "书名", "作者", "分类", "库存", "借阅次数");
    printf("----------------------------------------------------------------------------\n");
}

void AddBook(BookList *L, Book b) {
    if (L->length >= MAX_BOOK) {
        printf("存储空间不足，无法添加图书。\n");
        return;
    }
    if (SearchBook(L, b.bookId) != NULL) {
        printf("图书编号已存在。\n");
        return;
    }
    L->data[L->length++] = b;
    printf("图书添加成功。\n");
}

int DelBook(BookList *L, int bookId) {
    int i;
    for (i = 0; i < L->length; i++) {
        if (L->data[i].bookId == bookId) {
            int j;
            for (j = i; j < L->length - 1; j++)
                L->data[j] = L->data[j + 1];
            L->length--;
            return 1;
        }
    }
    return 0;
}

int ModBook(BookList *L, int bookId, Book newBook) {
    Book *target = SearchBook(L, bookId);
    if (!target) return 0;
    newBook.bookId = bookId;
    *target = newBook;
    return 1;
}

Book *SearchBook(BookList *L, int bookId) {
    int i;
    for (i = 0; i < L->length; i++)
        if (L->data[i].bookId == bookId) return &L->data[i];
    return NULL;
}

void SearchBookByName(BookList *L, char *name) {
    int i, found = 0;
    print_header();
    for (i = 0; i < L->length; i++) {
        if (strstr(L->data[i].bookName, name) != NULL) {
            print_book(&L->data[i]);
            found = 1;
        }
    }
    if (!found) printf("未找到匹配的图书。\n");
}

void ShowAllBook(BookList *L) {
    int i;
    if (L->length == 0) {
        printf("暂无图书数据。\n");
        return;
    }
    print_header();
    for (i = 0; i < L->length; i++) print_book(&L->data[i]);
}

void UpdateStock(BookList *L, int bookId, int num) {
    Book *b = SearchBook(L, bookId);
    if (b && b->stock + num >= 0) b->stock += num;
}

void UpdateBorrowCount(BookList *L, int bookId) {
    Book *b = SearchBook(L, bookId);
    if (b) b->borrowCount++;
}

void BookManage(BookList *L) {
    int choice;
    for (;;) {
        printf("\n========== 图书管理 ==========\n");
        printf("1. 添加新图书\n2. 删除图书\n3. 修改图书信息\n");
        printf("4. 按编号查询\n5. 按书名查询\n6. 浏览全部图书\n0. 返回\n");
        printf("请输入选项：");
        if (scanf("%d", &choice) != 1) {
            scanf("%*s");
            printf("输入格式错误。\n");
            continue;
        }
        if (choice == 0) return;
        if (choice == 1) {
            Book b = {0};
            printf("图书编号："); scanf("%d", &b.bookId);
            printf("书名："); scanf(" %49[^\n]", b.bookName);
            printf("作者："); scanf(" %29[^\n]", b.author);
            printf("分类："); scanf(" %19[^\n]", b.category);
            printf("库存："); scanf("%d", &b.stock);
            if (b.stock < 0) printf("库存不能为负数。\n");
            else AddBook(L, b);
        } else if (choice == 2) {
            int id;
            printf("图书编号："); scanf("%d", &id);
            printf(DelBook(L, id) ? "删除成功。\n" : "编号不存在。\n");
        } else if (choice == 3) {
            int id;
            Book b = {0};
            printf("要修改的图书编号："); scanf("%d", &id);
            if (!SearchBook(L, id)) {
                printf("编号不存在。\n");
                continue;
            }
            printf("新书名："); scanf(" %49[^\n]", b.bookName);
            printf("新作者："); scanf(" %29[^\n]", b.author);
            printf("新分类："); scanf(" %19[^\n]", b.category);
            printf("新库存："); scanf("%d", &b.stock);
            printf("累计借阅次数："); scanf("%d", &b.borrowCount);
            if (b.stock < 0 || b.borrowCount < 0) printf("库存和借阅次数不能为负数。\n");
            else printf(ModBook(L, id, b) ? "修改成功。\n" : "修改失败。\n");
        } else if (choice == 4) {
            int id;
            Book *b;
            printf("图书编号："); scanf("%d", &id);
            b = SearchBook(L, id);
            if (b) {
                print_header();
                print_book(b);
            } else printf("编号不存在。\n");
        } else if (choice == 5) {
            char name[50];
            printf("书名关键字："); scanf(" %49[^\n]", name);
            SearchBookByName(L, name);
        } else if (choice == 6) {
            ShowAllBook(L);
        } else {
            printf("无效选项。\n");
        }
    }
}
