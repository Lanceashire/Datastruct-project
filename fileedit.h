#ifndef FILEEDIT_H
#define FILEEDIT_H

void FileEditMenu(void);
void CreateFile(char *filename);
void OpenAndShowFile(char *filename);
void InsertLine(char *filename, int lineNo, char *content);
void DeleteLine(char *filename, int lineNo);
void ReplaceLine(char *filename, int lineNo, char *content);
void SearchInFile(char *filename, char *keyword);

#endif /* FILEEDIT_H */
