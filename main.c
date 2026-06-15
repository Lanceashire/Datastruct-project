#include <stdio.h>
#include "common.h"
#include "fileio.h"
#include "auth.h"
#include "menu.h"

int main(void) {
    BookList bL = {0};
    BookCopyList cL = {0};
    ReaderList rL = {0};
    BorrowList brL = {0};

    if (!LoadData(&bL, &cL, &rL, &brL)) {
        printf("数据加载失败。\n");
        return 1;
    }

    for (;;) {
        int role = Login();
        if (role == 0) break;
        MainMenu(role, &bL, &cL, &rL, &brL);
    }

    if (!SaveData(&bL, &cL, &rL, &brL)) {
        printf("数据保存失败。\n");
        return 1;
    }
    printf("数据已保存，感谢使用。\n");
    return 0;
}
