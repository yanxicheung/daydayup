#include <stdio.h>
#include <stdlib.h>
#include "pac.h"
typedef struct _TB{
    int length;
} TB;
int main(int argc,char* argv[]){
    TB tb={0};
    tb.length=1024;
    char buffer[1022];
    packet_in((byte*)&tb,sizeof(TB));
    packet_in((byte*)buffer,1020);
    packet_in((byte*)buffer,2);
    packet_in((byte*)buffer,2);
    packet_in((byte*)&tb,sizeof(TB));
    packet_in((byte*)buffer,1020);
    packet_in((byte*)buffer,2);
    pac_desc* pd=NULL;
    while((pd=packet_out())!=NULL){
        printf("packet:%d\t",pd->length);
    }
    packet_destory();
    return 0;
}