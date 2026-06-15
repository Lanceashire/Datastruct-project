#include <stdio.h>
#include <string.h>
#include "book.h"
#include "hash.h"

static const char *copy_status_name(int status) {
    if (status == COPY_AVAILABLE) return "可借";
    if (status == COPY_BORROWED) return "借出";
    if (status == COPY_DAMAGED) return "损坏";
    if (status == COPY_LOST) return "遗失";
    return "未知";
}

static void print_book_header(void) {
    printf("%-8s %-18s %-22s %-14s %-14s %-10s %-6s %-6s %-8s\n",
           "书目号", "ISBN", "书名", "作者", "出版社", "分类",
           "馆藏", "可借", "借阅次数");
    printf("------------------------------------------------------------------------------------------------------\n");
}

static void print_book(const Book *b, BookCopyList *cL) {
    printf("%-8d %-18s %-22s %-14s %-14s %-10s %-6d %-6d %-8d\n",
           b->catalogId, b->isbn, b->bookName, b->author, b->publisher,
           b->category, CountCopies(cL, b->catalogId, 0),
           CountCopies(cL, b->catalogId, COPY_AVAILABLE), b->borrowCount);
}

void AddBook(BookList *L, Book b) {
    if (L->length >= MAX_BOOK) {
        printf("书目表已满，无法添加。\n");
        return;
    }
    if (SearchBook(L, b.catalogId)) {
        printf("书目编号已存在。\n");
        return;
    }
    if (SearchBookByIsbn(L, b.isbn)) {
        printf("该 ISBN 已存在，请为原书目新增馆藏副本。\n");
        return;
    }
    L->data[L->length++] = b;
    RebuildBookHash(L);
    printf("书目添加成功，请继续录入实体馆藏。\n");
}

int DelBook(BookList *L, BookCopyList *cL, int catalogId) {
    int index = FindCatalogIndexById(catalogId);
    int i;
    if (index < 0) return 0;
    if (CountCopies(cL, catalogId, 0) > 0) return -1;
    for (i = index; i < L->length - 1; i++) L->data[i] = L->data[i + 1];
    L->length--;
    RebuildBookHash(L);
    return 1;
}

int ModBook(BookList *L, int catalogId, Book newBook) {
    int index = FindCatalogIndexById(catalogId);
    int isbnIndex;
    if (index < 0) return 0;
    isbnIndex = FindCatalogIndexByIsbn(newBook.isbn);
    if (isbnIndex >= 0 && isbnIndex != index) return -1;
    newBook.catalogId = catalogId;
    newBook.borrowCount = L->data[index].borrowCount;
    L->data[index] = newBook;
    RebuildBookHash(L);
    return 1;
}

Book *SearchBook(BookList *L, int catalogId) {
    int index = FindCatalogIndexById(catalogId);
    if (index < 0 || index >= L->length) return NULL;
    return &L->data[index];
}

Book *SearchBookByIsbn(BookList *L, const char *isbn) {
    int index = FindCatalogIndexByIsbn(isbn);
    if (index < 0 || index >= L->length) return NULL;
    return &L->data[index];
}

void SearchBookByName(BookList *L, BookCopyList *cL, char *name) {
    int i, found = 0;
    print_book_header();
    for (i = 0; i < L->length; i++) {
        if (strstr(L->data[i].bookName, name)) {
            print_book(&L->data[i], cL);
            found = 1;
        }
    }
    if (!found) printf("未找到匹配书目。\n");
}

void ShowAllBook(BookList *L, BookCopyList *cL) {
    int i;
    if (L->length == 0) {
        printf("暂无书目数据。\n");
        return;
    }
    print_book_header();
    for (i = 0; i < L->length; i++) print_book(&L->data[i], cL);
}

void AddCopy(BookCopyList *cL, BookList *bL, BookCopy copy) {
    if (cL->length >= MAX_COPY) {
        printf("馆藏副本表已满。\n");
        return;
    }
    if (!SearchBook(bL, copy.catalogId)) {
        printf("书目编号不存在。\n");
        return;
    }
    if (SearchCopy(cL, copy.copyId)) {
        printf("馆藏编号已存在。\n");
        return;
    }
    copy.status = COPY_AVAILABLE;
    cL->data[cL->length++] = copy;
    RebuildCopyHash(cL);
    printf("实体馆藏入库成功。\n");
}

int DelCopy(BookCopyList *cL, int copyId) {
    int index = FindCopyIndex(copyId);
    int i;
    if (index < 0) return 0;
    if (cL->data[index].status == COPY_BORROWED) return -1;
    for (i = index; i < cL->length - 1; i++) cL->data[i] = cL->data[i + 1];
    cL->length--;
    RebuildCopyHash(cL);
    return 1;
}

int SetCopyStatus(BookCopyList *cL, int copyId, int status) {
    BookCopy *copy = SearchCopy(cL, copyId);
    if (!copy) return 0;
    if (copy->status == COPY_BORROWED) return -1;
    if (status != COPY_AVAILABLE && status != COPY_DAMAGED && status != COPY_LOST)
        return -2;
    copy->status = status;
    return 1;
}

BookCopy *SearchCopy(BookCopyList *cL, int copyId) {
    int index = FindCopyIndex(copyId);
    if (index < 0 || index >= cL->length) return NULL;
    return &cL->data[index];
}

