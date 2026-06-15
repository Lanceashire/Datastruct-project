#include <stdio.h>
#include <string.h>
#include <time.h>
#include "borrow.h"
#include "book.h"
#include "reader.h"
#include "hash.h"

static Borrow *find_borrow(BorrowList *L, int borrowId) {
    int index = FindBorrowIndex(borrowId);
    if (index < 0 || index >= L->length) return NULL;
    return &L->data[index];
}

static void print_header(void) {
    printf("%-8s %-10s %-10s %-12s %-12s %-12s\n",
           "记录号", "馆藏编号", "读者号", "借阅日期", "归还日期", "截止日期");
    printf("----------------------------------------------------------------------------\n");
}

static void print_borrow(const Borrow *br) {
    printf("%-8d %-10d %-10d %-12s %-12s %-12s\n",
           br->borrowId, br->copyId, br->readerId,
           br->borrowTime, br->returnTime, br->deadline);
}

static int parse_date(const char *text, struct tm *out) {
    int y, m, d;
    memset(out, 0, sizeof(*out));
    if (sscanf(text, "%d-%d-%d", &y, &m, &d) != 3) return 0;
    if (m < 1 || m > 12 || d < 1 || d > 31) return 0;
    out->tm_year = y - 1900;
    out->tm_mon = m - 1;
    out->tm_mday = d;
    out->tm_hour = 12;
    return mktime(out) != (time_t)-1;
}

void AddBorrow(BorrowList *L, BookList *bL, BookCopyList *cL, Borrow br) {
    BookCopy *copy;
    if (L->length >= MAX_BORROW) {
        printf("借阅记录表已满。\n");
        return;
    }
    if (find_borrow(L, br.borrowId)) {
        printf("借阅编号已存在。\n");
        return;
    }
    copy = SearchCopy(cL, br.copyId);
    if (!copy) {
        printf("馆藏编号不存在。\n");
        return;
    }
    if (copy->status != COPY_AVAILABLE) {
        printf("该实体书当前不可借。\n");
        return;
    }
    strcpy(br.returnTime, "未归还");
    L->data[L->length++] = br;
    copy->status = COPY_BORROWED;
    UpdateBorrowCount(bL, copy->catalogId);
    RebuildBorrowHash(L);
    printf("借阅成功，馆藏 %d 已标记为借出。\n", br.copyId);
}

int ReturnBook(BorrowList *L, BookCopyList *cL, int borrowId, char *retTime) {
    Borrow *br = find_borrow(L, borrowId);
    BookCopy *copy;
    if (!br || strcmp(br->returnTime, "未归还") != 0) return 0;
    copy = SearchCopy(cL, br->copyId);
    if (!copy) return -1;
    strncpy(br->returnTime, retTime, sizeof(br->returnTime) - 1);
    br->returnTime[sizeof(br->returnTime) - 1] = '\0';
    copy->status = COPY_AVAILABLE;
    return 1;
}

void SearchBorrowByCatalog(BorrowList *L, BookCopyList *cL, int catalogId) {
    int i, found = 0;
    print_header();
    for (i = 0; i < L->length; i++) {
        BookCopy *copy = SearchCopy(cL, L->data[i].copyId);
        if (copy && copy->catalogId == catalogId) {
            print_borrow(&L->data[i]);
            found = 1;
        }
    }
    if (!found) printf("未找到该书目的借阅记录。\n");
}

void SearchBorrowByReader(BorrowList *L, int readerId) {
    int i, found = 0;
    print_header();
    for (i = 0; i < L->length; i++) {
        if (L->data[i].readerId == readerId) {
            print_borrow(&L->data[i]);
            found = 1;
        }
    }
    if (!found) printf("未找到借阅记录。\n");
}

