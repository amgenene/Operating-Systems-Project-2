#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>

typedef struct ancestry{
	pid_t ancestors[10];
	pid_t siblings[100];
	pid_t children[100];
}ancestry;
#define __NR_cs3013_syscall2 378
long testCall2 (unsigned short *target_pid, struct ancestry *response) {
	return (long) syscall(__NR_cs3013_syscall2,target_pid,response);
}
int main(void){
	unsigned short *targPid;
	targPid = (unsigned short *)malloc(sizeof(unsigned short));
	printf("Made it in Main\n");
	ancestry *response = (ancestry *)malloc(sizeof(ancestry));
	testCall2(targPid, response);
}

