#include <stdio.h>
#include "reader.h"

static const char *role_name(int role) {
    if (role == 1) return "管理员";
    if (role == 2) return "教师";
    return "学生";
}

static void print_reader(const Reader *r) {
    printf("%-10d %-16s %-8s %-16s %-10s\n",
           r->readerId, r->readerName, r->gender, r->phone, role_name(r->role));
}

static void print_header(void) {
    printf("%-10s %-16s %-8s %-16s %-10s\n",
           "读者编号", "姓名", "性别", "电话", "身份");
    printf("--------------------------------------------------------------\n");
}

void AddReader(ReaderList *L, Reader r) {
    if (L->length >= MAX_READER) {
        printf("存储空间不足，无法添加读者。\n");
        return;
    }
    if (SearchReader(L, r.readerId)) {
        printf("读者编号已存在。\n");
        return;
    }
    L->data[L->length++] = r;
    printf("读者添加成功。\n");
}

int DelReader(ReaderList *L, int readerId) {
    int i;
    for (i = 0; i < L->length; i++) {
        if (L->data[i].readerId == readerId) {
            int j;
            for (j = i; j < L->length - 1; j++)
                L->data[j] = L->data[j + 1];
            L->length--;
            return 1;
        }
    }
    return 0;
}

int ModReader(ReaderList *L, int readerId, Reader newReader) {
    Reader *target = SearchReader(L, readerId);
    if (!target) return 0;
    newReader.readerId = readerId;
    *target = newReader;
    return 1;
}

Reader *SearchReader(ReaderList *L, int readerId) {
    int i;
    for (i = 0; i < L->length; i++)
        if (L->data[i].readerId == readerId) return &L->data[i];
    return NULL;
}

void ShowAllReader(ReaderList *L) {
    int i;
    if (L->length == 0) {
        printf("暂无读者数据。\n");
        return;
    }
    print_header();
    for (i = 0; i < L->length; i++) print_reader(&L->data[i]);
}

void ReaderManage(ReaderList *L) {
    int choice;
    for (;;) {
        printf("\n========== 读者管理 ==========\n");
        printf("1. 添加读者\n2. 删除读者\n3. 修改读者\n");
        printf("4. 按编号查询\n5. 浏览全部读者\n0. 返回上一级\n");
        printf("请输入选项：");
        if (scanf("%d", &choice) != 1) {
            scanf("%*s");
            printf("输入格式错误。\n");
            continue;
        }
        if (choice == 0) return;
        if (choice == 1 || choice == 3) {
            Reader r = {0};
            int id;
            if (choice == 1) {
                printf("读者编号："); scanf("%d", &r.readerId);
                id = r.readerId;
            } else {
                printf("要修改的读者编号："); scanf("%d", &id);
                if (!SearchReader(L, id)) {
                    printf("编号不存在。\n");
                    continue;
                }
            }
            printf("姓名："); scanf(" %19[^\n]", r.readerName);
            printf("性别："); scanf("%4s", r.gender);
            printf("电话："); scanf("%14s", r.phone);
            printf("身份（1管理员/2教师/3学生）："); scanf("%d", &r.role);
            if (r.role < 1 || r.role > 3) {
                printf("身份编号无效。\n");
            } else if (choice == 1) {
                AddReader(L, r);
            } else {
                printf(ModReader(L, id, r) ? "修改成功。\n" : "修改失败。\n");
            }
        } else if (choice == 2) {
            int id;
            printf("读者编号："); scanf("%d", &id);
            printf(DelReader(L, id) ? "删除成功。\n" : "编号不存在。\n");
        } else if (choice == 4) {
            int id;
            Reader *r;
            printf("读者编号："); scanf("%d", &id);
            r = SearchReader(L, id);
            if (r) {
                print_header();
                print_reader(r);
            } else printf("编号不存在。\n");
        } else if (choice == 5) {
            ShowAllReader(L);
        } else {
            printf("无效选项。\n");
        }
    }
}
