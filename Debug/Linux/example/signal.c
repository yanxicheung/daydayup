#include <stdio.h>
#include <unistd.h>
#include <signal.h>
int temp = 0;

void handler_sigint(int signo)
{
	printf("recv SIGINT\n");
	sleep(5);
	temp += 1;
	printf("the value of temp is:%d\n",temp);
	printf("in handler_sigint, after sleep\n");
}

int main()
{
	struct sigaction act;
	act.sa_handler = handler_sigint;
	act.sa_flags = SA_NOMASK;
	sigaction(SIGINT, &act, NULL);
	while(1);
	return 0;
}