void ShowCopiesByCatalog(BookCopyList *cL, int catalogId) {
    int i, found = 0;
    printf("%-10s %-10s %-10s %-20s\n", "馆藏编号", "书目编号", "状态", "馆藏位置");
    printf("------------------------------------------------------\n");
    for (i = 0; i < cL->length; i++) {
        BookCopy *copy = &cL->data[i];
        if (copy->catalogId == catalogId) {
            printf("%-10d %-10d %-10s %-20s\n", copy->copyId, copy->catalogId,
                   copy_status_name(copy->status), copy->location);
            found = 1;
        }
    }
    if (!found) printf("该书目暂无实体馆藏。\n");
}

int CountCopies(BookCopyList *cL, int catalogId, int status) {
    int i, count = 0;
    for (i = 0; i < cL->length; i++) {
        if (cL->data[i].catalogId == catalogId &&
            (status == 0 || cL->data[i].status == status))
            count++;
    }
    return count;
}

void UpdateBorrowCount(BookList *L, int catalogId) {
    Book *book = SearchBook(L, catalogId);
    if (book) book->borrowCount++;
}

void BookManage(BookList *bL, BookCopyList *cL) {
    int choice;
    for (;;) {
        printf("\n========== 书目与馆藏管理 ==========\n");
        printf("1. 新增书目\n2. 修改书目\n3. 删除空书目\n");
        printf("4. 新增实体馆藏\n5. 注销实体馆藏\n6. 修改馆藏状态\n");
        printf("7. 查询书目\n8. 查看书目全部副本\n9. 浏览全部书目\n0. 返回上一级\n");
        printf("请输入选项：");
        if (scanf("%d", &choice) != 1) {
            scanf("%*s");
            printf("输入格式错误。\n");
            continue;
        }
        if (choice == 0) return;
        if (choice == 1 || choice == 2) {
            Book b = {0};
            int id, result;
            if (choice == 1) {
                printf("书目编号："); scanf("%d", &b.catalogId);
                id = b.catalogId;
            } else {
                printf("要修改的书目编号："); scanf("%d", &id);
                if (!SearchBook(bL, id)) {
                    printf("书目不存在。\n");
                    continue;
                }
            }
            printf("ISBN："); scanf("%19s", b.isbn);
            printf("书名："); scanf(" %49[^\n]", b.bookName);
            printf("作者："); scanf(" %29[^\n]", b.author);
            printf("出版社："); scanf(" %39[^\n]", b.publisher);
            printf("分类："); scanf(" %19[^\n]", b.category);
            if (choice == 1) AddBook(bL, b);
            else {
                result = ModBook(bL, id, b);
                printf(result == 1 ? "书目修改成功。\n" :
                       result == -1 ? "ISBN 已被其他书目使用。\n" : "修改失败。\n");
            }
        } else if (choice == 3) {
            int id, result;
            printf("书目编号："); scanf("%d", &id);
            result = DelBook(bL, cL, id);
            printf(result == 1 ? "书目删除成功。\n" :
                   result == -1 ? "该书目仍有实体馆藏，不能删除。\n" : "书目不存在。\n");
        } else if (choice == 4) {
            BookCopy copy = {0};
            printf("馆藏编号/条码："); scanf("%d", &copy.copyId);
            printf("所属书目编号："); scanf("%d", &copy.catalogId);
            printf("馆藏位置："); scanf(" %29[^\n]", copy.location);
            AddCopy(cL, bL, copy);
        } else if (choice == 5) {
            int id, result;
            printf("馆藏编号："); scanf("%d", &id);
            result = DelCopy(cL, id);
            printf(result == 1 ? "馆藏注销成功。\n" :
                   result == -1 ? "该副本正在借出，不能注销。\n" : "馆藏编号不存在。\n");
        } else if (choice == 6) {
            int id, status, result;
            printf("馆藏编号："); scanf("%d", &id);
            printf("新状态（1可借/3损坏/4遗失）："); scanf("%d", &status);
            result = SetCopyStatus(cL, id, status);
            printf(result == 1 ? "馆藏状态修改成功。\n" :
                   result == -1 ? "该副本正在借出，不能手工修改状态。\n" :
                   result == -2 ? "状态编号无效。\n" : "馆藏编号不存在。\n");
        } else if (choice == 7) {
            int mode;
            printf("1. 书目编号  2. ISBN  3. 书名："); scanf("%d", &mode);
            if (mode == 1) {
                int id;
                Book *book;
                printf("书目编号："); scanf("%d", &id);
                book = SearchBook(bL, id);
                if (book) { print_book_header(); print_book(book, cL); }
                else printf("书目不存在。\n");
            } else if (mode == 2) {
                char isbn[20];
                Book *book;
                printf("ISBN："); scanf("%19s", isbn);
                book = SearchBookByIsbn(bL, isbn);
                if (book) { print_book_header(); print_book(book, cL); }
                else printf("ISBN 不存在。\n");
            } else if (mode == 3) {
                char name[50];
                printf("书名关键字："); scanf(" %49[^\n]", name);
                SearchBookByName(bL, cL, name);
            } else printf("无效选项。\n");
        } else if (choice == 8) {
            int id;
            printf("书目编号："); scanf("%d", &id);
            ShowCopiesByCatalog(cL, id);
        } else if (choice == 9) {
            ShowAllBook(bL, cL);
        } else {
            printf("无效选项。\n");
        }
    }
}
