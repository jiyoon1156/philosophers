#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    int pid, child_pid, status;

    if ((pid=fork()) == -1)
       perror("fork failed");
    /* 부모 프로세스 */
    else if (pid != 0) {
       /* 자식 프로세스가 종료되기를 기다림 */
       pid = wait(&status);
       /* 정상적으로 종료되면 참이 됨 */
       if (WIFEXITED(status))
          /* 자식 프로세스의 종료 코드를 출력 */
          printf("child terminated with code %d\n", WEXITSTATUS(status));
       else
          printf("child terminated abnormally\n");
    /* 자식 프로세스 */
    } else {
       printf("run child\n");
       exit(27);   /* 자식 프로세스는 종료 코드 27을 반환하고 종료 */
    }
}
