/**
* TCP拆包 粘包代码
* @author mengdj@outlook.com
*/
#ifndef PAC_H
#define PAC_H
#ifndef INNER_BUFFER_SIZE
#define INNER_BUFFER_SIZE 10240         //内部缓冲区的大小
#endif
#include <memory.h>
#include <queue>
typedef unsigned char byte;
typedef struct _PAH_DESC{
    int length;
    int loop;
    byte* buffer;
} pac_desc;

//包队列（存放已经拆开的包）
static std::queue<pac_desc*> pendingQueue;
//内部缓冲区(分别多余包缓冲区以及当前处理缓冲区)
static pac_desc *pr=NULL,*pd=NULL;

//int转换为字节
static void int2byte(int i,byte* bytes,int size=4){
    memset(bytes,0,sizeof(byte)*size);
    bytes[0]=(unsigned char)(0xff&i);
    bytes[1]=(unsigned char)((0xff00&i)>>8);
    bytes[2]=(unsigned char)((0xff0000&i)>>16);
    bytes[3]=(unsigned char)((0xff000000&i)>>24);
}

//字节转换为int
static int byte2int(byte* bytes,int size=4){
    int addr=bytes[0]&0xFF;
    addr|=((bytes[1]<<8)&0xFF00);
    addr|=((bytes[2]<<16)&0xFF0000);
    addr|=((bytes[3]<<24)&0xFF000000);
    return addr;
}

/**
* 初始化
*/
void packet_init(){
}

//释放单个资源
void packet_recycle(pac_desc *pd){
    if(pd!=NULL){
        if(pd->buffer!=NULL){
            free(pd->buffer);
        }
        free(pd);
    }
}

/**
* 释放资源并销毁队列
*/
void packet_destory(){
     while(!pendingQueue.empty()){
        pac_desc* pt=(pac_desc*)pendingQueue.front();
        if(pt!=NULL){
            free(pt->buffer);
            free(pt);
        }
        pendingQueue.pop();
    }

    if(pr!=NULL){
        free(pr->buffer);
        free(pr);
        pr=NULL;
    }

    if(pd!=NULL){
        free(pd->buffer);
        free(pd);
        pd=NULL;
    }
}

//获取一个包
pac_desc* packet_out(){
    if(pendingQueue.empty()){
        return NULL;
    }
    pac_desc* pt=(pac_desc*)pendingQueue.front();
    pendingQueue.pop();
    return pt;
}

/**
* 获取队列大小
*/
int packet_size(){
    if(pendingQueue.empty()){
        return 0;
    }
    return pendingQueue.size();
}

/**
* 处理包长度和拆包，4字节：包内容
* buffer:内容指针
* size:指针长度
*/
void packet_in(byte* buffer,int size){
     if(pr!=NULL){
        if(pr->loop){
            //合并缓冲区,复制到缓冲区并指向新的缓冲区，大小为合并后的大小(内部缓冲区必须大于包的大小，仅分配一次内存)
            memcpy(pr->buffer+pr->loop,buffer,size);
            buffer=pr->buffer;
            size+=pr->loop;
            pr->loop=0;
        }
     }
     if(!size){
        return;
     }
     if(pd==NULL){
        pd=(pac_desc*)malloc(sizeof(pac_desc));
        pd->length=byte2int(buffer);
        pd->buffer=(byte*)malloc(pd->length);
        pd->loop=0;
        //调整指针和数据的长度
        buffer+=4;
        size-=4;
     }

     if(size>0){
         int cond=pd->loop+size;
         printf("%d\r\n",cond);
         if(cond>=pd->length){
            if(cond==pd->length){
                //刚好一个包
                memcpy(pd->buffer+pd->loop,buffer,size);
                pd->loop+=size;
                size=0;
            }else{
                //一个包后还有多余的
                int remain=(pd->length-pd->loop);
                memcpy(pd->buffer+pd->loop,buffer,remain);
                pd->loop+=remain;
                buffer+=remain;
                size-=remain;
            }
            if(pd->loop==pd->length){
                int pdh=pd->length;
                pendingQueue.push(pd);
                pd=NULL;
                if(size!=0){
                    //有多余的
                    if(size<4){
                        //不够包头的长度（需要启用第二缓冲区）
                        if(pr==NULL){
                            pr=(pac_desc*)malloc(sizeof(pac_desc));
                            //开启一块足够大的内存缓冲区
                            pr->buffer=(byte*)malloc(INNER_BUFFER_SIZE);
                        }
                        pr->length=size;
                        pr->loop=size;
                    }else{
                        packet_in(buffer,size);
                    }
                }
            }
         }else{
            memcpy(pd->buffer+pd->loop,buffer,size);
            pd->loop+=size;
         }
     }
}
#endif