#include <stdio.h>
#include <time.h>

int main(void) {
    time_t t = time(0)+(time_t)(60*24*60*60);
    char buf[10];
    strftime(buf,10,"%Y%m%d",localtime(&t));
    printf("#define EXPIRE_DATE %s\n",buf);
    return 0;
}