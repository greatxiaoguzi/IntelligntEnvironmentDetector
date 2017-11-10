#include "sys.h"
#include "ff.h"
#define SDMaxFileCount    10

extern FATFS *fs[2]; 

void FilePath_process(char * source,char *addstring);
void ClearPath(char *path);
// void FileOpen(LISTBOX_Handle hFileBrowseListBox,TEXT_Handle hText,char *buf);
uint8_t exfuns_init(void);
uint8_t exf_getfree(uint8_t *drv,uint32_t *total,uint32_t *free);

