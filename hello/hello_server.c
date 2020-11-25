#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

/* 自定义的错误处理函数 */
void error_handling(char *message);

int main(int argc, char *argv[])
{
    /* 服务器socket文件描述符 */
    int serv_sock;

    /* 客户端socket文件描述符 */
    int clnt_sock;

    /* 服务器端地址信息结构体 */
    struct sockaddr_in serv_addr;

    /* 客户端地址信息结构体 */
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;

    char message[] = "Hello World!";

    /**
     * argc:参数个数
     * 参数个数不为2时（文件名本身也算一个参数），提示输入端口号
     */
    if(argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);/* 异常退出 */
    }

    /**
     * socket()
     * 创建socket，接收三个参数，返回socket文件描述符
     * domain:协议族，AF_INET(PF_INET)和AF_INET6(PF_INET6)，分别表示IPV4和IPV6
     * type:协议类型，SOCK_STREAM（流格式套接字/面向连接的套接字），SOCK_DGRAM（数据报套接字/无连接的套接字）
     * protocol:传输协议，IPPROTO_TCP和IPPROTO_UDP，为0时系统自动推导
     */
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    /* 创建socket失败 */
    if(serv_sock == -1)
    {
        error_handling("socket() error");
    }
    
    /* 地址信息初始化 */
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    /**
     * bind()
     * 向套接字分配网络地址
     * 传入3个参数，依次为socket文件描述符，存储地址信息的结构体变量，结构体变量的长度
     * 成功返回0，失败返回-1
     */
    if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
    {
        error_handling("bind() error");
    }

    /**
     * listen()
     * 等待连接请求
     * 接收两个参数：依次为socket文件描述符，请求等待队列的长度
     * 成功返回0，失败返回-1
     */
    if(listen(serv_sock, 5) == -1)
    {
        error_handling("listen() error");
    }

    /**
     * accept()
     * 接收三个参数
     * sock:服务器端socket文件描述符
     * addr:客户端地址信息结构体
     * addrlen:结构体的长度
     * 成功时返回文件描述符，失败时返回-1
     */
    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    if(clnt_sock == -1) 
    {
        error_handling("accept() error");
    }

    /* 将消息写入socket */
    write(clnt_sock, message, sizeof(message));
    
    /* 关闭socket */ 
    close(clnt_sock);
    close(serv_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}