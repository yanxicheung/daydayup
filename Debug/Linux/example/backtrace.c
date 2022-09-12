#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <signal.h>       /* for signal */  
#include <execinfo.h>     /* for backtrace() */ 
  
#define BACKTRACE_SIZE   16  
  
void dump(void)  
{  
    int j, nptrs;  
    void *buffer[BACKTRACE_SIZE];  
    char **strings;  
      
    nptrs = backtrace(buffer, BACKTRACE_SIZE);  
      
    printf("backtrace() returned %d addresses\n", nptrs);  
  
    strings = backtrace_symbols(buffer, nptrs);  
    if (strings == NULL) {  
        perror("backtrace_symbols");  
        exit(EXIT_FAILURE);  
    }  
  
    for (j = 0; j < nptrs; j++)  
        printf("  [%02d] %s\n", j, strings[j]);  
  
    free(strings);  
}  

void signal_handler(int signo)  
{  
    printf("\n=========>>>maps <<<=========\n");  
    char buff[64] = {0x00};        
    sprintf(buff,"cat /proc/%d/maps", getpid());          
    system((const char*) buff);  
   
    printf("\n=========>>>catch signal %d <<<=========\n", signo);  
      
    printf("Dump stack start...\n");  
    dump();  
    printf("Dump stack end...\n");  
  
    signal(signo, SIG_DFL); /* 恢复信号默认处理 */  
    raise(signo);           /* 重新发送信号 */  
} 

int add1(int num)  
{  
    printf("add1 addr %p\n",add1);
    int ret = 0x00;  
    int *pTemp = NULL;  
    *pTemp = 0x01;  /* 这将导致一个段错误，致使程序崩溃退出 */      
    ret = num + *pTemp;    
    return ret;  
}  
  
int add(int num)  
{  
    int ret = 0x00;  
    printf("add addr %p\n",add);
    ret = add1(num);     
    return ret;  
} 

int main(int argc, char *argv[])  
{  
    int sum = 0x00;        
    signal(SIGSEGV, signal_handler);  /* 为SIGSEGV信号安装新的处理函数 */    
    sum = add(sum);      
    printf(" sum = %d \n", sum);  
    while(1);
    return 0x00;  
} 
