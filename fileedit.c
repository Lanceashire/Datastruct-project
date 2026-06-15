#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileedit.h"

/* ============================================================
   fileedit.c  —  文件编辑模块
   作者负责人：吴宣慧
   说明：实现对任意 txt 文件的按行增、删、改、查、插入操作。
   实现思路：
     每次操作前将文件全部内容读入字符串数组 lines[]，
     在内存中完成修改后，将 lines[] 整体覆盖写回文件。
   ============================================================ */

#define MAX_LINE_LEN 512    /* 单行最大字节数 */
#define MAX_LINES    500    /* 文件最大行数   */

/* ---- 内部辅助：去除行末换行符 ---- */
static void trim_newline(char *s) {
    int len = (int)strlen(s);
    while (len > 0 && (s[len-1] == '\n' || s[len-1] == '\r'))
        s[--len] = '\0';
}

/* ---- 内部辅助：把文件所有行读入 lines[]，返回行数，失败返回 -1 ---- */
static int read_all_lines(const char *filename,
                          char lines[][MAX_LINE_LEN], int max) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("错误：无法打开文件 %s\n", filename);
        return -1;
    }
    int count = 0;
    while (count < max && fgets(lines[count], MAX_LINE_LEN, fp)) {
        trim_newline(lines[count]);
        count++;
    }
    fclose(fp);
    return count;
}

/* ---- 内部辅助：将 lines[] 的 count 行覆盖写回文件，成功返回 1 ---- */
static int write_all_lines(const char *filename,
                           char lines[][MAX_LINE_LEN], int count) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("错误：无法写入文件 %s\n", filename);
        return 0;
    }
    int i;
    for (i = 0; i < count; i++)
        fprintf(fp, "%s\n", lines[i]);
    fclose(fp);
    return 1;
}

/* ================================================================
   CreateFile：创建新 txt 文件
   - 若文件已存在则提示，不覆盖原文件
   ================================================================ */
void CreateFile(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp) {
        fclose(fp);
        printf("提示：文件 %s 已存在，未重新创建。\n", filename);
        return;
    }
    fp = fopen(filename, "w");
    if (fp) {
        fclose(fp);
        printf("文件 %s 创建成功。\n", filename);
    } else {
        printf("错误：文件创建失败，请检查文件名是否合法。\n");
    }
}

/* ================================================================
   OpenAndShowFile：按行显示文件内容，每行前加行号
   ================================================================ */
void OpenAndShowFile(char *filename) {
    static char lines[MAX_LINES][MAX_LINE_LEN];
    int count = read_all_lines(filename, lines, MAX_LINES);
    if (count < 0) return;
    if (count == 0) {
        printf("文件 %s 内容为空。\n", filename);
        return;
    }
    int i;
    printf("------ %s（共 %d 行）------\n", filename, count);
    for (i = 0; i < count; i++)
        printf("%3d | %s\n", i + 1, lines[i]);
    printf("----------------------------\n");
}

/* ================================================================
   InsertLine：在第 lineNo 行之后插入新内容
   - lineNo = 0 表示插入到文件最开头（第 1 行之前）
   - lineNo = k 表示在第 k 行之后插入（k 从 1 计数）
   ================================================================ */
void InsertLine(char *filename, int lineNo, char *content) {
    static char lines[MAX_LINES][MAX_LINE_LEN];
    int count = read_all_lines(filename, lines, MAX_LINES);
    if (count < 0) return;

    if (lineNo < 0 || lineNo > count) {
        printf("错误：行号 %d 超出范围（当前共 %d 行，有效范围 0~%d）。\n",
               lineNo, count, count);
        return;
    }
    if (count >= MAX_LINES) {
        printf("错误：文件行数已达上限 %d，无法继续插入。\n", MAX_LINES);
        return;
    }

    /* 将 lineNo 位置（0-based）及之后的所有行向后移一位 */
    int i;
    for (i = count; i > lineNo; i--)
        strcpy(lines[i], lines[i - 1]);

    strncpy(lines[lineNo], content, MAX_LINE_LEN - 1);
    lines[lineNo][MAX_LINE_LEN - 1] = '\0';
    count++;

    if (write_all_lines(filename, lines, count))
        printf("已在第 %d 行后成功插入新内容。\n", lineNo);
}

/* ================================================================
   DeleteLine：删除第 lineNo 行（行号从 1 计数）
   ================================================================ */
