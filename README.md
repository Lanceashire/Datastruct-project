# 高校图书馆管理系统

一个使用 C 语言实现的控制台图书馆管理系统。项目同时使用**顺序表**保存业务数据，并使用**开放寻址哈希表**建立内存索引。

当前版本已将原来的“图书信息 + 库存数字”模型升级为“书目 + 实体馆藏”模型。同一种书只保存一份书目信息，但图书馆里的每一本实体书都有独立馆藏编号和状态。

## 核心模型

```text
Book（书目）
  1 ───── N
BookCopy（实体馆藏）
  1 ───── N
Borrow（借阅记录）
  N ───── 1
Reader（读者）
```

- `Book`：一种书，使用 ISBN 判断是否为同一版本。
- `BookCopy`：一本实际存在的书，拥有唯一馆藏编号或条码。
- `Borrow`：关联具体 `copyId`，可以准确追踪借走了哪一本。
- `Reader`：读者或管理员信息。

例如，两本相同的《数据结构》会表示为：

```text
书目：1001, ISBN 9787115379504, 数据结构
馆藏：5001, 属于书目 1001, A-01, 可借
馆藏：5002, 属于书目 1001, A-02, 已借出
```

## 功能

- 使用 ISBN 防止同一种书重复建立书目
- 为同一书目录入多个独立馆藏副本
- 按书目编号、ISBN、书名查询
- 查看总馆藏数、可借数量和每本实体书状态
- 使用具体馆藏编号办理借阅和归还
- 自动切换实体书的可借、借出状态
- 图书借阅排行和临期、超期提醒
- 读者信息增删改查
- 管理员、教师、学生分级菜单
- TXT 文件持久化和文本文件编辑

## 数据结构

### 顺序表

| 顺序表 | 容量 | 用途 |
|---|---:|---|
| `BookList` | 200 | 保存书目信息 |
| `BookCopyList` | 1000 | 保存实体馆藏 |
| `ReaderList` | 200 | 保存读者 |
| `BorrowList` | 500 | 保存借阅记录 |

顺序表适合全量浏览、文件保存和借阅排行排序。新增元素放在表尾，删除元素时后续元素向前移动。

### 哈希索引

系统使用字符串哈希函数和线性探测解决冲突，建立五张内存哈希表：

```text
catalogId → BookList 下标
ISBN      → BookList 下标
copyId    → BookCopyList 下标
readerId  → ReaderList 下标
borrowId  → BorrowList 下标
```

编号和 ISBN 查询的平均时间复杂度由顺序查找的 `O(n)`降低到 `O(1)`。删除顺序表元素会改变数组下标，因此删除后会重建对应哈希索引。

哈希索引不写入文件。程序启动加载数据后自动重建，避免保存重复数据和索引失效。

## 项目结构

```text
main.c                  程序入口
common.h                公共数据结构和接口
hash.c / hash.h         开放寻址哈希索引
book.c / book.h         书目和实体馆藏管理
reader.c / reader.h     读者管理
borrow.c / borrow.h     借阅、归还、排行和提醒
fileio.c / fileio.h     四张业务表的数据持久化
auth.c / auth.h         身份选择
menu.c / menu.h         分级菜单
fileedit.c / fileedit.h 文本文件编辑
改进设计说明.md          新旧模型对比与设计说明
```

## 编译运行

```bash
gcc -std=c11 -Wall -Wextra -pedantic -o library main.c hash.c book.c reader.c borrow.c fileio.c fileedit.c auth.c menu.c
```

Windows：

```powershell
.\library.exe
```

## 数据文件

`book.txt`保存书目：

```text
catalogId,isbn,bookName,author,publisher,category,borrowCount
1001,9787115379504,数据结构,严蔚敏,清华大学出版社,计算机,12
```

`copy.txt`保存实体馆藏：

```text
copyId,catalogId,status,location
5001,1001,1,A-01
5002,1001,2,A-02
```

馆藏状态：`1`可借、`2`借出、`3`损坏、`4`遗失。

`reader.txt`保存读者：

```text
readerId,readerName,gender,phone,role
2001,张三,男,13800000000,3
```

`borrow.txt`保存借阅记录：

```text
borrowId,copyId,readerId,borrowTime,returnTime,deadline
3001,5002,2001,2026-06-15,未归还,2026-07-15
```

## 操作流程

1. 管理员先建立书目，ISBN 必须唯一。
2. 为书目录入一个或多个实体馆藏编号。
3. 登记读者。
4. 借阅时输入具体馆藏编号，系统将该副本标记为借出。
5. 归还时输入借阅编号，系统将对应副本恢复为可借。
6. 登录页输入 `0`退出，四张业务表自动保存。

## 文档说明

仓库中的《综合性实验报告》记录的是最初的顺序表版本，其中 `Book.stock`表示库存。当前代码已经升级，最新实现以本 README 和《改进设计说明》为准。

## 局限

- 登录仍是课程演示形式，没有账号密码认证。
- 文本文件不是完整 CSV，字段中不应包含英文逗号。
- 哈希索引适合当前固定容量；更大规模系统应使用数据库索引。