void BorrowRank(BookList *bL, BookCopyList *cL) {
    BookList copy = *bL;
    int i, j;
    for (i = 0; i < copy.length - 1; i++) {
        for (j = 0; j < copy.length - 1 - i; j++) {
            if (copy.data[j].borrowCount < copy.data[j + 1].borrowCount) {
                Book temp = copy.data[j];
                copy.data[j] = copy.data[j + 1];
                copy.data[j + 1] = temp;
            }
        }
    }
    printf("%-6s %-8s %-18s %-22s %-8s %-8s\n",
           "排名", "书目号", "ISBN", "书名", "可借", "借阅次数");
    printf("--------------------------------------------------------------------------------\n");
    for (i = 0; i < copy.length; i++)
        printf("%-6d %-8d %-18s %-22s %-8d %-8d\n", i + 1,
               copy.data[i].catalogId, copy.data[i].isbn, copy.data[i].bookName,
               CountCopies(cL, copy.data[i].catalogId, COPY_AVAILABLE),
               copy.data[i].borrowCount);
}

void CheckRemind(BorrowList *L) {
    time_t now = time(NULL);
    struct tm today = *localtime(&now);
    int i, found = 0;
    today.tm_hour = 12;
    today.tm_min = today.tm_sec = 0;
    now = mktime(&today);
    for (i = 0; i < L->length; i++) {
        struct tm deadlineTm;
        double days;
        Borrow *br = &L->data[i];
        if (strcmp(br->returnTime, "未归还") != 0) continue;
        if (!parse_date(br->deadline, &deadlineTm)) {
            printf("记录 %d 的截止日期无效：%s\n", br->borrowId, br->deadline);
            continue;
        }
        days = difftime(mktime(&deadlineTm), now) / (24 * 60 * 60);
        if (days < 0) {
            printf("[超期] 记录%d，读者%d，馆藏%d，已超期%.0f天。\n",
                   br->borrowId, br->readerId, br->copyId, -days);
            found = 1;
        } else if (days <= 3) {
            printf("[临期] 记录%d，读者%d，馆藏%d，剩余%.0f天。\n",
                   br->borrowId, br->readerId, br->copyId, days);
            found = 1;
        }
    }
    if (!found) printf("当前没有临期或超期记录。\n");
}

void BorrowManage(BookList *bL, BookCopyList *cL, ReaderList *rL,
                  BorrowList *brL, int role) {
    int choice;
    for (;;) {
        printf("\n========== 借阅业务管理 ==========\n");
        if (role == 1) {
            printf("1. 办理借阅\n2. 办理归还\n3. 按书目查询记录\n");
            printf("4. 按读者查询记录\n5. 书目借阅排行\n6. 临期/超期提醒\n0. 返回上一级\n");
        } else {
            printf("1. 办理借阅\n2. 办理归还\n4. 查看个人借阅记录\n");
            printf("6. 临期/超期提醒\n0. 返回上一级\n");
        }
        printf("请输入选项：");
        if (scanf("%d", &choice) != 1) {
            scanf("%*s");
            printf("输入格式错误。\n");
            continue;
        }
        if (choice == 0) return;
        if (choice == 1) {
            Borrow br = {0};
            printf("借阅编号："); scanf("%d", &br.borrowId);
            printf("馆藏编号/条码："); scanf("%d", &br.copyId);
            printf("读者编号："); scanf("%d", &br.readerId);
            if (!SearchReader(rL, br.readerId)) {
                printf("读者编号不存在。\n");
                continue;
            }
            printf("借阅日期（YYYY-MM-DD）："); scanf("%14s", br.borrowTime);
            printf("截止日期（YYYY-MM-DD）："); scanf("%14s", br.deadline);
            AddBorrow(brL, bL, cL, br);
        } else if (choice == 2) {
            int id, result;
            char date[15];
            printf("借阅编号："); scanf("%d", &id);
            printf("归还日期（YYYY-MM-DD）："); scanf("%14s", date);
            result = ReturnBook(brL, cL, id, date);
            printf(result == 1 ? "归还成功，该实体书恢复为可借。\n" :
                   result == -1 ? "借阅记录对应的馆藏已不存在。\n" :
                   "记录不存在或已经归还。\n");
        } else if (choice == 3 && role == 1) {
            int id;
            printf("书目编号："); scanf("%d", &id);
            SearchBorrowByCatalog(brL, cL, id);
        } else if (choice == 4) {
            int id;
            printf("读者编号："); scanf("%d", &id);
            SearchBorrowByReader(brL, id);
        } else if (choice == 5 && role == 1) {
            BorrowRank(bL, cL);
        } else if (choice == 6) {
            CheckRemind(brL);
        } else printf("无效选项。\n");
    }
}
