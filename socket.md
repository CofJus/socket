## Socket编程

### 套接字类型与协议设置

#### 创建套接字

```c
#include <sys/socket.h>

/* 成功时返回文件描述符，失败时返回-1 */
int socket(int domain, int type, int protocol);
```

#### 协议族（Protocol Family）

**PF_INET:**IPV4

**PF_INET6:**IPV6

#### 套接字类型（Type）

##### 面向连接的套接字（SOCK_STREAM）

特点

- 传输过程中数据不会消失
- 按序传输数据
- 一一对应
- **传输的数据不存在数据边界（TCP粘包问题）**

##### 面向消息的套接字（SOCK_DGRAM）

特点

- 快速传输，不按顺序
- 数据可能丢失（UDP不可靠）
- 有数据边界（UDP无粘包问题）
- 限制每次传输的数据大小

#### 协议类型（Protocol）

SOCK_STREAM面向连接，在IPV4协议族中，只有IPPROTO_TCP面向连接。

SOCK_DGRAM面向消息，在IPV4协议族中，只有IPPORT_UDP面向消息。

所以第三个参数通常指定为0，系统会自动推导出协议类型。

### 网络地址的初始化与分配

#### 端口号

IP地址用于定位计算机，端口号用于定位计算机中的进程。

TCP和UDP不共享端口号，不会互相占用。

#### 地址信息的表示

##### sockaddr_in和in_addr

```c
struct sockaddr_in
{
    sa_family_t 	sin_family;  	//地址族
    uint16_t 		sin_port;		//16位TCP/UDP端口号
    struct in_addr  sin_addr;		//32位IP地址
    char 			sin_zero[8];	//填充为0，使sockaddr_in与sockaddr保持一致
};

struct in_addr
{
    In_addr_t		s_addr;			//32位IPV4地址
};
```

sockaddr和sockaddr_in的区别：后者保存IPV4地址信息，前者不只为IPV4设计。

#### 网络字节序与地址变换

##### CPU字节序

- 大端法
- 小端法

进行网络传输时，统一转换为大端序。

##### 字节序转换

- h(host)：主机
- n(network)：网络
- l(long)：long型数据，通常用于IP地址转换
- s(short)：short型数据，通常用于端口号转换

组合出四种转换字节序的函数

htons：把short型数据，从主机字节序转换到网络字节序

htonl：把long型数据，从网络字节序转换到主机字节序

ntohs：把short型数据，从主机字节序转换到网络字节序

ntohl：把long型数据，从网络字节序转换到主机字节序

**数据传输无需考虑字节序问题。**

#### 网络地址的初始化与分配

##### IP地址格式的转换

存储在sockaddr_in中的IP地址是32位整型，而人类熟悉的IP地址表示方法是**点分十进制表示法**。

```c
#include <arpa/inet.h>

/**
 * 将点分十进制表示的IP地址转换为32位整型的IP地址
 * 成功时返回32位整数，失败时返回INADDR_NONE
 * 1)检测无效IP地址 2)验证是否转换为了网络字节序
 */
in_addr_t inet_addr(const char * string);

/**
 * 功能与inet_addr相同，将转换后的IP地址直接赋给sockaddr_in结构体中的in_addr结构体变量
 * 成功返回1，失败返回0
 */
int inet_aton(const char * string, struct in_addr * addr);

/**
 * 32位整数转点分十进制IP地址
 * 成功返回字符串地址，失败返回-1
 * 调用该函数后，应立即将字符串值复制到其他内存空间，下一次调用该函数时，上一次的值将被覆盖
 */
char * inet_ntoa(struct in_addr adr);
```

##### 网络地址初始化

```c
memset(&serv_addr, 0, sizeof(serv_addr));				//清零
serv_addr.sin_family = AF_INET;							//地址族
serv_addr.sin_addr.s_addr = htonl(inet_addr(serv_ip));	//IP地址
serv_addr.sin_port = htons(atoi(serv_port));			//端口号
```

**INADDR_ANY:**利用常数分配服务器端的IP地址，自动获取服务器IP。若服务器有多个IP，只要端口号一致，可以从多个IP地址接收数据。

```c
serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
```

#### 向套接字分配网络地址

```c
#include <sys/socket.h>

/**
 * 传入3个参数，依次为socket文件描述符，存储地址信息的结构体变量，结构体变量的长度
 * 成功返回0，失败返回-1
 */
int bind(int sockfd, struct sockaddr * myaddr, socklen_t addrlen);
```

#### 进入等待连接请求状态

```c
#include <sys/socket.h>

/**
 * sock:希望进入等待状态的socket文件描述符
 * backlog:连接请求等待队列的长度
 */
int listen(int sock, int backlog);
```

#### 受理客户端请求

```c
#include <sys/socket.h>

/**
 * sock:服务器socket文件描述符
 * addr:保存客户端地址信息的结构体变量
 * addrlen:第二个参数的长度
 * 成功返回客户端socket文件描述符，失败返回-1
 */
int accept(int sock, struct sockaddr * addr, socket_t * addrlen);
```

### 断开套接字连接

#### TCP半关闭

##### close

close函数的断开是完全断开，即同时关闭接收和发送。

试想一种场景，A、B主机进行双向通信，A发送完所有数据后close，同时失去接收数据的能力，此时B尚未发送给A的数据将无法被A接收到。

##### shutdown

```c
#include <sys/socket.h>

/**
 * sock:文件描述符
 * howto:SHUT_RD(断开输入流),SHUT_WR(断开输出流),SHUTRDWR(同时断开)
 * 成功返回0，失败返回-1
 */
int shutdown(int sock, int howto);
```

SHUTRDWR相当于调用两次shutdown，两次参数分别为SHUT_RD,SHUT_WR。

### DNS

#### 使用域名的必要性

域名不常变而IP常变，在代码中使用IP地址不是一个好办法。

#### IP地址和域名的转换

##### 域名->IP

```c
#include <netdb.h>

/**
 * 域名->IP
 * 成功返回hostent结构体地址，失败返回NULL
 */
struct hostent * gethostbyname(const char * hostname);

struct hostent
{
    char * h_name;			//官方域名
    char ** h_aliases;		//其他域名
    int h_addrtype;			//IP地址类型(IPV4,IPV6)
    int h_length;			//IP地址长度
    char ** h_addr_list;	//IP地址
}
```

##### IP->域名

```c
#include <netdb.h>

/**
 * IP->域名
 * 传入三个参数，依次为IP地址，长度（IPV4时为4，IPV6时为16），地址族信息（AF_INET,AF_INET6)
 * 成功返回hostent结构体变量，失败返回NULL
 */
struct hostent * gethostbyaddr(const char * addr, socklen_t len, int family);
```

### 套接字的多种可选项

### 多进程服务器

#### 并发服务器实现模型

- 多进程服务器：通过创建多个进程提供服务
- 多路复用服务器：通过捆绑并统一管理I/O对象提供服务
- 多线程服务器：通过生成与客户端等量的线程提供服务

#### 进程

##### 进程ID

```shell
#查看当前运行的所有进程
ps au
```

##### fork函数

```c
#include <unistd.h>

/**
 * 成功时返回进程ID，失败返回-1
 */
pid_t fork(void);
```

fork函数通过复制父进程实现创建进程。

- 父进程：返回子进程ID
- 子进程：返回0

##### 僵尸进程

