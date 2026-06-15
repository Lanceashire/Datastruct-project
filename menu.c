#include <stdio.h>
#include "menu.h"
#include "book.h"
#include "reader.h"
#include "borrow.h"
#include "fileedit.h"

static void ReaderBookQuery(BookList *bL, BookCopyList *cL) {
    int choice;
    printf("\n========== 图书查询 ==========\n");
    printf("1. 按书目编号查询\n2. 按 ISBN 查询\n3. 按书名查询\n");
    printf("4. 浏览全部书目\n5. 查看某书目的实体副本\n0. 返回上一级\n");
    printf("请输入选项：");
    if (scanf("%d", &choice) != 1) {
        scanf("%*s");
        printf("输入格式错误。\n");
        return;
    }
    if (choice == 0) return;
    if (choice == 1) {
        int id;
        Book *book;
        printf("书目编号："); scanf("%d", &id);
        book = SearchBook(bL, id);
        if (book)
            printf("书目号：%d ISBN：%s 书名：%s 作者：%s 出版社：%s 分类：%s 馆藏：%d 可借：%d\n",
                   book->catalogId, book->isbn, book->bookName, book->author,
                   book->publisher, book->category, CountCopies(cL, id, 0),
                   CountCopies(cL, id, COPY_AVAILABLE));
        else printf("书目不存在。\n");
    } else if (choice == 2) {
        char isbn[20];
        Book *book;
        printf("ISBN："); scanf("%19s", isbn);
        book = SearchBookByIsbn(bL, isbn);
        if (book)
            printf("书目号：%d 书名：%s 作者：%s 可借：%d\n",
                   book->catalogId, book->bookName, book->author,
                   CountCopies(cL, book->catalogId, COPY_AVAILABLE));
        else printf("ISBN 不存在。\n");
    } else if (choice == 3) {
        char name[50];
        printf("书名关键字："); scanf(" %49[^\n]", name);
        SearchBookByName(bL, cL, name);
    } else if (choice == 4) {
        ShowAllBook(bL, cL);
    } else if (choice == 5) {
        int id;
        printf("书目编号："); scanf("%d", &id);
        ShowCopiesByCatalog(cL, id);
    } else printf("无效选项。\n");
}

void MainMenu(int role, BookList *bL, BookCopyList *cL,
              ReaderList *rL, BorrowList *brL) {
    int choice;
    for (;;) {
        if (role == 1) {
            printf("\n========== 管理员主菜单 ==========\n");
            printf("1. 书目与馆藏管理\n2. 读者信息管理\n3. 借阅业务管理\n");
            printf("4. 文件管理与文本编辑\n0. 返回登录页面\n");
            printf("请输入选项：");
            if (scanf("%d", &choice) != 1) {
                scanf("%*s");
                printf("输入格式错误。\n");
                continue;
            }
            if (choice == 0) return;
            if (choice == 1) BookManage(bL, cL);
            else if (choice == 2) ReaderManage(rL);
            else if (choice == 3) BorrowManage(bL, cL, rL, brL, role);
            else if (choice == 4) FileEditMenu();
            else printf("无效选项。\n");
        } else {
            int readerId;
            printf("\n========== 读者主菜单 ==========\n");
            printf("1. 查询书目与馆藏\n2. 图书借阅/归还\n3. 查看个人借阅记录\n");
            printf("4. 查看借阅提醒\n0. 返回登录页面\n");
            printf("请输入选项：");
            if (scanf("%d", &choice) != 1) {
                scanf("%*s");
                printf("输入格式错误。\n");
                continue;
            }
            if (choice == 0) return;
            if (choice == 1) ReaderBookQuery(bL, cL);
            else if (choice == 2) BorrowManage(bL, cL, rL, brL, role);
            else if (choice == 3) {
                printf("读者编号："); scanf("%d", &readerId);
                SearchBorrowByReader(brL, readerId);
            } else if (choice == 4) CheckRemind(brL);
            else printf("无效选项。\n");
        }
    }
}
