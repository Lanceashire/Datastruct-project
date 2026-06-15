#include <stdio.h>
#include "auth.h"

int Login(void) {
    int role;
    for (;;) {
        int result;
        printf("\n========== 图书管理系统 登录 ==========\n");
        printf("1. 系统管理员\n2. 教师\n3. 学生\n");
        printf("请选择身份（1-3）：");
        result = scanf("%d", &role);
        if (result == 1 && role >= 1 && role <= 3) return role;
        if (result != 1) scanf("%*s");
        printf("输入无效，请重新选择。\n");
    }
}
