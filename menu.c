#include <stdio.h>
#include "menu.h"
#include "book.h"
#include "reader.h"
#include "borrow.h"
#include "fileedit.h"

static void ReaderBookQuery(BookList *bL) {
    int choice;
    printf("1. 按编号查询  2. 按书名查询  3. 浏览全部：");
    if (scanf("%d", &choice) != 1) {
        scanf("%*s");
        printf("输入格式错误。\n");
        return;
    }
    if (choice == 1) {
        int id;
        Book *book;
        printf("图书编号："); scanf("%d", &id);
        book = SearchBook(bL, id);
        if (book)
            printf("编号：%d 书名：%s 作者：%s 分类：%s 库存：%d 借阅次数：%d\n",
                   book->bookId, book->bookName, book->author, book->category,
                   book->stock, book->borrowCount);
        else
            printf("编号不存在。\n");
    } else if (choice == 2) {
        char name[50];
        printf("书名关键字："); scanf(" %49[^\n]", name);
        SearchBookByName(bL, name);
    } else if (choice == 3) {
        ShowAllBook(bL);
    } else {
        printf("无效选项。\n");
    }
}

void MainMenu(int role, BookList *bL, ReaderList *rL, BorrowList *brL) {
    int choice;
    for (;;) {
        if (role == 1) {
            printf("\n========== 管理员主菜单 ==========\n");
            printf("1. 图书信息管理\n2. 读者信息管理\n3. 借阅业务管理\n");
            printf("4. 文件管理与文本编辑\n0. 退出系统\n");
            printf("请输入选项：");
            if (scanf("%d", &choice) != 1) {
                scanf("%*s");
                printf("输入格式错误。\n");
                continue;
            }
            if (choice == 0) return;
            if (choice == 1) BookManage(bL);
            else if (choice == 2) ReaderManage(rL);
            else if (choice == 3) BorrowManage(bL, rL, brL, role);
            else if (choice == 4) FileEditMenu();
            else printf("无效选项。\n");
        } else {
            int readerId;
            printf("\n========== 读者主菜单 ==========\n");
            printf("1. 查询图书\n2. 图书借阅/归还\n3. 查看个人借阅记录\n");
            printf("4. 查看借阅提醒\n0. 退出系统\n");
            printf("请输入选项：");
            if (scanf("%d", &choice) != 1) {
                scanf("%*s");
                printf("输入格式错误。\n");
                continue;
            }
            if (choice == 0) return;
            if (choice == 1) {
                ReaderBookQuery(bL);
            } else if (choice == 2) {
                BorrowManage(bL, rL, brL, role);
            } else if (choice == 3) {
                printf("读者编号："); scanf("%d", &readerId);
                SearchBorrowByReader(brL, readerId);
            } else if (choice == 4) {
                CheckRemind(brL);
            } else {
                printf("无效选项。\n");
            }
        }
    }
}
