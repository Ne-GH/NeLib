#ifndef _MY_LS_

#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>

/* Getname函数使用了opendir函数和readdir函数
* opendir() ------ dirent.h
* readdir() ------ dirent.h
*/
void Getname(){

    DIR *pdir            = NULL;
    struct dirent *pdent = NULL;
    char *file           = NULL;

    /* 如果打开失败 */
    if(NULL == (pdir = opendir("."))){
        exit(-1);
    }
    else{
        while(NULL != (pdent = readdir(pdir))){
            printf("%s ",pdent->d_name);
        }
    }
    putchar(10);

}
#define _MY_LS_
#endif
