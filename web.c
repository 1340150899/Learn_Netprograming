#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<pthread.h>
#include<unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
char head[] = "HTTP/1.1 200 OK\r\n"						\
				  "Content-Type: text/html\r\n"				\
				  "\r\n";
char err[]=	"HTTP/1.1 404 Not Found\r\n"			\
					"Content-Type: text/html\r\n"			\
					"\r\n"								\
					"<HTML><BODY>File not found</BODY></HTML>";
void *deal_client_fun(void *arg)//arg=new_fd
{
	int new_fd = *(int*)arg;
	
	//1、recv获取客户端的请求（只需要调用一次）
	unsigned char buf[512]="";
	recv(new_fd,buf,sizeof(buf), 0);
	
	//2、解析buf 提取所请求的文件名
	char file_name[128]="./html/";
	sscanf(buf,"GET /%[^ ]", file_name+7);
	if(file_name[7]=='\0')
		strcpy(file_name,"./html/index.html");
	
	//3、打开本地文件
	int fd = open(file_name, O_RDONLY);
	if(fd < 0)//打开文件失败
	{
		perror("open");
		//发送失败报文给客户端
		send(new_fd, err, strlen(err), 0);
	}
	else//打开本地文件成功
	{
		//发送成功报文 请准备接受
		send(new_fd, head, strlen(head), 0);
		
		//不停的给浏览器客户端 发送文件数据
		while(1)
		{
			//从本地文件读取数据
			unsigned char buf[1024]="";
			int ret = read(fd,buf,sizeof(buf));
			printf("ret=%d\n", ret);
			if(ret<1024)//文件末尾 将数据发送出去
			{
				send(new_fd,buf,ret,0);
				break;
			}
			send(new_fd,buf,ret,0);
		}
		
		//关闭本地文件描述符
		close(fd);
	}
	
	close(new_fd);
	return NULL;
}

//运行的方式：./a.out 8000
int main(int argc,char *argv[])
{
	if(argc != 2)
	{
		printf("./a.out 8000\n");
		return 0;
	}
	
	//1、创建TCP监听套接字
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		perror("socket");
		return 0;
	}
	
	//2、端口复用
	int yes = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(yes));
	
	//3、给服务器绑定固定的IP以及端口
	struct sockaddr_in my_addr;
	bzero(&my_addr,sizeof(my_addr));
	my_addr.sin_family = AF_INET;
//	my_addr.sin_port = htons(atoi(argv[1]));

	my_addr.sin_port = htons(9000);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int ret = bind(sockfd, (struct sockaddr *)&my_addr,sizeof(my_addr));
	if(ret == -1)
	{
		perror("bind");
	}
	
	//4、使用listen函数 将套接字 由主动变被动 创建连接队列
	listen(sockfd, 15);
	
	
	//5、while不停的提取客户端 产生已连接套接字
	while(1)
	{
		struct sockaddr_in cli_addr;
		socklen_t cli_len = sizeof(cli_addr);
		int new_fd = accept(sockfd,(struct sockaddr *)&cli_addr , &cli_len);
		
		//遍历客户端的信息ip port
		unsigned short port=ntohs(cli_addr.sin_port);
		char ip[16]="";
		inet_ntop(AF_INET,&cli_addr.sin_addr.s_addr, ip, 16);
		printf("已有客户端:%s:%hu连接上了服务器\n", ip, port);
		
		//创建线程 服务于客户端
		pthread_t tid;
		pthread_create(&tid,NULL, deal_client_fun, (void *)&new_fd);
		pthread_detach(tid);
	}
	
	//关闭监听套接字
	close(sockfd);
	return 0;
}
