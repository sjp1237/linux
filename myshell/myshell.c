#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>


#define LEN 1024
#define NUM 32
 int main()
{
  char cmd[LEN];
  char* myarg[NUM];
  while(1)
  {
    printf("[sjp myshell]#");
    fgets(cmd,LEN,stdin);
      
    pid_t id=fork();
    myarg[0]=strtok(cmd," ");
    cmd[strlen(cmd)-1]='\0';
    int i=1;
    while(myarg[i]=strtok(NULL," "))
    {
      i++;
    }

    if(id==0)
    {
      //child
       execvp(myarg[0],myarg);
       exit(20);
    }
    int status=0;
    pid_t ret=waitpid(id,&status,0);  
  }
  return 0;
}