void DeleteLine(char *filename, int lineNo) {
    static char lines[MAX_LINES][MAX_LINE_LEN];
    int count = read_all_lines(filename, lines, MAX_LINES);
    if (count < 0) return;

    if (lineNo < 1 || lineNo > count) {
        printf("错误：行号 %d 超出范围（当前共 %d 行）。\n", lineNo, count);
        return;
    }

    /* 将目标行之后的所有行向前移一位，覆盖被删行 */
    int i;
    for (i = lineNo - 1; i < count - 1; i++)
        strcpy(lines[i], lines[i + 1]);
    count--;

    if (write_all_lines(filename, lines, count))
        printf("第 %d 行已成功删除。\n", lineNo);
}

/* ================================================================
   ReplaceLine：替换第 lineNo 行的内容（行号从 1 计数）
   ================================================================ */
void ReplaceLine(char *filename, int lineNo, char *content) {
    static char lines[MAX_LINES][MAX_LINE_LEN];
    int count = read_all_lines(filename, lines, MAX_LINES);
    if (count < 0) return;

    if (lineNo < 1 || lineNo > count) {
        printf("错误：行号 %d 超出范围（当前共 %d 行）。\n", lineNo, count);
        return;
    }

    strncpy(lines[lineNo - 1], content, MAX_LINE_LEN - 1);
    lines[lineNo - 1][MAX_LINE_LEN - 1] = '\0';

    if (write_all_lines(filename, lines, count))
        printf("第 %d 行已成功替换。\n", lineNo);
}

/* ================================================================
   SearchInFile：在文件中查找含有关键字的行并输出
   ================================================================ */
void SearchInFile(char *filename, char *keyword) {
    static char lines[MAX_LINES][MAX_LINE_LEN];
    int count = read_all_lines(filename, lines, MAX_LINES);
    if (count < 0) return;

    int i, found = 0;
    printf("------ 搜索关键字：\"%s\" ------\n", keyword);
    for (i = 0; i < count; i++) {
        if (strstr(lines[i], keyword)) {
            printf("%3d | %s\n", i + 1, lines[i]);
            found++;
        }
    }
    if (found == 0)
        printf("未找到含关键字的行。\n");
    else
        printf("共找到 %d 行。\n", found);
}

/* ================================================================
   FileEditMenu：文件编辑子菜单（管理员专用入口）
   ================================================================ */
void FileEditMenu(void) {
    char filename[100];
    int  choice, lineNo;
    char content[MAX_LINE_LEN];
    char keyword[100];

    printf("\n请输入要操作的文件名（如 book.txt，输入 0 返回）：");
    scanf("%99s", filename);
    if (strcmp(filename, "0") == 0) return;

    while (1) {
        printf("\n========== 文件编辑 [%s] ==========\n", filename);
        printf("1. 显示文件内容\n");
        printf("2. 创建新文件\n");
        printf("3. 在指定行后插入一行\n");
        printf("4. 删除指定行\n");
        printf("5. 替换指定行内容\n");
        printf("6. 查找关键字\n");
        printf("7. 切换操作文件\n");
        printf("0. 返回上一级\n");
        printf("请输入选项：");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                OpenAndShowFile(filename);
                break;

            case 2:
                CreateFile(filename);
                break;

            case 3:
                OpenAndShowFile(filename);
                printf("在第几行后插入？（输入 0 表示插入到开头）：");
                scanf("%d", &lineNo);
                printf("请输入新内容：");
                scanf(" %511[^\n]", content);
                InsertLine(filename, lineNo, content);
                break;

            case 4:
                OpenAndShowFile(filename);
                printf("请输入要删除的行号：");
                scanf("%d", &lineNo);
                DeleteLine(filename, lineNo);
                break;

            case 5:
                OpenAndShowFile(filename);
                printf("请输入要替换的行号：");
                scanf("%d", &lineNo);
                printf("请输入新内容：");
                scanf(" %511[^\n]", content);
                ReplaceLine(filename, lineNo, content);
                break;

            case 6:
                printf("请输入关键字：");
                scanf("%99s", keyword);
                SearchInFile(filename, keyword);
                break;

            case 7:
                printf("请输入新文件名：");
                scanf("%99s", filename);
                break;

            case 0:
                return;

            default:
                printf("无效选项，请重新输入。\n");
        }
    }
}
