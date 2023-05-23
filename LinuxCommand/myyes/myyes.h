#ifndef __MYYES_MYYES_H__
#define __MYYES_MYYES_H__
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFFERSIZE (1024*8)

char buf[BUFFERSIZE];
void GetStr(int argc,char **argv){
    int pos = argc - 1;
    int len = strlen(argv[pos]);

    // 在 buf 中存放第一个字符串
    memcpy(buf, argv[pos], len);
    buf[len++] = '\n';

    int use = 0;
    for(int i = len;i < BUFFERSIZE;i *= 2){
        memcpy(buf+i,buf, i);
    }

    for(int i = 0;i < BUFFERSIZE ; ++i){
        printf("%d ",buf[i]);
    }

}


#endif
