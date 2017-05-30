#include <linux/unistd.h>

int main(void){

int fd = open("test.txt");
char word[1024];
while(read(fd,word,1024)!=0);
close(fd);
return 0;
}
