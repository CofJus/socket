#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    pid_t pid = fork();

    if(0 == pid)
    {
        puts("Hi, I am a child process.");
    }
    else
    {
        printf("Child Process ID: %d \n", pid);
        sleep(30);
    }

    if(0 == pid)
    {
        puts("End child process.");
    }
    else
    {
        puts("End parent process.");
    }
    return 0;
}