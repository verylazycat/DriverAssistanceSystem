/*
 * @Author: verylazycat 
 * @Date: 2020-02-09 13:27:35 
 * @Last Modified by: verylazycat
 * @Last Modified time: 2020-02-09 13:37:06
 */
#include <main.h>
void getCurrentTime()
{
    time_t rawtime;
    struct tm *ptminfo;
 
    time(&rawtime);
    ptminfo = localtime(&rawtime);
    printf("current: %02d-%02d-%02d %02d:%02d:%02d\n",
            ptminfo->tm_year + 1900, ptminfo->tm_mon + 1, ptminfo->tm_mday,
            ptminfo->tm_hour, ptminfo->tm_min, ptminfo->tm_sec);
}